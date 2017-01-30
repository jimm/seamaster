#include <stdlib.h>
#include "consts.h"
#include "connection.h"
#include "input.h"
#include "output.h"
#include "debug.h"

int accept_from_input(connection *conn, PmMessage msg);
int inside_zone(connection *conn, PmMessage msg);
void midi_out(connection *conn, PmMessage msg);

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
  for (int i = 0; i < 128; ++i)
    conn->cc_maps[i] = i;
  return conn;
}

void connection_free(connection *conn) {
  free(conn);
}

void connection_start(connection *conn, list *start_messages) {
  debug("connection_start %p\n", conn);
  for (int i = 0; i < list_length(start_messages); ++i)
    midi_out(conn, (PmMessage)list_at(start_messages, i));
  if (conn->prog.bank_msb >= 0)
    midi_out(conn, Pm_Message(CONTROLLER + conn->output_chan, CC_BANK_SELECT_MSB, conn->prog.bank_msb));
  if (conn->prog.bank_lsb >= 0)
    midi_out(conn, Pm_Message(CONTROLLER + conn->output_chan, CC_BANK_SELECT_LSB, conn->prog.bank_lsb));
  if (conn->prog.prog >= 0)
    midi_out(conn, Pm_Message(PROGRAM_CHANGE + conn->output_chan, conn->prog.prog, 0));

  input_add_connection(conn->input, conn);
}

void connection_stop(connection *conn, list *stop_messages) {
  debug("connection_stop %p\n", conn);
  for (int i = 0; i < list_length(stop_messages); ++i)
    midi_out(conn, (PmMessage)list_at(stop_messages, i));
  input_remove_connection(conn->input, conn);
}

void connection_midi_in(connection *conn, PmMessage msg) {
  debug("connection_midi_in %p, message %p\n", conn, msg);
  if (!accept_from_input(conn, msg))
      return;

  int status = Pm_MessageStatus(msg);
  int high_nibble = status & 0xf0;
  int data1 = Pm_MessageData1(msg);
  int data2 = Pm_MessageData2(msg);

  switch (high_nibble) {
  case NOTE_ON: case NOTE_OFF: case POLY_PRESSURE:
    if (!inside_zone(conn, msg))
      return;
    if (conn->output_chan != -1)
      status = high_nibble + conn->output_chan;
    data1 += conn->xpose;
    midi_out(conn, Pm_Message(status, data1, data2));
    break;
  case CONTROLLER: case PROGRAM_CHANGE: case CHANNEL_PRESSURE: case PITCH_BEND:
    if (conn->output_chan != -1)
      status = high_nibble + conn->output_chan;
    if (high_nibble == CONTROLLER) /* map controller number */
      data1 = conn->cc_maps[data1]; /* won't be -1, that's already filtered */
    midi_out(conn, Pm_Message(status, data1, data2));
    break;
  default:
    midi_out(conn, msg);
    break;
  }
}

int accept_from_input(connection *conn, PmMessage msg) {
  if (conn->input_chan == -1)
    return true;
  unsigned char status = Pm_MessageStatus(msg);

  if (status >= SYSEX)
    return true;
  if (conn->input_chan != (Pm_MessageStatus(msg) & 0x0f))
    return false;

  if ((status & 0xf0) == CONTROLLER) {
    unsigned char controller = Pm_MessageData1(msg);
    if (conn->cc_maps[controller] == -1)
      return false;
  }
  return true;
}

int inside_zone(connection *conn, PmMessage msg) {
  int note = Pm_MessageData1(msg);
  if (note < conn->zone.low)
    return false;
  if (conn->zone.high != -1 && note > conn->zone.high)
    return false;
  return true;
}

void midi_out(connection *conn, PmMessage message) {
  debug("connection_midi_out %p, message %p\n", conn, message);
  PmEvent event = {message, 0};
  output_write(conn->output, &event, 1);
}

void connection_debug(connection *conn) {
  if (conn == 0) {
    debug("conn NULL\n");
    return;
  }

  debug("conn (%p), in [%s, %d], out [%s, %d], prog [%d, %d, %d], zone [%d, %d], xpose %d\n",
        conn,
        conn->input ? conn->input->name : "(null input)",
        conn->input ? conn->input_chan : 0,
        conn->output ? conn->output->name : "(null output)",
        conn->output ? conn->output_chan : 0,
        conn->prog.bank_msb, conn->prog.bank_lsb, conn->prog.prog,
        conn->zone.low, conn->zone.high,
        conn->xpose);
}
