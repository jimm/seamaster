#ifndef TRIGGER_H
#define TRIGGER_H

#include <portmidi.h>
#include "message.h"

class PatchMaster;

typedef enum TriggerAction {
  NEXT_SONG,
  PREV_SONG,
  NEXT_PATCH,
  PREV_PATCH,
  MESSAGE
} TriggerAction;

class Trigger {
public:
  PmMessage trigger_message;
  TriggerAction action;
  Message *output_message;

  Trigger(PmMessage message, TriggerAction action, Message *output);
  ~Trigger();

  void signal(PmEvent *buf, int len);

private:
  void perform_action();
  void send_message(PatchMaster *pm);
};

#endif /* TRIGGER_H */