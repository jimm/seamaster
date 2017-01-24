#ifndef CONNECTION_H
#define CONNECTION_H

#include "list.h"

typedef struct input input;
typedef struct output output;

typedef struct program {
  int bank_msb;
  int bank_lsb;
  int prog;
} program;

typedef struct zone {
  int low;
  int high;
} zone;

typedef struct connection {
  int id;
  input *input;
  output *output;
  int input_chan;
  int output_chan;
  program prog;
  zone zone;
  int xpose;
  // TODO filter
} connection;

connection *connection_new(int id,
                           input *input, int input_chan,
                           output *output, int output_chan,
                           program prog, zone zone, int xpose);
void connection_free(connection *conn);

void connection_start(connection *, list *);
void connection_stop(connection *, list *);


#endif /* CONNECTION_H */
