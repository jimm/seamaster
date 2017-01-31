#ifndef TRIGGER_WINDOW_H
#define TRIGGER_WINDOW_H

#include "window.h"
#include "../trigger.h"

typedef struct trigger_window {
  window *w;
  Trigger *trigger;
} trigger_window;

trigger_window *trigger_window_new(rect, const char *);
void trigger_window_free(trigger_window *);

void trigger_window_set_contents(trigger_window *, const char *title, Trigger *trigger);

void trigger_window_draw(trigger_window *);

#endif /* TRIGGER_WINDOW_H */
