#include <stdlib.h>
#include "info_window.h"

char *info_window_read_help();

info_window *info_window_new(rect r, char *title_prefix) {
  info_window *iw = malloc(sizeof(info_window));
  iw->w = window_new(r, title_prefix);
  iw->text = 0;
  iw->info_text = info_window_read_help();
  return iw;
}

void info_window_free(info_window *iw) {
  // we don't own text
  free(iw);
}

void info_window_set_contents(info_window *iw, char *text) {
  // note: we don't own text
  if (text) {
    iw->w->title = "Song Notes";
    iw->text = text;
  }
  else {
    iw->w->title = "SeaMaster Help";
    iw->text = iw->info_text;
  }
  info_window_draw(iw);
}

void info_window_draw(info_window *iw) {
  window_draw(iw->w);
  if (iw->text == 0)
    return;

  // TODO
  /* i = 1 */
  /* @text.each_line do |line| */
  /*   break if i >= @win.maxy - 2 */
  /*   @win.move(i+1, 1) */
  /*   @win.addstr(make_fit(" #{line.chomp}")) */
  /*   i += 1 */
  /* end */
}

char *info_window_read_help() {
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
"q    - quit\n";
}
