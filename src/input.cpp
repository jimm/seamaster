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

  if (real_port()) {
    int err = Pm_OpenInput(&stream, port_num, 0, MIDI_BUFSIZ, 0, 0);
    // TODO check error
  }
}

Input::~Input() {
  for (int i = 0; i < triggers.length(); ++i)
    delete triggers[i];
}

void Input::add_connection(Connection *conn) {
  vdebug("input %p adding connection %p\n", this, conn);
  connections << conn;
}

void Input::remove_connection(Connection *conn) {
  vdebug("input %p removing connection %p\n", this, conn);
  connections.remove(conn);
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

void Input::read(PmEvent *buf, int len) {
  vdebug("input_read %d events\n", len);

  // triggers
  for (int i = 0; i < triggers.length(); ++i)
    triggers[i]->signal(buf, len);

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
    List<Connection *> conns;
    switch (high_nibble) {
    case NOTE_OFF:
      conns = notes_off_conns[chan][note];
      break;
    case NOTE_ON:
      conns = connections;
      notes_off_conns[chan][note] = connections;
      break;
    default:
      conns = connections;
      break;
    }

    for (int j = 0; j < conns.length(); ++j)
      conns[j]->midi_in(msg);
  }
}

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
  for (int i = 0; i < connections.length(); ++i)
    vdebug("    %p\n", connections[i]);
  vdebug("  running: %d\n", running);
  vdebug("  thread: %p\n", portmidi_thread);
  triggers.debug("input triggers");
}
