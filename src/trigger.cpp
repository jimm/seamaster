#include <stdlib.h>
#include <string.h>
#include "trigger.h"
#include "patchmaster.h"

Trigger::Trigger(sqlite3_int64 id, TriggerAction ta, Message *out_msg)
  : DBObj(id),
    trigger_key_code(UNDEFINED), trigger_message(Pm_Message(0, 0, 0)),
    action(ta), output_message(out_msg)
{
}

Trigger::~Trigger() {
}

void Trigger::set_trigger_key_code(int key_code) {
  trigger_key_code = key_code;
  remove_from_input();
}

void Trigger::set_trigger_message(Input *input, PmMessage message) {
  trigger_key_code = UNDEFINED;
  remove_from_input();
  trigger_message = message;
  input->add_trigger(this);
}

Input *Trigger::input() {
  if (trigger_key_code != UNDEFINED)
    return nullptr;

  for (auto &input : PatchMaster_instance()->inputs)
    for (auto &trigger : input->triggers)
      if (trigger == this)
        return input;
  return nullptr;
}

void Trigger::signal(PmMessage msg) {
  if (msg == trigger_message)
      perform_action();
}

void Trigger::perform_action() {
  PatchMaster *pm = PatchMaster_instance();

  switch (action) {
  case TA_NEXT_SONG:
    pm->next_song();
    break;
  case TA_PREV_SONG:
    pm->prev_song();
    break;
  case TA_NEXT_PATCH:
    pm->next_patch();
    break;
  case TA_PREV_PATCH:
    pm->prev_patch();
    break;
  case TA_PANIC:
    pm->panic(false);
    break;
  case TA_SUPER_PANIC:
    pm->panic(true);
    break;
  case TA_MESSAGE:
    output_message->send();
    break;
  }
}

void Trigger::remove_from_input() {
  Input *i = input();
  if (i != nullptr)
    i->remove_trigger(this);
}
