#ifndef MESSAGE_H
#define MESSAGE_H

#include <portmidi.h>
#include "named.h"
#include "list.h"

class Message : public Named {
public:
  List<PmMessage> messages;

  Message(const char *name);
  ~Message();
};

#endif /* MESSAGE_H */