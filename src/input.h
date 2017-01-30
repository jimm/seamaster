#ifndef INPUT_H
#define INPUT_H

#include <pthread.h>
#include <portmidi.h>
#include "consts.h"
#include "list.h"
#include "connection.h"
#include "trigger.h"

#define MIDI_BUFSIZ 128

typedef struct input {
  char *name;
  char *sym;
  int port_num;
  PortMidiStream *stream;
  list *connections;
  list *triggers;
  bool running;
  pthread_t portmidi_thread;
  list *notes_off_conns[MIDI_CHANNELS][NOTES_PER_CHANNEL];

  PmMessage received_messages[MIDI_BUFSIZ]; // used during testing only
  int num_received_messages;
} input;

input *input_new(char *sym, char *name, int port_num);
void input_free(input *);

void input_add_connection(input *, Connection *);
void input_remove_connection(input *, Connection *);

void input_add_trigger(input *, trigger *);
void input_remove_trigger(input *, trigger *);

void input_start(input *);
void input_stop(input *);

void input_read(input *, PmEvent *buf, int len);

void input_debug(input *);
void input_clear(input *);      // testing only

#endif /* INPUT_H */
