#ifndef PATCH_H
#define PATCH_H

#include <vector>
#include <portmidi.h>
#include "named.h"
#include "connection.h"
#include "input.h"

class Patch : public Named {
public:
  vector<Connection *> connections;
  vector<PmMessage> start_messages;
  vector<PmMessage> stop_messages;
  int num_stop_messages;
  bool running;

  Patch(const char *name);
  ~Patch();

  void start();
  bool is_running();
  void stop();

private:
  void send_messages_to_outputs(vector<PmMessage> &messages);
};

#endif /* PATCH_H */
