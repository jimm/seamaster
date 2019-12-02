#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "input.h"
#include "trigger.h"
#include "consts.h"

// 10 milliseconds, in nanoseconds
#define SLEEP_NANOSECS 1e7

vector<Input *> inputs;
pthread_t portmidi_thread;


void *input_thread(void *_) {
  struct timespec rqtp = {0, SLEEP_NANOSECS};

  while (true) {
    bool processed_something = false;
    for (vector<Input *>::iterator i = inputs.begin(); i != inputs.end(); ++i) {
      Input *in = *i;
      if (Pm_Poll(in->stream) == TRUE) {
        processed_something = true;
        PmEvent buf[MIDI_BUFSIZ];
        int n = Pm_Read(in->stream, buf, MIDI_BUFSIZ);
        in->read(buf, n);
      }
    }
    if (!processed_something) {
      if (nanosleep(&rqtp, 0) == -1)
        pthread_exit(0);
    }
  }

  /* NOTREACHED */
  pthread_exit(0);
}


Input::Input(const char *sym, const char *name, int port_num)
  : Instrument(sym, name, port_num)
{
  for (int i = 0; i < MIDI_CHANNELS; ++i)
    seen_progs[i].bank_msb = seen_progs[i].bank_lsb =
      seen_progs[i].prog = -1;

  if (real_port()) {
    int err = Pm_OpenInput(&stream, port_num, 0, MIDI_BUFSIZ, 0, 0);
    if (err != 0) {
      fprintf(stderr, "error opening input stream %s: %d\n", name, err);
      exit(1);
    }
  }
}

Input::~Input() {
  for (vector<Trigger *>::iterator i = triggers.begin(); i != triggers.end(); ++i)
    delete *i;
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

void Input::start() {
  if (!real_port())
    return;

  // Not thread safe, but we don't care because this method is called
  // synchronously from a single thread.
  if (inputs.size() == 0) {
    int status = pthread_create(&portmidi_thread, 0, input_thread, 0);
    if (status != 0) {
      fprintf(stderr, "error creating input stream thread %s: %d\n",
              name.c_str(), status);
      exit(1);
    }
  }
  inputs.push_back(this);
}

void Input::stop() {
  for (vector<Input *>::iterator i = inputs.begin(); i != inputs.end(); ++i) {
    if (*i == this) {
      inputs.erase(i);
      return;
    }
  }
}

void Input::read(PmEvent *buf, int len) {
  // triggers
  for (vector<Trigger *>::iterator i = triggers.begin(); i != triggers.end(); ++i)
    (*i)->signal(buf, len);

  for (int i = 0; i < len; ++i) {
    PmMessage msg = buf[i].message;

    // when testing, remember the messages we've seen. this could be made
    // more efficient by doing a bulk copy before or after this for loop,
    // making sure not to copy over the end of received_messages.
    if (!real_port() && num_io_messages < MIDI_BUFSIZ-1)
      io_messages[num_io_messages++] = msg;

    remember_program_change_messages(msg);

    vector<Connection *> &conns = connections_for_message(msg);
    for (vector<Connection *>::iterator i = conns.begin(); i != conns.end(); ++i)
      (*i)->midi_in(msg);
  }
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

// Return connections to use for `msg`. Normally it's the same as our list
// of connections. However for every note on we store those connections, and
// use those connections for the corresponding note off. Same for sustain
// controller messages.
vector<Connection *> &Input::connections_for_message(PmMessage msg) {
  unsigned char status = Pm_MessageStatus(msg);
  unsigned char high_nibble = status & 0xf0;
  unsigned char chan = status & 0x0f;
  unsigned char data1 = Pm_MessageData1(msg);

  // note off messages must be sent to their original connections, so for
  // incoming note on messages we store the current connections in
  // note_off_conns.
  switch (high_nibble) {
  case NOTE_OFF:
    return notes_off_conns[chan][data1];
  case NOTE_ON:
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
