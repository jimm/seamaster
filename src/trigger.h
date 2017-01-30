#ifndef TRIGGER_H
#define TRIGGER_H

#include <portmidi.h>
#include "list.h"

typedef struct trigger {
  // TODO
  char *name;
} trigger;

trigger *trigger_new(char *);
void trigger_free(trigger *);

void trigger_signal(trigger *, PmEvent *buf, int len);

#endif /* TRIGGER_H */