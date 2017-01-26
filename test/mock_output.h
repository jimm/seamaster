#ifndef OUTPUT_H
#define OUTPUT_H

#include <portmidi.h>
#include "../src/list.h"

typedef struct output {
  char *name;
  char *sym;
  int port_num;
  PortMidiStream *stream;
  list *sent_messages;
} output;

output *output_new(char *sym, char *name, int port_num);
void output_free(output *);

void output_write(output *, PmEvent *buf, int len);

void output_debug(output *);

void output_clear(output *);

#endif /* OUTPUT_H */