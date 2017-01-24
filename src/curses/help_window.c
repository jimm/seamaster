#include <stdlib.h>
#include <string.h>
#include "help_window.h"
#include "info_window.h"

char *help_window_read_help();

help_window *help_window_new(rect r, char *title) {
  help_window *hw = malloc(sizeof(help_window));
  hw->w = window_new(r, 0);
  hw->w->title = malloc(strlen(title)+1);
  strcpy(hw->w->title, title);
  hw->lines = info_window_text_to_lines(help_window_read_help());
  return hw;
}

void help_window_free(help_window *hw) {
  free(hw->w->title);
  if (list_length(hw->lines) > 0)
    free(list_at(hw->lines, 0));
  list_free(hw->lines, 0);
  free(hw);
}

void help_window_draw(help_window *hw) {
  window_draw(hw->w);
  for (int i = 0; i < list_length(hw->lines); ++i) {
    wmove(hw->w->win, i+1, 1);
    waddstr(hw->w->win, list_at(hw->lines, i));
  }
}

char *help_window_read_help() {
  return
"j, down, space  - next patch\n" \
"k, up           - prev patch\n" \
"n, right        - next song\n" \
"p, left         - prev song\n" \
"\n" \
"g    - goto song\n" \
"t    - goto song list\n" \
"\n" \
"h, ? - help\n" \
"ESC  - panic\n" \
"\n" \
"l    - load\n" \
"r    - reload\n" \
"\n" \
"q    - quit";
}
