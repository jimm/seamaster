#include <stdlib.h>
#include <string.h>
#include "input.h"
#include "trigger.h"
#include "consts.h"
#include "debug.h"

void *input_thread(void *);

Input::Input(const char *sym, const char *name, int port_num)
  : Instrument(sym, name, port_num)
{
  running = false;
  portmidi_thread = 0;

  connections = list_new();
  triggers = list_new();
  for (int chan = 0; chan < MIDI_CHANNELS; ++chan)
    for (int note = 0; note < NOTES_PER_CHANNEL; ++note)
      notes_off_conns[chan][note] = list_new();

  if (real_port()) {
    int err = Pm_OpenInput(&stream, port_num, 0, MIDI_BUFSIZ, 0, 0);
    // TODO check error
  }
}

Input::~Input() {
  list_free(connections, 0);
  list_free(triggers, 0);
  for (int chan = 0; chan < 16; ++chan)
    for (int note = 0; note < 128; ++note)
      list_free(notes_off_conns[chan][note], 0);
}

void Input::add_connection(Connection *conn) {
  vdebug("input %p adding connection %p\n", this, conn);
  list_append(connections, conn);
}

void Input::remove_connection(Connection *conn) {
  vdebug("input %p removing connection %p\n", this, conn);
  list_remove(connections, conn);
}

void Input::add_trigger(trigger *trigger) {
  list_append(triggers, trigger);
}

void Input::remove_trigger(trigger *trigger) {
  list_remove(triggers, trigger);
}

void Input::start() {
  int status;

  vdebug("input_start\n");
  running = true;
  if (real_port()) {
    status = pthread_create(&portmidi_thread, 0, input_thread, this);
    // TODO check status
  }
}

void Input::stop() {
  vdebug("input_stop\n");
  if (running)
    running = false;
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wint-to-void-pointer-cast"

void Input::read(PmEvent *buf, int len) {
  vdebug("input_read %d events\n", len);

  // triggers
  for (int i = 0; i < list_length(triggers); ++i)
    trigger_signal((trigger *)list_at(triggers, i), buf, len);

  for (int i = 0; i < len; ++i) {
    PmMessage msg = buf[i].message;

    // when testing, remember the messages we've seen. this could be made
    // more efficient by doing a bulk copy before or after this for loop,
    // making sure not to copy over the end of received_messages.
    if (!real_port() && num_io_messages < MIDI_BUFSIZ-1)
      io_messages[num_io_messages++] = msg;

    unsigned char status = Pm_MessageStatus(msg);
    unsigned char high_nibble = status & 0xf0;
    unsigned char chan = status & 0x0f;
    unsigned char note = Pm_MessageData1(msg);

    // note off messages must be sent to their original connections, so for
    // incoming note on messages we store the current connections in
    // note_off_conns.
    list *conns;
    switch (high_nibble) {
    case NOTE_OFF:
      conns = notes_off_conns[chan][note];
      break;
    case NOTE_ON:
      conns = connections;
      list_copy(notes_off_conns[chan][note], conns);
      break;
    default:
      conns = connections;
      break;
    }

    for (int j = 0; j < list_length(conns); ++j) {
      Connection *conn = (Connection *)list_at(conns, i);
      conn->midi_in(msg);
    }
  }
}

#pragma clang diagnostic pop

void *input_thread(void *in_voidptr) {
  Input *in = (Input *)in_voidptr;
  while (in->running) {
    if (Pm_Poll(in->stream) == TRUE) {
      PmEvent buf[MIDI_BUFSIZ];
      int n = Pm_Read(in->stream, buf, MIDI_BUFSIZ);
      vdebug("%d events seen, sending to Input::read\n", n);
      in->read(buf, n);
    }
  }

  vdebug("input exiting\n");
  pthread_exit(0);
}

void Input::debug() {
  Instrument::debug();
  vdebug("  ...is an input\n");
  vdebug("  connections:");
  for (int i = 0; i < list_length(connections); ++i)
    vdebug("    %p\n", list_at(connections, i));
  vdebug("  running: %d\n", running);
  vdebug("  thread: %p\n", portmidi_thread);
}
