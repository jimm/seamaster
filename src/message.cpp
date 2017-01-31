#include <stdlib.h>
#include <string.h>
#include "message.h"

Message::Message(const char *name)
  : Named(name)
{
  messages = list_new();
}

Message::~Message() {
  list_free(messages, 0);
}
