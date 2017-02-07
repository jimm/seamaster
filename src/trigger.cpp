#include <stdlib.h>
#include <string.h>
#include "trigger.h"
#include "patchmaster.h"

Trigger::Trigger(PmMessage message, TriggerAction ta, Message *out_msg)
  : trigger_message(message), action(ta), output_message(out_msg)
{
}

Trigger::~Trigger() {
}

void Trigger::signal(PmEvent *buf, int len) {
  for (int i = 0; i < len; ++i)
    if (buf[i].message == trigger_message)
      perform_action();
}

void Trigger::perform_action() {
  PatchMaster *pm = PatchMaster_instance();

  switch (action) {
  case NEXT_SONG:
    pm->next_song();
    break;
  case PREV_SONG:
    pm->prev_song();
    break;
  case NEXT_PATCH:
    pm->next_patch();
    break;
  case PREV_PATCH:
    pm->prev_patch();
    break;
  case MESSAGE:
    send_message(pm);
  }
}

void Trigger::send_message(PatchMaster *pm) {
  for (int i = 0; i < pm->outputs.length(); ++i)
    output_message->send(*pm->outputs[i]);
}
