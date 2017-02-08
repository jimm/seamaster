#include <stdlib.h>
#include "patch.h"
#include "debug.h"

Patch::Patch(const char *patch_name)
  : Named(patch_name)
{
  running = false;
}

Patch::~Patch() {
  for (int i = 0; i < connections.length(); ++i)
    delete connections[i];
}

void Patch::start() {
  vdebug("Patch::start %s\n", name.c_str());
  if (running)
    return;

  for (int i = 0; i < connections.length(); ++i)
    connections[i]->start(start_messages);
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
    connections[i]->stop(stop_messages);
  running = false;
}

void Patch::debug() {
  vdebug("patch %s\n", name.c_str());
  for (int i = 0; i < connections.length(); ++i)
    connections[i]->debug();
}
