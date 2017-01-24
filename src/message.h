#ifndef MESSAGE_H
#define MESSAGE_H

#include "types.h"

typedef struct message {
  int id;
  char *name;
  int num_bytes;
  byte *bytes;
} message;

message *message_new(int id, char *name, int num_bytes, byte *bytes);
void message_free(message *);

#endif /* MESSAGE_H */