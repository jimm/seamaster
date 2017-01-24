// Defines positions and sizes of windows.
#include <ncurses.h>
#include "geometry.h"

int TOP_HEIGHT() { return (LINES - 1) * 2 / 3; }
int BOT_HEIGHT() { return (LINES - 1) - TOP_HEIGHT(); }
int TOP_WIDTH() { return COLS / 3; }
int SLS_HEIGHT() { return ((LINES - 1) * 2 / 3) / 3; }
int SL_HEIGHT() { return ((LINES - 1) * 2 / 3) - SLS_HEIGHT(); }

rect geom_song_list_rect() {
  rect r;

  r.row = 0;
  r.col = 0;
  r.height = SL_HEIGHT();
  r.width = TOP_WIDTH();
  return r;
}

rect geom_song_rect() {
  rect r;

  r.row = 0;
  r.col = TOP_WIDTH();
  r.height = SL_HEIGHT();
  r.width = TOP_WIDTH();
  return r;
}

rect geom_song_lists_rect() {
  rect r;

  r.row = SL_HEIGHT();
  r.col = 0;
  r.height = SLS_HEIGHT();
  r.width = TOP_WIDTH();
  return r;
}

rect geom_trigger_rect() {
  rect r;

  r.row = SL_HEIGHT();
  r.col = TOP_WIDTH();
  r.height = SLS_HEIGHT();
  r.width = TOP_WIDTH();
  return r;
}

rect geom_patch_rect() {
  rect r;

  r.row = ((LINES - 1) * 2 / 3);
  r.col = 0;
  r.height = BOT_HEIGHT();
  r.width = COLS;
  return r;
}

rect geom_message_rect() {
  rect r;

  r.row = LINES - 1;
  r.col = 0;
  r.height = 1;
  r.width = COLS;
  return r;
}

rect geom_info_rect() {
  rect r;

  r.row = 0;
  r.col = TOP_WIDTH() * 2;
  r.height = ((LINES - 1) * 2 / 3);
  r.width = COLS - (TOP_WIDTH() * 2);
  return r;
}

rect geom_help_rect() {
  rect r;

  r.row = 3;
  r.col = 3;
  r.height = LINES - 6;
  r.width = COLS - 6;
  return r;
}
