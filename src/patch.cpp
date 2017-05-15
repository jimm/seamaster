#include <stdlib.h>
#include "patch.h"

Patch::Patch(const char *patch_name)
  : Named(patch_name)
{
  running = false;
}

Patch::~Patch() {
  for (vector<Connection *>::iterator i = connections.begin(); i != connections.end(); ++i)
    delete *i;
}

void Patch::start() {
  if (running)
    return;

  for (vector<Connection *>::iterator i = connections.begin(); i != connections.end(); ++i)
    (*i)->start(start_messages);
  running = true;
}

bool Patch::is_running() {
  return running;
}

void Patch::stop() {
  if (!running)
    return;

  for (vector<Connection *>::iterator i = connections.begin(); i != connections.end(); ++i)
    (*i)->stop(stop_messages);
  running = false;
}
