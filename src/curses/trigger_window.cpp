#include <stdlib.h>
#include "trigger_window.h"

trigger_window *trigger_window_new(rect r, const char *title_prefix) {
  trigger_window *tw = (trigger_window *)malloc(sizeof(trigger_window));
  tw->w = window_new(r, title_prefix);
  tw->w->title = "Triggers";
  tw->trigger = 0;
  return tw;
}

void trigger_window_free(trigger_window * tw) {
}

void trigger_window_set_contents(trigger_window *tw, const char *title, Trigger *trigger) {
  tw->w->title = title;
  tw->trigger = trigger;
  trigger_window_draw(tw);
}

void trigger_window_draw(trigger_window *tw) {
  window_draw(tw->w);
  if (tw->trigger == 0)
    return;

  // TODO
}
