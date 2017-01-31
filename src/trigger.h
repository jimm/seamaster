#ifndef TRIGGER_H
#define TRIGGER_H

#include <portmidi.h>
#include "named.h"
#include "list.h"

class Trigger : public Named {
public:
  Trigger(const char *name);
  ~Trigger();

  void signal(PmEvent *buf, int len);
};

#endif /* TRIGGER_H */