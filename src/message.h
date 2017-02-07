#ifndef MESSAGE_H
#define MESSAGE_H

#include <portmidi.h>
#include "named.h"
#include "list.h"

class Output;

class Message : public Named {
public:
  List<PmMessage> messages;


  Message(const char *name);
  ~Message();

  void send(Output &);

private:
  PmEvent *events;
  int num_events;

  void convert_messages();
};

#endif /* MESSAGE_H */
