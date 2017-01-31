#include <stdlib.h>
#include <string.h>
#include "trigger.h"

Trigger::Trigger(const char *name)
  : Named(name)
{
}

Trigger::~Trigger() {
}

void Trigger::signal(PmEvent *buf, int len) {
  // TODO
}
