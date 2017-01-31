#ifndef HELP_WINDOW_H
#define HELP_WINDOW_H

#include "window.h"
#include "../list.h"

class HelpWindow : public Window {
public:
  List *lines;

  HelpWindow(struct rect, const char *);
  ~HelpWindow();

  void draw();
};

const char *help_window_read_help(); // used by info_window

#endif /* HELP_WINDOW_H */
