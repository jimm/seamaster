#ifndef PATCH_H
#define PATCH_H

#include <portmidi.h>
#include "list.h"
#include "connection.h"
#include "input.h"

typedef struct patch {
  char *name;
  list *connections;
  PmMessage *start_messages;
  int num_start_messages;
  PmMessage *stop_messages;
  int num_stop_messages;
  bool running;
} patch;

patch *patch_new(char *name);
void patch_free(patch *);

list *patch_inputs(patch *);
char *patch_name(patch *);

void patch_start(patch *);
bool patch_is_running(patch *);
void patch_stop(patch *);

void patch_debug(patch *);

#endif /* PATCH_H */
