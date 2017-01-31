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

void InfoWindow::set_contents(list *text_lines) {
  if (text_lines && list_length(text_lines) > 0) {
    title = "Song Notes";
    text_lines = text_lines;
    display_list = text_lines;
  }
  else {
    title = "SeaMaster Help";
    display_list = help_lines;
  }
  draw();
}

void InfoWindow::draw() {
  Window::draw();
  for (int i = 0; i < list_length(display_list); ++i) {
    wmove(win, i+1, 1);
    waddstr(win, (char *)list_at(display_list, i));
  }
}

/*
 * Splits `text` into lines and returns a list containing the lines. When
 * you are done with the list, only the first entry should be freed.
 */
list *info_window_text_to_lines(const char *text) {
  list *l = list_new();

  char *line;
  char *s = strdup(text);
  while ((line = strsep(&s, "\n")) != NULL)
    list_append(l, line);

  return l;
}

void info_window_free_lines(list *lines) {
  if (lines == 0)
    return;

  if (lines != 0) {
    if (list_length(lines) > 0)
      free(list_first(lines));
    list_free(lines, 0);
  }
}
