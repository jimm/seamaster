#include <stdlib.h>
#include <string.h>
#include "trigger.h"

trigger *trigger_new(char *name) {
  trigger *t = malloc(sizeof(trigger));
  t->name = malloc(strlen(name)+1);
  strcpy(t->name, name);
  return t;
}

void trigger_free(trigger *t) {
  free(t->name);
  free(t);
}

void trigger_signal(trigger *t, PmEvent *buf, int len) {
  // TODO
}

