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

void InfoWindow::set_contents(List<char *> *text_lines) {
  if (text_lines && text_lines->length() > 0) {
    title = "Song Notes";
    text_lines = text_lines;
    display_list = text_lines;
  }
  else {
    title = "SeaMaster Help";
    display_list = help_lines;
  }
}

void InfoWindow::draw() {
  char fitted[BUFSIZ];

  Window::draw();
  for (int i = 0; i < display_list->length() && i < visible_height(); ++i) {
    wmove(win, i+1, 1);
    make_fit(display_list->at(i), 1, fitted);
    waddstr(win, fitted);
  }
}

/*
 * Splits `text` into lines and returns a list containing the lines. When
 * you are done with the list, only the first entry should be freed.
 */
List<char *> *info_window_text_to_lines(const char *text) {
  List<char *> *l = new List<char *>();

  char *line;
  char *s = strdup(text);
  while ((line = strsep(&s, "\n")) != NULL)
    l->append(line);

  return l;
}

void info_window_free_lines(List<char *> *lines) {
  if (lines == 0)
    return;

  if (lines->length() > 0)
    free(lines->first());
  delete lines;
}
