#include <stdlib.h>
#include <string.h>
#include "mock_input.h"
#include "../src/trigger.h"
#include "../src/debug.h"

input *input_new(char *sym, char *name, int port_num) {
  input *in = malloc(sizeof(input));

  in->sym = malloc(strlen(sym) + 1);
  strcpy(in->sym, sym);
  in->name = malloc(strlen(name) + 1);
  strcpy(in->name, name);

  in->port_num = port_num;
  in->running = false;

  in->connections = list_new();
  in->triggers = list_new();

  in->received_messages = list_new();
  return in;
}

void input_free(input *in) {
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
}

void input_stop(input *in) {
  in->running = false;
}

void input_read(input *in, PmEvent *buf, int len) {
  debug("input_read %d events\n", len);
  list *messages = list_new();
  for (int i = 0; i < len; ++i) {
    void  *msg = (void *)buf[i].message;
    list_append(messages, msg);
    list_append(in->received_messages, msg);
  }

  for (int i = 0; i < list_length(in->triggers); ++i)
    trigger_signal(list_at(in->triggers, i), messages);

  for (int i = 0; i < list_length(in->connections); ++i)
    connection_midi_in(list_at(in->connections, i), messages);

  list_free(messages, 0);
}

void input_debug(input *in) {
  debug("input %s %s (%p)\n", in->sym, in->name, in);
  debug("  port_num %d stream %p\n", in->port_num, in->stream);
  debug("  connections:");
  for (int i = 0; i < list_length(in->connections); ++i)
    debug("    %p\n", list_at(in->connections, i));
  debug("  running: %d\n", in->running);
}

void input_clear(input *in) {
  list_clear(in->received_messages, 0);
}
