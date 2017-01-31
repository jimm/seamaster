#ifndef MESSAGE_H
#define MESSAGE_H

#include "named.h"
#include "list.h"

class Message : public Named {
public:
  List messages;

  Message(const char *name);
  ~Message();
};

#endif /* MESSAGE_H */