#include <stdlib.h>
#include "patch.h"
#include "debug.h"

Patch::Patch(const char *patch_name)
  : Named(patch_name)
{
  connections = list_new();
  start_messages = stop_messages = 0;
  num_start_messages = num_stop_messages = 0;
  running = false;
}

Patch::~Patch() {
  if (start_messages != 0)
    free(start_messages);
  if (stop_messages != 0)
    free(stop_messages);
  for (int i = 0; i < list_length(connections); ++i) {
    Connection *conn = (Connection *)list_at(connections, i);
    delete conn;
  }
  list_free(connections, 0);
}

list *Patch::inputs() {
  list *inputs = list_new();
  for (int i = 0; i < list_length(connections); ++i) {
    Connection *conn = (Connection *)list_at(connections, i);
    list_append(inputs, conn->input);
  }
  return inputs;
}

void Patch::start() {
  vdebug("Patch::start %s\n", reinterpret_cast<const char *>(&name));
  if (running)
    return;

  for (int i = 0; i < list_length(connections); ++i) {
    Connection *conn = (Connection *)list_at(connections, i);
    conn->start(start_messages, num_start_messages);
  }
  running = true;
}

bool Patch::is_running() {
  return running;
}

void Patch::stop() {
  vdebug("Patch::stop %s\n", reinterpret_cast<const char *>(&name));
  if (!running)
    return;

  for (int i = 0; i < list_length(connections); ++i) {
    Connection *conn = (Connection *)list_at(connections, i);
    conn->stop(stop_messages, num_stop_messages);
  }
  running = false;
}

void Patch::debug() {
  vdebug("patch %s\n", reinterpret_cast<const char *>(&name));
  for (int i = 0; i < list_length(connections); ++i) {
    Connection *conn = (Connection *)list_at(connections, i);
    conn->debug();
  }
}
