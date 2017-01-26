#include <stdlib.h>
#include <string.h>
#include "input.h"
#include "trigger.h"
#include "debug.h"

#define MIDI_BUFSIZ 128
#define NANOSECS_PER_SEC 1000000000

/* #define SLEEP_BETWEEN_POLLS */

#ifdef SLEEP_BETWEEN_POLLS
// 1/1000 second
#define NANOSECS_WAIT (NANOSECS_PER_SEC / 1000)
static struct timespec rqtp = {0, NANOSECS_WAIT};
#endif

void *input_thread(void *);

input *input_new(char *sym, char *name, int port_num) {
  input *in = malloc(sizeof(input));

  in->sym = malloc(strlen(sym) + 1);
  strcpy(in->sym, sym);
  in->name = malloc(strlen(name) + 1);
  strcpy(in->name, name);

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
  debug("input %p adding connection %p\n", in, conn);
  list_append(in->connections, conn);
}

void input_remove_connection(input *in, connection *conn) {
  debug("input %p removing connection %p\n", in, conn);
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

  debug("input_start\n");
  in->running = true;
  status = pthread_create(&in->portmidi_thread, 0, input_thread, in);
  // TODO check status
}

void input_stop(input *in) {
  debug("input_stop\n");
  if (in->running) {
    in->running = false;
    pthread_join(in->portmidi_thread, 0);
  }
}

void input_read(input *in, PmEvent *buf, int len) {
  debug("input_read %d events\n", len);
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
  while (in->running) {
    if (Pm_Poll(in->stream) == TRUE) {
      PmEvent buf[MIDI_BUFSIZ];
      int n = Pm_Read(in->stream, buf, MIDI_BUFSIZ);
      debug("%d events seen, sending to input_read\n", n);
      input_read(in, buf, n);
    }
#ifdef SLEEP_BETWEEN_POLLS
    else {
      nanosleep(&rqtp, 0);
    }
#endif
  }
  debug("input exiting\n");
  pthread_exit(0);
}

void input_debug(input *in) {
  if (in == 0) {
    debug("input NULL\n");
    return;
  }

  debug("input %s %s (%p)\n", in->sym, in->name, in);
  debug("  port_num %d stream %p\n", in->port_num, in->stream);
  debug("  connections:");
  for (int i = 0; i < list_length(in->connections); ++i)
    debug("    %p\n", list_at(in->connections, i));
  debug("  running: %d\n", in->running);
  debug("  thread: %p\n", in->portmidi_thread);
}
