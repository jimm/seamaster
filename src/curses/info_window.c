#include <stdlib.h>
#include <string.h>
#include "info_window.h"
#include "help_window.h"

void info_window_free_text_lines(info_window *);

info_window *info_window_new(rect r, char *title_prefix) {
  info_window *iw = malloc(sizeof(info_window));
  iw->w = window_new(r, title_prefix);
  iw->text_lines = list_new();
  iw->help_lines = info_window_text_to_lines(help_window_read_help());
  iw->display_list = iw->help_lines; /* do not delete this */
  return iw;
}

void info_window_free(info_window *iw) {
  info_window_free_text_lines(iw);
  list_free(iw->text_lines, 0);
  if (list_length(iw->help_lines) > 0)
    free(list_at(iw->help_lines, 0));
  list_free(iw->help_lines, 0);
  free(iw);
}

void info_window_set_contents(info_window *iw, char *text) {
  info_window_free_text_lines(iw);
  if (text) {
    iw->w->title = "Song Notes";
    iw->display_list = iw->text_lines;
  }
  else {
    iw->w->title = "SeaMaster Help";
    iw->display_list = iw->help_lines;
  }
  info_window_draw(iw);
}

void info_window_draw(info_window *iw) {
  window_draw(iw->w);
  for (int i = 0; i < list_length(iw->display_list); ++i) {
    wmove(iw->w->win, i+1, 1);
    waddstr(iw->w->win, list_at(iw->display_list, i));
  }
}

/*
 * Splits `text` into lines and returns a list containing the lines. When
 * you are done with the list, only the first entry should be freed.
 */
list *info_window_text_to_lines(char *text) {
  list *l = list_new();

  char *line;
  char *s = strdup(text);
  while ((line = strsep(&s, "\n")) != NULL)
    list_append(l, line);

  return l;
}

void info_window_free_text_lines(info_window *iw) {
  if (list_length(iw->text_lines) > 0)
    free(list_at(iw->text_lines, 0));
  list_clear(iw->text_lines, 0);
}
