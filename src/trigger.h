#ifndef TRIGGER_H
#define TRIGGER_H

#include <portmidi.h>
#include "db_obj.h"
#include "message.h"

class PatchMaster;

typedef enum TriggerAction {
  TA_NEXT_SONG,
  TA_PREV_SONG,
  TA_NEXT_PATCH,
  TA_PREV_PATCH,
  TA_MESSAGE
} TriggerAction;

class Trigger : public DBObj {
public:
  PmMessage trigger_message;
  TriggerAction action;
  Message *output_message;

  Trigger(sqlite3_int64 id, PmMessage message, TriggerAction action, Message *output);
  ~Trigger();

  void signal(PmMessage msg);

private:
  void perform_action();
  void send_message(PatchMaster *pm);
};

#endif /* TRIGGER_H */