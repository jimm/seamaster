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
  input *input;
  output *output;
  int input_chan;
  int output_chan;
  program prog;
  zone zone;
  int xpose;
  // TODO filter
} connection;

connection *connection_new(input *input, int input_chan,
                           output *output, int output_chan);
void connection_free(connection *conn);

void connection_start(connection *, list *);
void connection_stop(connection *, list *);

#ifdef DEBUG
void connection_debug(connection *);
#endif

#endif /* CONNECTION_H */
