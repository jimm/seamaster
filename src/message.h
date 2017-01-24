#ifndef MESSAGE_H
#define MESSAGE_H

#include "list.h"

typedef struct message {
  char *name;
  list *messages;
} message;

message *message_new(char *name);
void message_free(message *);

#endif /* MESSAGE_H */