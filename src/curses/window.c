#include <stdlib.h>
#include <string.h>
#include "window.h"

void window_set_max_contents_len(window *, int);


window *window_new(rect r, const char *title_prefix) {
  window *w = malloc(sizeof(window));
  w->win = newwin(r.height, r.width, r.row, r.col);
  w->title_prefix = title_prefix;
  w->title = 0;
  w->rect = r;
  window_set_max_contents_len(w, r.width);
  return w;
}

void window_free(window *w) {
  delwin(w->win);
  free(w);
}

void window_move_and_resize(window *w, rect r) {
  w->rect = r;
  wmove(w->win, r.row, r.col);
  wresize(w->win, r.height, r.width);
  window_set_max_contents_len(w, r.width);
}

void window_draw(window *w) {
  wclear(w->win);
  box(w->win, ACS_VLINE, ACS_HLINE);
  if (w->title_prefix == 0 && w->title == 0)
    return;

  wmove(w->win, 0, 1);
  wattron(w->win, A_REVERSE);
  waddch(w->win, ' ');
  if (w->title_prefix) {
    waddstr(w->win, w->title_prefix);
    waddstr(w->win, ": ");
  }
  if (w->title)
    waddstr(w->win, w->title);
  waddch(w->win, ' ');
  wattroff(w->win, A_REVERSE);
}

int window_visible_height(window *w) {
  return getmaxy(w->win) - 2;
}

/*
 * Returns a newly allocated string in all cases. You'll probably want to
 * free it after giving it to curses `addstr`.
 */
char *window_make_fit(window *w, const char *str, int reduce_max_len_by) {
  int len = strlen(str);
  int w_maxlen = w->max_contents_len - reduce_max_len_by;
  int newlen = len < w_maxlen ? len : w_maxlen;
  char *newstr = malloc(newlen + 1);
  strncpy(newstr, str, newlen);
  newstr[newlen] = 0;
  return newstr;
}

void window_set_max_contents_len(window *w, int width) {
  w->max_contents_len = width - 3; /* 2 for borders */
}

#ifdef DEBUG

void window_debug(window *w) {
  fprintf(stderr, "window %p, win %p, title_prefix %s, title %s\n", w, w->win,
          w->title_prefix ? w->title_prefix : "(null)",
          w->title ? w->title : "(null)");
}

#endif
