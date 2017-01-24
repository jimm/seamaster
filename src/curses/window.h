#ifndef SM_WINDOW_H
#define SM_WINDOW_H

#include <ncurses.h>
#include "geometry.h"

typedef struct window {
  WINDOW *win;
  const char *title_prefix;
  char *title;
  rect rect;
  int max_contents_len;
} window;

window *window_new(rect r, const char *title_prefix);
void window_free(window *);

void window_move_and_resize(window *, rect);
void window_draw(window *);
int window_visible_height(window *);
char *window_make_fit(window *, const char *, int);

#ifdef DEBUG
void window_debug(window *);
#endif

#endif /* SM_WINDOW_H */
