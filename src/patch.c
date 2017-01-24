#include <stdlib.h>
#include <string.h>
#include "patch.h"

patch *patch_new(int id, char *name) {
  patch *p = malloc(sizeof(patch));
  p->id = id;
  p->name = malloc(strlen(name)+1);
  strcpy(p->name, name);
  p->connections = list_new();
  p->start_messages = list_new();
  p->stop_messages = list_new();
  p->running = false;
  return p;
}

void patch_free(patch *p) {
  if (p->name)
    free(p->name);
  list_free(p->start_messages, 0);
  list_free(p->stop_messages, 0);
  list_free(p->connections, connection_free);
  free(p);
}

list *patch_inputs(patch *p) {
  list *inputs = list_new();
  for (int i = 0; i < list_length(p->connections); ++i) {
    connection *conn = list_at(p->connections, i);
    list_append(inputs, conn->input);
  }
  return inputs;
}

char *patch_name(patch *p) {
  return p->name;
}

void patch_start(patch *p) {
  if (p == 0 || p->running)
    return;
  for (int i = 0; i < list_length(p->connections); ++i) {
    connection *conn = list_at(p->connections, i);
    connection_start(conn, p->start_messages);
  }
  p->running = true;
}

bool patch_is_running(patch *p) {
  return p->running;
}

void patch_stop(patch *p) {
  if (p == 0 || !p->running)
    return;
  for (int i = 0; i < list_length(p->connections); ++i) {
    connection *conn = list_at(p->connections, i);
    connection_stop(conn, p->stop_messages);
  }
  p->running = false;
}
