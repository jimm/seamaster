#include <stdlib.h>
#include <string.h>
#include "message.h"

message *message_new(int id, char *name) {
  message *m = (message *)malloc(sizeof(message));
  m->id = id;
  m->name = malloc(strlen(name) + 1);
  strcpy(m->name, name);
  m->messages = list_new();

  return m;
}

void message_free(message *m) {
  free(m->name);
  list_free(m->messages, 0);
  free(m);
}
