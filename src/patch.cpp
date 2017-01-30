#include <stdlib.h>
#include <string.h>
#include "patch.h"
#include "debug.h"

patch *patch_new(char *name) {
  patch *p = (patch *)malloc(sizeof(patch));
  p->name = (char *)malloc(strlen(name)+1);
  strcpy(p->name, name);
  p->connections = list_new();
  p->start_messages = p->stop_messages = 0;
  p->num_start_messages = p->num_stop_messages = 0;
  p->running = false;
  return p;
}

void patch_free(patch *p) {
  if (p->name)
    free(p->name);
  if (p->start_messages != 0)
    free(p->start_messages);
  if (p->stop_messages != 0)
    free(p->stop_messages);
  for (int i = 0; i < list_length(p->connections); ++i) {
    Connection *conn = (Connection *)list_at(p->connections, i);
    delete conn;
  }
  list_free(p->connections, 0);
  free(p);
}

list *patch_inputs(patch *p) {
  list *inputs = list_new();
  for (int i = 0; i < list_length(p->connections); ++i) {
    Connection *conn = (Connection *)list_at(p->connections, i);
    list_append(inputs, conn->input);
  }
  return inputs;
}

char *patch_name(patch *p) {
  return p->name;
}

void patch_start(patch *p) {
  vdebug("patch_start %s\n", p ? p->name : "(null)");
  if (p == 0 || p->running)
    return;

  if (p == 0 || p->running)
    return;
  for (int i = 0; i < list_length(p->connections); ++i) {
    Connection *conn = (Connection *)list_at(p->connections, i);
    conn->start(p->start_messages, p->num_start_messages);
  }
  p->running = true;
}

bool patch_is_running(patch *p) {
  return p->running;
}

void patch_stop(patch *p) {
  vdebug("patch_stop %s\n", p ? p->name : "(null)");
  if (p == 0 || !p->running)
    return;

  for (int i = 0; i < list_length(p->connections); ++i) {
    Connection *conn = (Connection *)list_at(p->connections, i);
    conn->stop(p->stop_messages, p->num_stop_messages);
  }
  p->running = false;
}

void patch_debug(patch *p) {
  if (p == 0) {
    vdebug("patch NULL\n");
    return;
  }

  vdebug("patch %s\n", p->name);
  for (int i = 0; i < list_length(p->connections); ++i) {
    Connection *conn = (Connection *)list_at(p->connections, i);
    conn->debug();
  }
}
