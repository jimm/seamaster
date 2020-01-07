#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "error.h"
#include "input.h"
#include "trigger.h"
#include "consts.h"

// 10 milliseconds, in nanoseconds
#define SLEEP_NANOSECS 10000000L

vector<Input *> inputs;
pthread_t portmidi_pthread = 0;


// For each running input in `inputs`, sees if there is any MIDI data to be
// read. If so, reads as much as is available and tells the input to enqueue
// the data for processing in a separate thread.
//
// When no data is available for any input, sleeps for `SLEEP_NANOSECS`.
void *input_thread(void *_) {
  struct timespec rqtp = {0, SLEEP_NANOSECS};

  while (true) {
    bool processed_something = false;
    for (auto& in : inputs) {
      if (in->running && Pm_Poll(in->stream) == TRUE) {
        PmEvent buf[MIDI_BUFSIZ];
        int n = Pm_Read(in->stream, buf, MIDI_BUFSIZ);
        if (n > 0) {
          processed_something = true;
          in->enqueue(buf, n);
        }
      }
    }
    if (!processed_something) {
      if (nanosleep(&rqtp, 0) == -1)
        return 0;
    }
  }
}

// While the Input pointed to by `in_voidptr` is running, take PmMessages
// from its input queue and process them. If the queue is empty, sleeps for
// `SLEEP_NANOSECS` before looking in the queue again.
void *read_thread(void *in_voidptr) {
  Input *in = (Input *)in_voidptr;
  struct timespec rqtp = {0, SLEEP_NANOSECS};

  while (in->running) {
    PmMessage msg = in->message_from_read_queue();
    if (msg != 0)
      in->read(msg);
    else {
      if (nanosleep(&rqtp, 0) == -1)
        return 0;
    }
  }
  return 0;
}


Input::Input(const char *sym, const char *name, int port_num)
  : Instrument(sym, name, port_num), running(false), read_pthread(0)
{
  for (int i = 0; i < MIDI_CHANNELS; ++i)
    seen_progs[i].bank_msb = seen_progs[i].bank_lsb =
      seen_progs[i].prog = -1;

  if (real_port()) {
    PmError err = Pm_OpenInput(&stream, port_num, 0, MIDI_BUFSIZ, 0, 0);
    if (err != 0) {
      char buf[BUFSIZ];
      sprintf(buf, "error opening input stream %s: %s\n", name,
              Pm_GetErrorText(err));
      error_message(buf);
      exit(1);
    }
    err = Pm_SetFilter(stream, PM_FILT_ACTIVE); // TODO cmd line option to enable
    if (err != 0) {
      char buf[BUFSIZ];
      sprintf(buf, "error setting PortMidi filter for input %s: %s\n", name,
              Pm_GetErrorText(err));
      error_message(buf);
      exit(1);
    }
  }
}

Input::~Input() {
  for (auto& trigger : triggers)
    delete trigger;
}

void Input::add_connection(Connection *conn) {
  connections.push_back(conn);
}

void Input::remove_connection(Connection *conn) {
  for (vector<Connection *>::iterator i = connections.begin(); i != connections.end(); ++i) {
    if (*i == conn) {
      connections.erase(i);
      return;
    }
  }
}

// Lazily starts the `input_thread` if needed. Sets `running` to `true` and
// starts a `read_thread` for this Input.
void Input::start() {
  int status;

  if (!real_port())
    return;

  // Not thread safe, but we don't care because this method is called
  // synchronously from a single thread.
  if (portmidi_pthread == 0) {
    status = pthread_create(&portmidi_pthread, 0, input_thread, 0);
    if (status != 0) {
      char buf[BUFSIZ];
      sprintf(buf, "error creating global input stream thread %s: %d\n",
              name.c_str(), status);
      error_message(buf);
      exit(1);
    }
  }
  inputs.push_back(this);

  running = true;
  status = pthread_create(&read_pthread, 0, read_thread, this);
  if (status != 0) {
    char buf[BUFSIZ];
    sprintf(buf, "error creating input read thread %s: %d\n",
            name.c_str(), status);
    error_message(buf);
    exit(1);
  }
}

// Sets `running` to `false`, which will cause the `read_thread` for this
// Input to exit. Also removes the Input from `inputs`.
void Input::stop() {
  running = false;
  read_pthread = 0;
  for (vector<Input *>::iterator i = inputs.begin(); i != inputs.end(); ++i) {
    if (*i == this) {
      inputs.erase(i);
      return;
    }
  }
}

// Adds all the PmMessages in `events` to our message queue in a thread-safe
// manner.
void Input::enqueue(PmEvent *events, int num) {
  message_queue_mutex.lock();
  for (int i = 0; i < num; ++i)
    message_queue.push(events[i].message);
  message_queue_mutex.unlock();
}

void Input::read(PmMessage msg) {
  // triggers
  for (auto& trigger : triggers)
    trigger->signal(msg);

  // When testing, remember the messages we've seen. This could be made
  // more efficient by doing a bulk copy before or after this for loop,
  // making sure not to copy over the end of received_messages.
  if (!real_port() && num_io_messages < MIDI_BUFSIZ-1)
    io_messages[num_io_messages++] = msg;

  remember_program_change_messages(msg);

  for (auto &conn : connections_for_message(msg))
    conn->midi_in(msg);
}

// Removes a single PmMessages from our message queue in a thread-safe
// manner and returns it.
PmMessage Input::message_from_read_queue() {
  PmMessage msg = 0;
  message_queue_mutex.lock();
  if (!message_queue.empty()) {
    msg = message_queue.front();
    message_queue.pop();
  }
  message_queue_mutex.unlock();
  return msg;
}

void Input::remember_program_change_messages(PmMessage msg) {
  unsigned char status = Pm_MessageStatus(msg);
  unsigned char high_nibble = status & 0xf0;
  unsigned char chan = status & 0x0f;
  unsigned char data1 = Pm_MessageData1(msg);

  switch (high_nibble) {
  case PROGRAM_CHANGE:
    seen_progs[chan].prog = data1;
    break;
  case CONTROLLER:
    switch (data1) {
    case CC_BANK_SELECT_MSB:
      seen_progs[chan].bank_msb = Pm_MessageData2(msg);
      break;
    case CC_BANK_SELECT_LSB:
      seen_progs[chan].bank_lsb = Pm_MessageData2(msg);
      break;
    }
  }
}

// Return the connections to use for `msg`. Normally it's the same as our
// list of connections. However for every note on we store those connections
// so we can use them later for the corresponding note off. Same for sustain
// controller messages.
vector<Connection *> &Input::connections_for_message(PmMessage msg) {
  unsigned char status = Pm_MessageStatus(msg);
  unsigned char high_nibble = status & 0xf0;
  unsigned char chan = status & 0x0f;
  unsigned char data1 = Pm_MessageData1(msg);

  // Note off messages must be sent to their original connections, so for
  // incoming note on messages we store the current connections in
  // note_off_conns.
  switch (high_nibble) {
  case NOTE_OFF:
    return notes_off_conns[chan][data1];
  case NOTE_ON:
    // Velocity 0 means we should use the note-off connections.
    if (Pm_MessageData2(msg) == 0)
      return notes_off_conns[chan][data1];
    notes_off_conns[chan][data1] = connections;
    return connections;
  case CONTROLLER:
    if (data1 == CC_SUSTAIN) {
      if (Pm_MessageData2(msg) == 0)
        return sustain_off_conns[chan];
      else {
        sustain_off_conns[chan] = connections;
        return connections;
      }
    }
    else
      return connections;
    break;
  default:
    return connections;
  }
}
