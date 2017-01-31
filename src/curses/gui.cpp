#include <stdlib.h>
#include <ncurses.h>
#include "gui.h"
#include "geometry.h"
#include "list_window.h"
#include "patch_window.h"
#include "trigger_window.h"
#include "info_window.h"
#include "help_window.h"
#include "../cursor.h"

#define NAME_FUNC(f) ((char *(*)(void *))f)
#define PTR_FUNC(f) ((void *(*)(void *))f)

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
void resize_windows(windows *);
void free_windows(windows *);
void gui_run(patchmaster *, windows *);
void refresh_all(patchmaster *pm, windows *);
void set_window_data(patchmaster *pm, windows *);
void close_screen();
void gui_help(windows *);
void show_message(windows *, const char *);

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
  int ch, prev_cmd = 0;

  while (!done) {
    refresh_all(pm, ws);
    ch = getch();
    switch (ch) {
    case 'j': case KEY_DOWN: case ' ':
      patchmaster_next_patch(pm);
      break;
    case 'k': case KEY_UP:
      patchmaster_prev_patch(pm);
      break;
    case 'n': case KEY_RIGHT:
      patchmaster_next_song(pm);
      break;
    case 'p': case KEY_LEFT:
      patchmaster_prev_song(pm);
      break;
    case 'g':
      // TODO go to song
      break;
    case 't':
      // TODO go to song list
      break;
    case 'h': case '?':
      gui_help(ws);
      break;
    case '\e':                  /* escape */
      show_message(ws, "Sending panic note off messages...");
      // TODO, panic
      /* # Twice in a row sends individual note-off commands */
      /* @pm.panic(@prev_cmd == 27) */
      show_message(ws, "Panic sent");
      break;
    case 'l':
      // TODO load file
      /* file = PromptWindow.new('Load', 'Load file:').gets */
      /* if file.length > 0 */
      /*   begin */
      /*     load(file) */
      /*     show_message(ws, "Loaded #{file}") */
      /*   rescue => ex */
      /*     show_message(ws, ex.to_s) */
      /*   end */
      /* end */
      break;
    case 'r':
      // TODO reload file
      /* if @pm.loaded_file && @pm.loaded_file.length > 0 */
      /*   load(@pm.loaded_file) */
      /*   show_message(ws, "Reloaded #{@pm.loaded_file}") */
      /* else */
      /*   show_message(ws, "No file loaded"); */
      /* end */
      break;
    case 'q':
      done = TRUE;
      break;
    case KEY_RESIZE:
      resize_windows(ws);
      break;
    }
    prev_cmd = ch;

    // TODO messages and code keys
    /* msg_name = @pm.message_bindings[ch]; */
    /* @pm.send_message(msg_name) if msg_name; */
    /* code_key = @pm.code_bindings[ch]; */
    /* code_key.call if code_key; */
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
  windows *ws = (windows *)malloc(sizeof(windows));

  ws->song_lists = list_window_new(geom_song_lists_rect(), 0,
                                   PTR_FUNC(&cursor_song_list));
  ws->song_list = list_window_new(geom_song_list_rect(), "Song List",
                                  PTR_FUNC(&cursor_song));
  ws->song = list_window_new(geom_song_rect(), "Song",
                             PTR_FUNC(&cursor_patch));
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
}

void set_window_data(patchmaster *pm, windows *ws) {
  list_window_set_contents(ws->song_lists, "Song Lists", pm->song_lists,
                           pm->cursor);

  SongList *sl = cursor_song_list(pm->cursor);
  list_window_set_contents(ws->song_list, sl->name, sl->songs, pm->cursor);

  Song *song = cursor_song(pm->cursor);
  if (song != 0) {
    list_window_set_contents(ws->song, song->name, song->patches, pm->cursor);
    info_window_set_contents(ws->info, song->notes);
    Patch *patch = cursor_patch(pm->cursor);
    patch_window_set_contents(ws->patch, patch);
  }
  else {
    list_window_set_contents(ws->song, 0, 0, 0);
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
  endwin();
}

void gui_help(windows *ws) {
  rect r = geom_help_rect();
  help_window *hw = help_window_new(r, "Help");
  help_window_draw(hw);
  wnoutrefresh(hw->w->win);
  doupdate();
  getch();                      /* wait for key and eat it */
  help_window_free(hw);
}

void show_message(windows *ws, const char *msg) {
  WINDOW *win = ws->message->win;
  wclear(win);
  waddstr(win, msg);
  wrefresh(win);
}
