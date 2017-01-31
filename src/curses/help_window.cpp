#include <stdlib.h>
#include <string.h>
#include "help_window.h"
#include "info_window.h"

HelpWindow::HelpWindow(struct rect r, const char *title)
  : Window(r, title)
{
  lines = info_window_text_to_lines(help_window_read_help());
}

HelpWindow::~HelpWindow() {
  info_window_free_lines(lines);
}

void HelpWindow::draw() {
  Window::draw();
  for (int i = 0; i < lines->length(); ++i) {
    wmove(win, i+1, 1);
    waddstr(win, (char *)lines->at(i));
  }
}

const char *help_window_read_help() {
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
