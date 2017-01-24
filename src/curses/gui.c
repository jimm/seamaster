#include <stdlib.h>
#include <ncurses.h>
#include "gui.h"
#include "geometry.h"
#include "list_window.h"
#include "patch_window.h"
#include "trigger_window.h"
#include "info_window.h"
#include "../cursor.h"

#define NAME_FUNC(f) ((char *(*)(void *))f)

typedef struct windows {
  list_window *song_lists;
  list_window *song_list;
  list_window *song;
  patch_window *patch;
  window *message;
  trigger_window *trigger;
  info_window *info;
} windows;

void config_curses();
windows *create_windows();
void resize_windows();
void free_windows(windows *);
void gui_run(patchmaster *, windows *);
void refresh_all(patchmaster *pm, windows *);
void set_window_data(patchmaster *pm, windows *);
void close_screen();

void gui_main(patchmaster *pm) {
  config_curses();
  windows *ws = create_windows();
  gui_run(pm, ws);
  clear();
  refresh();
  close_screen();
  free_windows(ws);
}

void gui_run(patchmaster *pm, windows *ws) {
  bool done = FALSE;

  while (!done) {
    refresh_all(pm, ws);
    switch (getch()) {
    case 'q':
      done = TRUE;
      break;
    case KEY_RESIZE:
      resize_windows(ws);
      break;
    }
  }
}

void config_curses() {
  initscr();
  cbreak();                     /* unbuffered input */
  noecho();                     /* do not show typed keys */
  keypad(stdscr, true);         /* enable arrow keys */
  nl();                         /* return key => newline, \n => \r\n */
  curs_set(0);                  /* cursor: 0 = invisible, 1 = normal */
}

windows *create_windows() {
  windows *ws = malloc(sizeof(windows));

  ws->song_lists = list_window_new(geom_song_lists_rect(), 0,
                                   NAME_FUNC(&song_list_name), 0);
  ws->song_list = list_window_new(geom_song_list_rect(), "Song List",
                                  NAME_FUNC(&song_list_name), 0);
  ws->song = list_window_new(geom_song_rect(), "Song", NAME_FUNC(&patch_name), 0);
  ws->patch = patch_window_new(geom_patch_rect(), "Patch");
  ws->message = window_new(geom_message_rect(), 0);
  ws->trigger = trigger_window_new(geom_trigger_rect(), 0);
  ws->info = info_window_new(geom_info_rect(), 0);

  scrollok(stdscr, false);
  scrollok(ws->message->win, false);

  return ws;
}

void resize_windows(windows *ws) {
  window_move_and_resize(ws->song_lists->w, geom_song_lists_rect());
  window_move_and_resize(ws->song_list->w, geom_song_list_rect());
  window_move_and_resize(ws->song->w, geom_song_rect());
  window_move_and_resize(ws->patch->w, geom_patch_rect());
  window_move_and_resize(ws->message, geom_message_rect());
  window_move_and_resize(ws->trigger->w, geom_trigger_rect());
  window_move_and_resize(ws->info->w, geom_info_rect());
}

void free_windows(windows *ws) {
  list_window_free(ws->song_lists);
  list_window_free(ws->song_list);
  list_window_free(ws->song);
  patch_window_free(ws->patch);
  window_free(ws->message);
  trigger_window_free(ws->trigger);
  info_window_free(ws->info);
}

void refresh_all(patchmaster *pm, windows *ws) {
  set_window_data(pm, ws);
#ifndef DEBUG
  list_window_draw(ws->song_lists);
  list_window_draw(ws->song_list);
  list_window_draw(ws->song);
  patch_window_draw(ws->patch);
  window_draw(ws->message);
  trigger_window_draw(ws->trigger);
  info_window_draw(ws->info);
  wnoutrefresh(stdscr);
  wnoutrefresh(ws->song_lists->w->win);
  wnoutrefresh(ws->song_list->w->win);
  wnoutrefresh(ws->song->w->win);
  wnoutrefresh(ws->patch->w->win);
  wnoutrefresh(ws->info->w->win);
  wnoutrefresh(ws->trigger->w->win);
  doupdate();
#endif
}

void set_window_data(patchmaster *pm, windows *ws) {
  list_window_set_contents(ws->song_lists, "Song Lists", pm->song_lists);

  song_list *sl = cursor_song_list(pm->cursor);
  list_window_set_contents(ws->song_list, sl->name, sl->songs);

  song *song = cursor_song(pm->cursor);
  if (song != 0) {
    list_window_set_contents(ws->song, song->name, song->patches);
    info_window_set_contents(ws->info, song->notes);
    patch *patch = cursor_patch(pm->cursor);
    patch_window_set_contents(ws->patch, patch);
  }
  else {
    list_window_set_contents(ws->song, 0, 0);
    info_window_set_contents(ws->info, 0);
    patch_window_set_contents(ws->patch, 0);
  }
}

void close_screen() {
  curs_set(1);
  echo();
  nl();
  noraw();
  nocbreak();
  refresh();
}
