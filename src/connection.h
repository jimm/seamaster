#ifndef CONNECTION_H
#define CONNECTION_H

#include "types.h"
#include "bytes.h"

typedef struct input input;
typedef struct output output;

typedef struct connection {
  int id;
  input *input;
  output *output;
  int input_chan;
  int output_chan;
  int bank_msb;
  int bank_lsb;
  int pc_prog;
  int zone_low;
  int zone_high;
  int xpose;
  // TODO filter
} connection;

connection *connection_new(int id,
                           input *input, byte input_chan,
                           output *output, byte output_chan,
                           byte bank_msb, byte bank_lsb, byte pc_prog,
                           byte zone_low, byte zone_high,
                           byte xpose);
void connection_free(connection *conn);

void connection_start(connection *, bytes *);
void connection_stop(connection *, bytes *);


#endif /* CONNECTION_H */
