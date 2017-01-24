#ifndef TRIGGER_H
#define TRIGGER_H

#include <portmidi.h>

typedef struct trigger {
  // TODO
  int id;
  char *name;
} trigger;

trigger *trigger_new();
void trigger_free(trigger *);

void trigger_signal(trigger *, PmEvent);

#endif /* TRIGGER_H */