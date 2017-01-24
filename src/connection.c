#include <stdlib.h>
#include "connection.h"
#include "input.h"
#include "output.h"

connection *connection_new(int id,
                           input *input, byte input_chan,
                           output *output, byte output_chan,
                           byte bank_msb, byte bank_lsb, byte pc_prog,
                           byte zone_low, byte zone_high,
                           byte xpose)
{
  connection *conn = malloc(sizeof(connection));
  conn->id = id;
  conn->input = input;
  conn->input_chan = input_chan;
  conn->output = output;
  conn->output_chan = output_chan;
  conn->bank_msb = bank_msb;
  conn->bank_lsb = bank_lsb;
  conn->pc_prog = pc_prog;
  conn->zone_low = zone_low;
  conn->zone_high = zone_high;
  conn->xpose = xpose;
  return conn;
}

void connection_free(connection *conn) {
  free(conn);
}

void connection_start(connection *conn, bytes *bytes) {
  // TODO bytes may be 0
}

void connection_stop(connection *conn, bytes *bytes) {
  // TODO bytes may be 0
}
