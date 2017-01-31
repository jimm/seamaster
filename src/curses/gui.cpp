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
  ListWindow *song_lists;
  ListWindow *song_list;
  ListWindow *song;
  PatchWindow *patch;
  Window *message;
  TriggerWindow *trigger;
  InfoWindow *info;
} windows;

void config_curses();
windows *create_windows();
void resize_windows(windows *);
void free_windows(windows *);
void gui_run(PatchMaster *, windows *);
void refresh_all(PatchMaster *pm, windows *);
void set_window_data(PatchMaster *pm, windows *);
void close_screen();
void gui_help(windows *);
void show_message(windows *, const char *);

void gui_main(PatchMaster *pm) {
  config_curses();
  windows *ws = create_windows();
  gui_run(pm, ws);
  clear();
  refresh();
  close_screen();
  free_windows(ws);
}

void gui_run(PatchMaster *pm, windows *ws) {
  bool done = FALSE;
  int ch, prev_cmd = 0;

  while (!done) {
    refresh_all(pm, ws);
    ch = getch();
    switch (ch) {
    case 'j': case KEY_DOWN: case ' ':
      pm->next_patch();
      break;
    case 'k': case KEY_UP:
      pm->prev_patch();
      break;
    case 'n': case KEY_RIGHT:
      pm->next_song();
      break;
    case 'p': case KEY_LEFT:
      pm->prev_song();
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

  ws->song_lists = new ListWindow(geom_song_lists_rect(), 0);
  ws->song_list = new ListWindow(geom_song_list_rect(), "Song List");
  ws->song = new ListWindow(geom_song_rect(), "Song");
  ws->patch = new PatchWindow(geom_patch_rect(), "Patch");
  ws->message = new Window(geom_message_rect(), "");
  ws->trigger = new TriggerWindow(geom_trigger_rect(), "");
  ws->info = new InfoWindow(geom_info_rect(), "");

  scrollok(stdscr, false);
  scrollok(ws->message->win, false);

  return ws;
}

void resize_windows(windows *ws) {
  ws->song_lists->move_and_resize(geom_song_lists_rect());
  ws->song_list->move_and_resize(geom_song_list_rect());
  ws->song->move_and_resize(geom_song_rect());
  ws->patch->move_and_resize(geom_patch_rect());
  ws->message->move_and_resize(geom_message_rect());
  ws->trigger->move_and_resize(geom_trigger_rect());
  ws->info->move_and_resize(geom_info_rect());
}

void free_windows(windows *ws) {
  delete ws->song_lists;
  delete ws->song_list;
  delete ws->song;
  delete ws->patch;
  delete ws->message;
  delete ws->trigger;
  delete ws->info;
}

void refresh_all(PatchMaster *pm, windows *ws) {
  set_window_data(pm, ws);
  ws->song_lists->draw();
  ws->song_list->draw();
  ws->song->draw();
  ws->patch->draw();
  ws->message->draw();
  ws->trigger->draw();
  ws->info->draw();
  wnoutrefresh(stdscr);
  wnoutrefresh(ws->song_lists->win);
  wnoutrefresh(ws->song_list->win);
  wnoutrefresh(ws->song->win);
  wnoutrefresh(ws->patch->win);
  wnoutrefresh(ws->info->win);
  wnoutrefresh(ws->trigger->win);
  doupdate();
}

void set_window_data(PatchMaster *pm, windows *ws) {
  ws->song_lists->set_contents("Song Lists", &pm->song_lists, pm->cursor->song_list());

  SongList *sl = pm->cursor->song_list();
  ws->song_list->set_contents(sl->name.c_str(), &sl->songs, pm->cursor->song());

  Song *song = pm->cursor->song();
  if (song != 0) {
    ws->song->set_contents(song->name.c_str(), &song->patches, pm->cursor->patch());
    ws->info->set_contents(&song->notes);
    Patch *patch = pm->cursor->patch();
    ws->patch->set_contents(patch);
  }
  else {
    ws->song->set_contents(0, 0, 0);
    ws->info->set_contents(0);
    ws->patch->set_contents(0);
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
  HelpWindow hw(r, "Help");
  hw.draw();
  wnoutrefresh(hw.win);
  doupdate();
  getch();                      /* wait for key and eat it */
}

void show_message(windows *ws, const char *msg) {
  WINDOW *win = ws->message->win;
  wclear(win);
  waddstr(win, msg);
  wrefresh(win);
}
