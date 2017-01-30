#ifndef OUTPUT_H
#define OUTPUT_H

#include <portmidi.h>
#include "list.h"
#include "input.h"

typedef struct output {
  char *name;
  char *sym;
  int port_num;
  PortMidiStream *stream;

  PmMessage sent_messages[MIDI_BUFSIZ]; // testing only
  int num_sent_messages;
} output;

output *output_new(char *sym, char *name, int port_num);
void output_free(output *);

void output_write(output *, PmEvent *buf, int len);

void output_debug(output *);
void output_clear(output *);    // testing only

#endif /* OUTPUT_H */
