#include <stdlib.h>
#include <string.h>
#include "trigger.h"
#include "patchmaster.h"

Trigger::Trigger(sqlite3_int64 id, PmMessage message, TriggerAction ta,
                 Message *out_msg)
  : DBObj(id), trigger_message(message), action(ta), output_message(out_msg)
{
}

Trigger::~Trigger() {
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
