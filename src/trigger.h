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
  TA_PANIC,
  TA_SUPER_PANIC,
  TA_MESSAGE
} TriggerAction;

class Input;

class Trigger : public DBObj {
public:
  int trigger_key_code;         // if UNDEFINED use trigger_message
  PmMessage trigger_message;
  TriggerAction action;
  Message *output_message;

  Trigger(sqlite3_int64 id, TriggerAction action, Message *output);
  ~Trigger();

  void set_trigger_key_code(int key_code);
  void set_trigger_message(Input *input, PmMessage message);

  Input *input();               // will return nullptr if key is defined

  bool signal_message(PmMessage msg);
  bool signal_key(int key_code);

private:
  void perform_action();
  void remove_from_input();
};

#endif /* TRIGGER_H */