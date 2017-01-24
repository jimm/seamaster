#ifndef INFO_WINDOW_H
#define INFO_WINDOW_H

#include "window.h"
#include "../list.h"

typedef struct info_window {
  window *w;
  list *text_lines;
  list *help_lines;
  list *display_list;
} info_window;

info_window *info_window_new(rect, char *);
void info_window_free(info_window *);

void info_window_set_contents(info_window *, char *text);

void info_window_draw(info_window *);

list *info_window_text_to_lines(char *);

#endif /* INFO_WINDOW_H */
