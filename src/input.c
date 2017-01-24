#include <stdlib.h>
#include <string.h>
#include "input.h"
#include "trigger.h"

#define MIDI_BUFSIZ 128
// 1/10 second
#define NANOSECS_WAIT 100000000

static struct timespec rqtp = {0, NANOSECS_WAIT};

void *input_thread(void *);

input *input_new(char *sym, char *name, int port_num) {
  input *in = malloc(sizeof(input));

  in->sym = malloc(strlen(sym) + 1);
  strcpy(in->sym, sym);
  in->name = malloc(strlen(name) + 1);
  strcpy(in->name, sym);

  in->port_num = port_num;
  in->running = false;
  in->portmidi_thread = 0;

  int err = Pm_OpenInput(&in->stream, port_num, 0, 128, 0, 0);
  // TODO check error

  in->connections = list_new();
  in->triggers = list_new();

  return in;
}

void input_free(input *in) {
  Pm_Close(in->stream);
  free(in->sym);
  free(in->name);
  list_free(in->connections, 0);
  list_free(in->triggers, 0);
  free(in);
}

void input_add_connection(input *in, connection *conn) {
  list_append(in->connections, conn);
}

void input_remove_connection(input *in, connection *conn) {
  list_remove(in->connections, conn);
}

void input_add_trigger(input *in, trigger *trigger) {
  list_append(in->triggers, trigger);
}

void input_remove_trigger(input *in, trigger *trigger) {
  list_remove(in->triggers, trigger);
}

void input_start(input *in) {
  int status;

  in->running = true;
  status = pthread_create(&in->portmidi_thread, 0, input_thread, in);
  // TODO check status
}

void input_stop(input *in) {
  if (in->running) {
    in->running = false;
    pthread_join(in->portmidi_thread, 0);
  }
}

void input_read(input *in, PmEvent *buf, int len) {
  list *messages = list_new();
  for (int i = 0; i < len; ++i)
    list_append(messages, (void *)buf[i].message);

  for (int i = 0; i < list_length(in->triggers); ++i)
    trigger_signal(list_at(in->triggers, i), messages);

  for (int i = 0; i < list_length(in->connections); ++i)
    connection_midi_in(list_at(in->connections, i), messages);

  list_free(messages, 0);
}

void *input_thread(void *in_voidptr) {
  input *in = (input *)in_voidptr;
  // TOOD if nothing to do, sleep for 1 second (unistd.h, sleep(seconds))
  while (in->running) {
    if (Pm_Poll(in->stream) == TRUE) {
      PmEvent buf[MIDI_BUFSIZ];
      int n = Pm_Read(in->stream, buf, MIDI_BUFSIZ);
      input_read(in, buf, n);
    }
    else {
      nanosleep(&rqtp, 0);
    }
  }
  pthread_exit(0);
}
