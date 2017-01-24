#ifndef OUTPUT_H
#define OUTPUT_H

#include <portmidi.h>
#include "list.h"

typedef struct output {
  int id;
  char *name;
  char *sym;
  int port_num;
  PortMidiStream *stream;
} output;

output *output_new(int id, char *sym, char *name, int port_num);
void output_free(output *);

void output_start(output *);
void output_stop(output *);

void output_write(output *, PmEvent *buf, int len);

#endif /* OUTPUT_H */
