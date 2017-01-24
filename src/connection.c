#include <stdlib.h>
#ifdef DEBUG
#include <stdio.h>
#endif
#include "consts.h"
#include "connection.h"
#include "input.h"
#include "output.h"

void midi_in(connection *conn, list *messages);
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

void midi_in(connection *conn, list *messages) {
  // TODO
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
