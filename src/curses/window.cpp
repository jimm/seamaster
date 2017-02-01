#include <stdlib.h>
#include <string.h>
#include "window.h"

Window::Window(struct rect r, const char *title_prefix_str)
{
  if (title_prefix_str != 0)
    title_prefix = title_prefix_str;
  title = "";
  win = newwin(r.height, r.width, r.row, r.col);
  rect = r;
  set_max_contents_len(r.width);
}

Window::~Window() {
  delwin(win);
}

void Window::move_and_resize(struct rect r) {
  rect = r;
  wmove(win, r.row, r.col);
  wresize(win, r.height, r.width);
  Window::set_max_contents_len(r.width);
}

void Window::draw() {
  wclear(win);
  box(win, ACS_VLINE, ACS_HLINE);
  if (title.length() == 0 && title.length() == 0)
    return;

  wmove(win, 0, 1);
  wattron(win, A_REVERSE);
  waddch(win, ' ');
  if (title_prefix.length() > 0) {
    waddstr(win, title_prefix.c_str());
    waddstr(win, ": ");
  }
  if (title.length() > 0)
    waddstr(win, title.c_str());
  waddch(win, ' ');
  wattroff(win, A_REVERSE);
}

int Window::visible_height() {
  return getmaxy(win) - 2;
}

/*
 * Copies str into outbuf to the maximum window width minus `reduce_max_len_by`.
 */
void Window::make_fit(const char *str, int reduce_max_len_by, char *outbuf) {
  int len = strlen(str);
  int w_maxlen = max_contents_len - reduce_max_len_by;
  int newlen = len < w_maxlen ? len : w_maxlen;
  strncpy(outbuf, str, newlen);
  outbuf[newlen] = 0;
}

void Window::set_max_contents_len(int width) {
  max_contents_len = width - 3; /* 2 for borders */
}

#ifdef DEBUG

void Window::debug() {
  fprintf(stderr, "window %p, win %p, title_prefix %s, title %s\n",
          this, win, title_prefix.c_str(), title.c_str());
}

#endif
