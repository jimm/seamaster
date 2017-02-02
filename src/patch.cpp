#include <stdlib.h>
#include "patch.h"
#include "debug.h"

Patch::Patch(const char *patch_name)
  : Named(patch_name)
{
  start_messages = stop_messages = 0;
  num_start_messages = num_stop_messages = 0;
  running = false;
}

Patch::~Patch() {
  if (start_messages != 0)
    free(start_messages);
  if (stop_messages != 0)
    free(stop_messages);
  for (int i = 0; i < connections.length(); ++i)
    delete connections[i];
}

List<Input *> *Patch::inputs() {
  List<Input *> *inputs = new List<Input *>();
  for (int i = 0; i < connections.length(); ++i)
    inputs->append(connections[i]->input);
  return inputs;
}

void Patch::start() {
  vdebug("Patch::start %s\n", name.c_str());
  if (running)
    return;

  for (int i = 0; i < connections.length(); ++i)
    connections[i]->start(start_messages, num_start_messages);
  running = true;
}

bool Patch::is_running() {
  return running;
}

void Patch::stop() {
  vdebug("Patch::stop %s\n", name.c_str());
  if (!running)
    return;

  for (int i = 0; i < connections.length(); ++i)
    connections[i]->stop(stop_messages, num_stop_messages);
  running = false;
}

void Patch::debug() {
  vdebug("patch %s\n", name.c_str());
  for (int i = 0; i < connections.length(); ++i)
    connections[i]->debug();
}
