#include <stdlib.h>
#include <set>
#include "patch.h"
#include "output.h"

Patch::Patch(const char *patch_name)
  : Named(patch_name), running(false)
{
}

Patch::~Patch() {
  for (auto& conn : connections)
    delete conn;
}

void Patch::start() {
  if (running)
    return;

  send_messages_to_outputs(start_messages);
  for (auto& conn : connections)
    conn->start();
  running = true;
}

bool Patch::is_running() {
  return running;
}

void Patch::stop() {
  if (!running)
    return;

  for (auto& conn : connections)
    conn->stop();
  send_messages_to_outputs(stop_messages);
  running = false;
}

void Patch::send_messages_to_outputs(vector<PmMessage> &messages) {
  PmEvent event = {0, 0};
  set<Output *> outputs;
  for (auto& conn : connections)
    outputs.insert(conn->output);

  for (auto& out : outputs) {
    for (auto& msg : messages) {
      event.message = msg;
      out->write(&event, 1);
    }
  }
}
