#include <stdlib.h>
#ifdef DEBUG
#include <stdio.h>
#endif
#include "consts.h"
#include "connection.h"
#include "input.h"
#include "output.h"

int accept_from_input(connection *conn, PmMessage msg);
int inside_zone(connection *conn, PmMessage msg);
void do_midi_in(connection *conn, PmMessage msg, list *out_msgs);
void midi_out(connection *conn, list *messages);

connection *connection_new(input *input, int input_chan,
                           output *output, int output_chan)
{
  connection *conn = malloc(sizeof(connection));
  conn->input = input;
  conn->input_chan = input_chan;
  conn->output = output;
  conn->output_chan = output_chan;
  conn->prog.bank_msb = conn->prog.bank_lsb = conn->prog.prog = -1;
  conn->zone.low = conn->zone.high = -1;
  conn->xpose = 0;
  return conn;
}

void connection_free(connection *conn) {
  free(conn);
}

void connection_start(connection *conn, list *start_messages) {
  list *msgs = list_new();
  list_append_list(msgs, start_messages);
  if (conn->prog.bank_msb >= 0)
    list_append(msgs, (void *)Pm_Message(CONTROLLER + conn->output_chan, CC_BANK_SELECT_MSB, conn->prog.bank_msb));
  if (conn->prog.bank_lsb >= 0)
    list_append(msgs, (void *)Pm_Message(CONTROLLER + conn->output_chan, CC_BANK_SELECT_LSB, conn->prog.bank_lsb));
  if (conn->prog.prog >= 0)
    list_append(msgs, (void *)Pm_Message(PROGRAM_CHANGE + conn->output_chan, conn->prog.prog, 0));
  midi_out(conn, msgs);
  list_free(msgs, 0);

  input_add_connection(conn->input, conn);
}

void connection_stop(connection *conn, list *stop_messages) {
  midi_out(conn, stop_messages);
  input_remove_connection(conn->input, conn);
}

void connection_midi_in(connection *conn, list *messages) {
  list *out_msgs = list_new();
  for (int i = 0; i < list_length(messages); ++i) {
    PmMessage msg = (PmMessage)list_at(messages, i);
    if (accept_from_input(conn, msg))
      do_midi_in(conn, msg, out_msgs);
  }
  list_free(out_msgs, 0);
}

void do_midi_in(connection *conn, PmMessage msg, list *out_msgs) {
  int status = Pm_MessageStatus(msg);
  int high_nibble = status & 0xf0;
  int data1 = Pm_MessageData1(msg);
  int data2 = Pm_MessageData2(msg);

  switch (high_nibble) {
  case NOTE_ON: case NOTE_OFF: case POLY_PRESSURE:
    if (inside_zone(conn, msg)) {
      if (conn->output_chan != -1)
        status = high_nibble + conn->output_chan;
      data1 += conn->xpose;
      list_append(out_msgs, (void *)Pm_Message(status, data1, data2));
    }
    break;
  case CONTROLLER: case PROGRAM_CHANGE: case CHANNEL_PRESSURE: case PITCH_BEND:
    if (conn->output_chan != -1)
      status = high_nibble + conn->output_chan;
    list_append(out_msgs, (void *)Pm_Message(status, data1, data2));
    break;
  default:
    list_append(out_msgs, (void *)msg);
    break;
  }
}

int accept_from_input(connection *conn, PmMessage msg) {
  if (conn->input_chan == -1)
    return true;
  if (Pm_MessageStatus(msg) >= SYSEX)
    return true;
  return conn->input_chan == (Pm_MessageStatus(msg) & 0x0f);
}

int inside_zone(connection *conn, PmMessage msg) {
  int note = Pm_MessageData1(msg);
  if (note < conn->zone.low)
    return false;
  if (conn->zone.high != -1 && note > conn->zone.high)
    return false;
  return true;
}

void midi_out(connection *conn, list *messages) {
  int num_messages = list_length(messages);
  if (num_messages == 0)
    return;

  PmEvent *events = calloc(num_messages, sizeof(PmEvent));

  for (int i = 0; i < num_messages; ++i)
    events[i].message = (PmMessage)list_at(messages, i);
  output_write(conn->output, events, list_length(messages));

  free(events);
}

#ifdef DEBUG

void connection_debug(connection *c) {
  fprintf(stderr, "conn %s, %d, %s, %d\n",
          c->input->name, c->input_chan,
          c->output->name, c->output_chan);
}

#endif
