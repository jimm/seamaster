#include <stdlib.h>
#include <ncurses.h>
#include "gui.h"
#include "../patchmaster.h"
#include "geometry.h"
#include "help_window.h"
#include "info_window.h"
#include "list_window.h"
#include "patch_window.h"
#include "prompt_window.h"
#include "trigger_window.h"
#include "../cursor.h"

GUI::GUI(PatchMaster &pmaster)
  : pm(pmaster)
{
}

GUI::~GUI() {
}

void GUI::run() {
  config_curses();
  create_windows();
  event_loop();
  clear();
  refresh();
  close_screen();
  free_windows();
}

void GUI::event_loop() {
  bool done = FALSE;
  int ch, prev_cmd = 0;
  PromptWindow *pwin;
  string name_regex;

  while (!done) {
    refresh_all();
    ch = getch();
    switch (ch) {
    case 'j': case KEY_DOWN: case ' ':
      pm.next_patch();
      break;
    case 'k': case KEY_UP:
      pm.prev_patch();
      break;
    case 'n': case KEY_RIGHT:
      pm.next_song();
      break;
    case 'p': case KEY_LEFT:
      pm.prev_song();
      break;
    case 'g':
      pwin = new PromptWindow("Go To Song", "Go to song:");
      name_regex = pwin->gets();
      delete pwin;
      if (name_regex.length() > 0)
        pm.goto_song(name_regex);
      break;
    case 't':
      pwin = new PromptWindow("Go To Song List", "Go to song list:");
      name_regex = pwin->gets();
      delete pwin;
      if (name_regex.length() > 0)
        pm.goto_song_list(name_regex);
      break;
    case 'h': case '?':
      help();
      break;
    case '\e':                  /* escape */
      show_message("Sending panic note off messages...");
      // TODO, panic
      /* # Twice in a row sends individual note-off commands */
      /* @pm.panic(@prev_cmd == 27) */
      show_message("Panic sent");
      break;
    case 'l':
      // TODO load file
      /* file = PromptWindow.new('Load', 'Load file:').gets */
      /* if file.length > 0 */
      /*   begin */
      /*     load(file) */
      /*     show_message("Loaded #{file}") */
      /*   rescue => ex */
      /*     show_message(ex.to_s) */
      /*   end */
      /* end */
      break;
    case 'r':
      // TODO reload file
      /* if @pm.loaded_file && @pm.loaded_file.length > 0 */
      /*   load(@pm.loaded_file) */
      /*   show_message("Reloaded #{@pm.loaded_file}") */
      /* else */
      /*   show_message("No file loaded"); */
      /* end */
      break;
    case 'q':
      done = TRUE;
      break;
    case KEY_RESIZE:
      resize_windows();
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

void GUI::config_curses() {
  initscr();
  cbreak();                     /* unbuffered input */
  noecho();                     /* do not show typed keys */
  keypad(stdscr, true);         /* enable arrow keys */
  nl();                         /* return key => newline, \n => \r\n */
  curs_set(0);                  /* cursor: 0 = invisible, 1 = normal */
}

void GUI::create_windows() {
  song_lists = new ListWindow(geom_song_lists_rect(), 0);
  song_list = new ListWindow(geom_song_list_rect(), "Song List");
  song = new ListWindow(geom_song_rect(), "Song");
  patch = new PatchWindow(geom_patch_rect(), "Patch");
  message = new Window(geom_message_rect(), "");
  trigger = new TriggerWindow(geom_trigger_rect(), "");
  info = new InfoWindow(geom_info_rect(), "");

  scrollok(stdscr, false);
  scrollok(message->win, false);
}

void GUI::resize_windows() {
  song_lists->move_and_resize(geom_song_lists_rect());
  song_list->move_and_resize(geom_song_list_rect());
  song->move_and_resize(geom_song_rect());
  patch->move_and_resize(geom_patch_rect());
  message->move_and_resize(geom_message_rect());
  trigger->move_and_resize(geom_trigger_rect());
  info->move_and_resize(geom_info_rect());
}

void GUI::free_windows() {
  delete song_lists;
  delete song_list;
  delete song;
  delete patch;
  delete message;
  delete trigger;
  delete info;
}

void GUI::refresh_all() {
  set_window_data();
  song_lists->draw();
  song_list->draw();
  song->draw();
  patch->draw();
  message->draw();
  trigger->draw();
  info->draw();
  wnoutrefresh(stdscr);
  wnoutrefresh(song_lists->win);
  wnoutrefresh(song_list->win);
  wnoutrefresh(song->win);
  wnoutrefresh(patch->win);
  wnoutrefresh(info->win);
  wnoutrefresh(trigger->win);
  doupdate();
}

void GUI::set_window_data() {
  song_lists->set_contents("Song Lists",
                           reinterpret_cast<List<Named *> *>(&pm.song_lists),
                           pm.cursor->song_list());

  SongList *sl = pm.cursor->song_list();
  song_list->set_contents(sl->name.c_str(),
                          reinterpret_cast<List<Named *> *>(&sl->songs),
                          pm.cursor->song());

  Song *s = pm.cursor->song();
  if (s != 0) {
    song->set_contents(s->name.c_str(),
                       reinterpret_cast<List<Named *> *>(&s->patches),
                       pm.cursor->patch());
    info->set_contents(&s->notes);
    Patch *p = pm.cursor->patch();
    patch->set_contents(p);
  }
  else {
    song->set_contents(0, 0, 0);
    info->set_contents(0);
    patch->set_contents(0);
  }
}

void GUI::close_screen() {
  curs_set(1);
  echo();
  nl();
  noraw();
  nocbreak();
  refresh();
  endwin();
}

void GUI::help() {
  rect r = geom_help_rect();
  HelpWindow hw(r, "Help");
  hw.draw();
  wnoutrefresh(hw.win);
  doupdate();
  getch();                      /* wait for key and eat it */
}

void GUI::show_message(const char *msg) {
  WINDOW *win = message->win;
  wclear(win);
  waddstr(win, msg);
  wrefresh(win);
}
