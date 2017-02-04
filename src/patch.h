#ifndef PATCH_H
#define PATCH_H

#include <portmidi.h>
#include "named.h"
#include "list.h"
#include "connection.h"
#include "input.h"

class Patch : public Named {
public:
  List<Connection *> connections;
  PmMessage *start_messages;
  int num_start_messages;
  PmMessage *stop_messages;
  int num_stop_messages;
  bool running;

  Patch(const char *name);
  ~Patch();

  void start();
  bool is_running();
  void stop();

  void debug();
};

#endif /* PATCH_H */
