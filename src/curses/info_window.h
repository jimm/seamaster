#ifndef INFO_WINDOW_H
#define INFO_WINDOW_H

#include "window.h"

typedef struct info_window {
  window *w;
  char *text;
  char *info_text;
} info_window;

info_window *info_window_new(rect, char *);
void info_window_free(info_window *);

void info_window_set_contents(info_window *, char *text);

void info_window_draw(info_window *);

#endif /* INFO_WINDOW_H */
