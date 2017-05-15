#include <stdlib.h>
#include <string.h>
#include "info_window.h"
#include "help_window.h"

InfoWindow::InfoWindow(struct rect r, const char *title_prefix)
  : Window(r, title_prefix)
{
  text_lines = 0;
  help_lines = info_window_text_to_lines(help_window_read_help());
  display_list = help_lines;    /* do not delete this */
}

InfoWindow::~InfoWindow() {
  info_window_free_lines(help_lines);
}

void InfoWindow::set_contents(vector<char *> *text_lines_v) {
  if (text_lines && !text_lines_v->empty()) {
    title = "Song Notes";
    text_lines = text_lines_v;
    display_list = text_lines_v;
  }
  else {
    title = "SeaMaster Help";
    display_list = help_lines;
  }
}

void InfoWindow::draw() {
  char fitted[BUFSIZ];

  Window::draw();
  for (vector<char *>::iterator i = display_list->begin(); i != display_list->end(); ++i) {
    wmove(win, i - display_list->begin() + 1, 1);
    make_fit(*i, 1, fitted);
    waddstr(win, fitted);
  }
}

/*
 * Splits `text` into lines and returns a list containing the lines. When
 * you are done with the list, only the first entry should be freed.
 */
vector<char *> *info_window_text_to_lines(const char *text) {
  vector<char *> *l = new vector<char *>();

  char *line;
  char *s = strdup(text);
  while ((line = strsep(&s, "\n")) != NULL)
    l->push_back(line);

  return l;
}

void info_window_free_lines(vector<char *> *lines) {
  if (lines == 0)
    return;

  if (!lines->empty())
    free((*lines)[0]);
  delete lines;
}
