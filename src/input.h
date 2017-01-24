#ifndef INPUT_H
#define INPUT_H

#include <pthread.h>
#include <portmidi.h>
#include "list.h"
#include "connection.h"
#include "trigger.h"

typedef struct input {
  int id;
  char *name;
  char *sym;
  int port_num;
  PortMidiStream *stream;
  list *connections;
  list *triggers;
  bool running;
  pthread_t portmidi_thread;
} input;

input *input_new(int id, char *sym, char *name, int port_num);
void input_free(input *);

void input_add_connection(input *, connection *);
void input_remove_connection(input *, connection *);

void input_add_trigger(input *, trigger *);
void input_remove_trigger(input *, trigger *);

void input_start(input *);
void input_stop(input *);

void input_read(input *, PmEvent *buf, int len);

#endif /* INPUT_H */
