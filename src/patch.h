#ifndef PATCH_H
#define PATCH_H

#include "list.h"
#include "bytes.h"
#include "connection.h"
#include "input.h"

typedef struct patch {
  int id;
  char *name;
  list *connections;
  bytes *start_bytes;
  bytes *stop_bytes;
  bool running;
} patch;

patch *patch_new(int id, char *name);
void patch_free(patch *);

void patch_add_connection(patch *, connection *);
list *patch_inputs(patch *);
char *patch_name(patch *);

void patch_start(patch *);
bool patch_is_running(patch *);
void patch_stop(patch *);

#endif /* PATCH_H */
