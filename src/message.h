#ifndef MESSAGE_H
#define MESSAGE_H

#include <vector>
#include <portmidi.h>
#include "db_obj.h"
#include "named.h"

using namespace std;

class Output;

class Message : public DBObj, public Named {
public:
  vector<PmMessage> messages;

  Message(int id, const char *name);
  ~Message();

  void send(Output &);

private:
  PmEvent *events;
  int num_events;

  void convert_messages();
};

#endif /* MESSAGE_H */
