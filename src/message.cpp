#include <stdlib.h>
#include <string.h>
#include "message.h"

Message::Message(const char *name)
  : Named(name)
{
}

Message::~Message() {
}
