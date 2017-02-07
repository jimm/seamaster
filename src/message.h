#ifndef MESSAGE_H
#define MESSAGE_H

#include <portmidi.h>
#include "named.h"
#include "list.h"

class Output;

class Message : public Named {
public:
  PmEvent *events;
  int num_events;

  Message(const char *name);
  ~Message();

  void send(Output &);
};

#endif /* MESSAGE_H */
