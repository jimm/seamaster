#include <stdlib.h>
#include <string.h>
#include "message.h"

message *message_new(int id, char *name, int num_bytes, byte *bytes) {
  message *m = (message *)malloc(sizeof(message));
  m->id = id;
  m->name = malloc(strlen(name) + 1);
  strcpy(m->name, name);
  m->num_bytes = num_bytes;
  m->bytes = malloc(num_bytes);
  memcpy(m->bytes, bytes, num_bytes);

  return m;
}

void message_free(message *m) {
  free(m->name);
  free(m->bytes);
  free(m);
}
