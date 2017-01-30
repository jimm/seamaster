#include <stdlib.h>
#include <string.h>
#include "input.h"
#include "trigger.h"
#include "consts.h"
#include "debug.h"

#define MIDI_BUFSIZ 128

bool input_real_port(input *);
void *input_thread(void *);

input *input_new(char *sym, char *name, int port_num) {
  input *in = malloc(sizeof(input));

  in->sym = malloc(strlen(sym) + 1);
  strcpy(in->sym, sym);
  in->name = malloc(strlen(name) + 1);
  strcpy(in->name, name);

  in->port_num = port_num;
  in->running = false;
  in->portmidi_thread = 0;

  in->connections = list_new();
  in->triggers = list_new();
  for (int chan = 0; chan < 16; ++chan)
    for (int note = 0; note < 128; ++note)
      in->notes_off_conns[chan][note] = list_new();

  in->messages = list_new();
  in->received_messages = list_new();

  if (input_real_port(in)) {
    int err = Pm_OpenInput(&in->stream, port_num, 0, 128, 0, 0);
    // TODO check error
  }

  return in;
}

void input_free(input *in) {
  if (input_real_port(in))
    Pm_Close(in->stream);
  free(in->sym);
  free(in->name);
  list_free(in->connections, 0);
  list_free(in->triggers, 0);
  for (int chan = 0; chan < 16; ++chan)
    for (int note = 0; note < 128; ++note)
      list_free(in->notes_off_conns[chan][note], 0);
  list_free(in->messages, 0);
  list_free(in->received_messages, 0);
  free(in);
}

bool input_real_port(input *in) {
  return in->port_num != pmNoDevice;
}

void input_add_connection(input *in, connection *conn) {
  debug("input %p adding connection %p\n", in, conn);
  list_append(in->connections, conn);
}

void input_remove_connection(input *in, connection *conn) {
  debug("input %p removing connection %p\n", in, conn);
  list_remove(in->connections, conn);
}

void input_add_trigger(input *in, trigger *trigger) {
  list_append(in->triggers, trigger);
}

void input_remove_trigger(input *in, trigger *trigger) {
  list_remove(in->triggers, trigger);
}

void input_start(input *in) {
  int status;

  debug("input_start\n");
  in->running = true;
  if (input_real_port(in)) {
    status = pthread_create(&in->portmidi_thread, 0, input_thread, in);
    // TODO check status
  }
}

void input_stop(input *in) {
  debug("input_stop\n");
  if (in->running)
    in->running = false;
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wint-to-void-pointer-cast"

void input_read(input *in, PmEvent *buf, int len) {
  debug("input_read %d events\n", len);

  // Extract messages and split into note off and non-note-off.
  list_clear(in->messages, 0);
  for (int i = 0; i < len; ++i) {
    void *msg = (void *)buf[i].message;
    list_append(in->messages, msg);
    if (!input_real_port(in))
      list_append(in->received_messages, msg);
  }

  // triggers
  for (int i = 0; i < list_length(in->triggers); ++i)
    trigger_signal(list_at(in->triggers, i), in->messages);

  for (int i = 0; i < len; ++i) {
    PmMessage msg = buf[i].message;
    unsigned char status = Pm_MessageStatus(msg);
    unsigned char high_nibble = status & 0xf0;
    unsigned char chan = status & 0x0f;
    unsigned char note = Pm_MessageData1(msg);

    // note off messages must be sent to their original connections, so for
    // incoming note on messages we store the current connections in
    // in->note_off_conns.
    list *conns;
    switch (high_nibble) {
    case NOTE_OFF:
      conns = in->notes_off_conns[chan][note];
      break;
    case NOTE_ON:
      conns = in->connections;
      list_copy(in->notes_off_conns[chan][note], conns);
      break;
    default:
      conns = in->connections;
      break;
    }

    for (int j = 0; j < list_length(conns); ++j)
      connection_midi_in(list_at(conns, i), msg);
  }
}

#pragma clang diagnostic pop

void *input_thread(void *in_voidptr) {
  input *in = (input *)in_voidptr;
  while (in->running) {
    if (Pm_Poll(in->stream) == TRUE) {
      PmEvent buf[MIDI_BUFSIZ];
      int n = Pm_Read(in->stream, buf, MIDI_BUFSIZ);
      debug("%d events seen, sending to input_read\n", n);
      input_read(in, buf, n);
    }
  }

  debug("input exiting\n");
  pthread_exit(0);
}

void input_debug(input *in) {
  if (in == 0) {
    debug("input NULL\n");
    return;
  }

  debug("input %s %s (%p)\n", in->sym, in->name, in);
  debug("  port_num %d stream %p\n", in->port_num, in->stream);
  debug("  connections:");
  for (int i = 0; i < list_length(in->connections); ++i)
    debug("    %p\n", list_at(in->connections, i));
  debug("  running: %d\n", in->running);
  debug("  thread: %p\n", in->portmidi_thread);
}

// only used during testing
void input_clear(input *in) {
  list_clear(in->received_messages, 0);
}
