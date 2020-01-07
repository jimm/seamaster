#include <stdlib.h>
#include "patch.h"

Patch::Patch(const char *patch_name)
  : Named(patch_name)
{
  running = false;
}

Patch::~Patch() {
  for (auto& conn : connections)
    delete conn;
}

void Patch::start() {
  if (running)
    return;

  for (auto& conn : connections)
    conn->start(start_messages);
  running = true;
}

bool Patch::is_running() {
  return running;
}

void Patch::stop() {
  if (!running)
    return;

  for (auto& conn : connections)
    conn->stop(stop_messages);
  running = false;
}
