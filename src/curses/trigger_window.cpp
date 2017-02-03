#include <stdlib.h>
#include "trigger_window.h"

TriggerWindow::TriggerWindow(struct rect r, const char *title_prefix)
  : Window(r, title_prefix)
{
  title = "Triggers";
  trigger = 0;
}

TriggerWindow::~TriggerWindow() {
}

void TriggerWindow::set_contents(const char *title, Trigger *trigger) {
  title = title;
  trigger = trigger;
}

void TriggerWindow::draw() {
  Window::draw();
  if (trigger == 0)
    return;

  // TODO
}
