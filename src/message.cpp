#include <stdlib.h>
#include <string.h>
#include "message.h"
#include "output.h"

Message::Message(const char *name)
  : Named(name), events(0), num_events(0)
{
}

Message::~Message() {
  if (events != 0)
    free(events);
}

void Message::send(Output &out) {
  if (num_events > 0)
    out.write(events, num_events);
}
