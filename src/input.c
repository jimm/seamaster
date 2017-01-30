#include <stdlib.h>
#include <string.h>
#include "input.h"
#include "trigger.h"
#include "consts.h"
#include "debug.h"

#define MIDI_BUFSIZ 128

void input_remember_note(input *, PmMessage);
void *input_thread(void *);
void input_poll_and_send(input *);

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

  int err = Pm_OpenInput(&in->stream, port_num, 0, 128, 0, 0);
  // TODO check error

  return in;
}

void input_free(input *in) {
  Pm_Close(in->stream);
  free(in->sym);
  free(in->name);
  list_free(in->connections, 0);
  list_free(in->triggers, 0);
  for (int chan = 0; chan < 16; ++chan)
    for (int note = 0; note < 128; ++note)
      list_free(in->notes_off_conns[chan][note], 0);
  free(in);
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
  status = pthread_create(&in->portmidi_thread, 0, input_thread, in);
  // TODO check status
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
  list *note_off_messages = list_new();
  list *other_messages = list_new();

  // Extract messages and split into note off and non-note-off.
  for (int i = 0; i < len; ++i) {
    PmMessage msg = buf[i].message;
    if (Pm_MessageStatus(msg) < NOTE_ON) /* note off */
      list_append(note_off_messages, (void *)msg);
    else
      list_append(other_messages, (void *)msg);
    input_remember_note(in, msg);
  }

  // triggers
  for (int i = 0; i < list_length(in->triggers); ++i)
    trigger_signal(list_at(in->triggers, i), other_messages);

  // note off messages must be sent to their original connections, stored in
  // in->note_off_conns.
  if (list_length(note_off_messages) > 0) {
    list *one_note_off_message = list_new();
    list_append(one_note_off_message, 0); /* reserve space for the one message */
    for (int i = 0; i < list_length(note_off_messages); ++i) {
      PmMessage msg = (PmMessage)list_at(note_off_messages, i);
      list_at_set(one_note_off_message, 0, (void *)msg);

      list *conns = in->notes_off_conns[Pm_MessageStatus(msg) & 0x0f][Pm_MessageData1(msg)];
      for (int j = 0; j < list_length(conns); ++j)
        connection_midi_in(list_at(conns, i), one_note_off_message);
    }
    list_free(one_note_off_message, 0);
  }

  // non-note-off messages
  for (int i = 0; i < list_length(in->connections); ++i)
    connection_midi_in(list_at(in->connections, i), other_messages);

  list_free(other_messages, 0);
}

void input_remember_note(input *in, PmMessage msg) {
  // remember notes on, un-remember notes off
  unsigned char status = Pm_MessageStatus(msg);
  if (status >= POLY_PRESSURE)  /* return unless note on or note off */
    return;

  unsigned char high_nibble = status & 0xf0;
  unsigned char chan = status & 0x0f;
  unsigned char note = Pm_MessageData1(msg);
  if (high_nibble == NOTE_ON)
    list_copy(in->notes_off_conns[chan][note], in->connections);
  else if (high_nibble == NOTE_OFF)
    list_clear(in->notes_off_conns[chan][note], 0);
}

#pragma clang diagnostic pop

void *input_thread(void *in_voidptr) {
  input *in = (input *)in_voidptr;
  while (in->running)
    input_poll_and_send(in);

  // TODO keep sending note offs for notes we've marked as still on in
  // in->notes_on. See notes in README.

  /* int num_note_offs_to_expect = in->num_notes_on; */
  /* unsigned char notes_on[16][128]; */
  /* memcpy(notes_on, in->notes_on, 16 * 128); */

  /* bool done = num_note_offs_to_expect == 0; */
  /* while (!done) { */
  /*   input_poll_and_send(in); */
  /* } */

  debug("input exiting\n");
  pthread_exit(0);
}

void input_poll_and_send(input *in) {
  if (Pm_Poll(in->stream) == TRUE) {
    PmEvent buf[MIDI_BUFSIZ];
    int n = Pm_Read(in->stream, buf, MIDI_BUFSIZ);
    debug("%d events seen, sending to input_read\n", n);
    input_read(in, buf, n);
  }
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
