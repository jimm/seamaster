#ifndef HELP_WINDOW_H
#define HELP_WINDOW_H

#include "window.h"
#include "../list.h"

typedef struct help_window {
  window *w;
  list *lines;
} help_window;

help_window *help_window_new(rect, const char *);
void help_window_free(help_window *);

void help_window_draw(help_window *);

const char *help_window_read_help();  // used by info_window

#endif /* HELP_WINDOW_H */
