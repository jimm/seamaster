#include <sstream>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include "gui.h"
#include "../consts.h"
#include "../patchmaster.h"
#include "../loader.h"
#include "geometry.h"
#include "help_window.h"
#include "info_window.h"
#include "list_window.h"
#include "patch_window.h"
#include "prompt_window.h"
#include "trigger_window.h"
f#include "midi_monitor_window.h"
#include "../cursor.h"


static GUI *g_instance = 0;

GUI *gui_instance() {
  return g_instance;
}

GUI::GUI(PatchMaster *pmaster, WindowLayout wlayout)
  : pm(pmaster), layout(wlayout), midi_monitor(nullptr), clear_msg_id(0)
{
  g_instance = this;
}

GUI::~GUI() {
  if (g_instance == this)
    g_instance = 0;
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
      pwin = new PromptWindow("Go To Song");
      name_regex = pwin->gets();
      delete pwin;
      if (name_regex.length() > 0)
        pm->goto_song(name_regex);
      break;
    case 't':
      pwin = new PromptWindow("Go To Song List");
      name_regex = pwin->gets();
      delete pwin;
      if (name_regex.length() > 0)
        pm->goto_song_list(name_regex);
      break;
    case 'h': case '?':
      help();
      break;
    case '\e':                  /* escape */
      show_message("Sending panic...");
      pm->panic(prev_cmd == '\e');
      show_message("Panic sent");
      clear_message_after(5);
      break;
    case 'l':
      load();
      break;
    case 'r':
      reload();
      break;
    case 'v':
      toggle_view();
      break;
    case 'm':
      toggle_midi_monitor();
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
    /* msg_name = @pm->message_bindings[ch]; */
    /* @pm->send_message(msg_name) if msg_name; */
    /* code_key = @pm->code_bindings[ch]; */
    /* code_key.call if code_key; */
  }
}

void GUI::config_curses() {
  initscr();
  cbreak();                     /* unbuffered input */
  noecho();                     /* do not show typed keys */
  keypad(stdscr, true);         /* enable arrow keys and window resize as keys */
  nl();                         /* return key => newline, \n => \r\n */
  curs_set(0);                  /* cursor: 0 = invisible, 1 = normal */
}

void GUI::create_windows() {
  song_lists = new ListWindow(geom_song_lists_rect(), 0);
  song_list = new ListWindow(geom_song_list_rect(), "Song List");
  song = new ListWindow(geom_song_rect(), "Song");
  patch = new PatchWindow(geom_patch_rect(), "Patch",
                          max_name_len(reinterpret_cast<vector<Named *> *>(&pm->inputs)),
                          max_name_len(reinterpret_cast<vector<Named *> *>(&pm->outputs)));
  message = new Window(geom_message_rect(), "");
  trigger = new TriggerWindow(geom_trigger_rect(), "");
  info = new InfoWindow(geom_info_rect(), "");

  play_song = new ListWindow(geom_play_song_rect(), "");
  play_notes = new InfoWindow(geom_play_notes_rect(), "");
  play_patch = new PatchWindow(geom_play_patch_rect(), "Patch",
                               max_name_len(reinterpret_cast<vector<Named *> *>(&pm->inputs)),
                               max_name_len(reinterpret_cast<vector<Named *> *>(&pm->outputs)));

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

  play_song->move_and_resize(geom_play_song_rect());
  play_notes->move_and_resize(geom_play_notes_rect());
  play_patch->move_and_resize(geom_patch_rect());

  if (midi_monitor != nullptr)
    midi_monitor->move_and_resize(geom_midi_monitor_rect());
}

void GUI::free_windows() {
  delete song_lists;
  delete song_list;
  delete song;
  delete patch;
  delete message;
  delete trigger;
  delete info;

  delete play_song;
  delete play_notes;
  delete play_patch;

  if (midi_monitor != nullptr)
    delete midi_monitor;
}

void GUI::toggle_view() {
  layout = layout == CURSES_LAYOUT_NORMAL ? CURSES_LAYOUT_PLAY : CURSES_LAYOUT_NORMAL;
}

void GUI::toggle_midi_monitor() {
  if (midi_monitor == nullptr) {
    midi_monitor = new MIDIMonitorWindow(geom_midi_monitor_rect(), pm);
  }
  else {
    delete midi_monitor;
    midi_monitor = nullptr;
  }
}

void GUI::refresh_all() {
  set_window_data();
  switch (layout) {
  case CURSES_LAYOUT_NORMAL:
    song_lists->draw();
    song_list->draw();
    song->draw();
    patch->draw();
    message->draw();
    trigger->draw();
    info->draw();
    break;
  case CURSES_LAYOUT_PLAY:
    play_song->draw();
    play_notes->draw();
    play_patch->draw();
    break;
  }
  if (midi_monitor != nullptr)
    midi_monitor->draw();

  wnoutrefresh(stdscr);

  switch (layout) {
  case CURSES_LAYOUT_NORMAL:
    wnoutrefresh(song_lists->win);
    wnoutrefresh(song_list->win);
    wnoutrefresh(song->win);
    wnoutrefresh(patch->win);
    wnoutrefresh(info->win);
    wnoutrefresh(trigger->win);
    break;
  case CURSES_LAYOUT_PLAY:
    wnoutrefresh(play_song->win);
    wnoutrefresh(play_notes->win);
    wnoutrefresh(play_patch->win);
    break;
  }

  if (midi_monitor != nullptr)
    wnoutrefresh(midi_monitor->win);

  doupdate();
}

void GUI::set_window_data() {
  switch (layout) {
  case CURSES_LAYOUT_NORMAL:
    set_normal_window_data();
    break;
  case CURSES_LAYOUT_PLAY:
    set_play_window_data();
    break;
  }
}

void GUI::set_normal_window_data() {
  SongList *sl = pm->cursor->song_list();
  Song *s = pm->cursor->song();
  Patch *p = pm->cursor->patch();

  song_lists->set_contents("Song Lists",
                           reinterpret_cast<vector<Named *> *>(&pm->song_lists),
                           pm->cursor->song_list());

  song_list->set_contents(sl->name.c_str(),
                          reinterpret_cast<vector<Named *> *>(&sl->songs),
                          pm->cursor->song());

  if (s != nullptr) {
    song->set_contents(s->name.c_str(),
                       reinterpret_cast<vector<Named *> *>(&s->patches),
                       pm->cursor->patch());
    info->set_contents(&s->notes);
    patch->set_contents(p);
  }
  else {
    song->set_contents(0, 0, 0);
    info->set_contents(nullptr);
    patch->set_contents(nullptr);
  }
}

void GUI::set_play_window_data() {
  Song *s = pm->cursor->song();
  Patch *p = pm->cursor->patch();

  if (s != nullptr) {
    play_song->set_contents(s->name.c_str(),
                            reinterpret_cast<vector<Named *> *>(&s->patches),
                            pm->cursor->patch());
    play_notes->set_contents(&s->notes);
    play_patch->set_contents(p);
  }
  else {
    play_song->set_contents(nullptr, nullptr, nullptr);
    play_notes->set_contents(nullptr);
    play_patch->set_contents(nullptr);
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

void GUI::load() {
  PromptWindow *pwin = new PromptWindow("Load File");
  string path = pwin->gets();
  delete pwin;

  load(path);
}

void GUI::load(string path) {
  if (path.length() == 0) {
    show_message("error: no file loaded");
    return;
  }

  bool testing = pm->testing;
  PatchMaster *old_pm = pm;
  Loader loader;
  pm = loader.load(path.c_str(), testing);
  delete old_pm;

  ostringstream ostr;
  if (loader.has_error()) {
    ostr << "error: " << loader.error();
    show_message(ostr.str());
    return;
  }

  pm->start();

  ostr << "loaded file " << path;
  show_message(ostr.str());
}

void GUI::reload() {
  load(pm->loaded_from_file);
}

void GUI::help() {
  rect r = geom_help_rect();
  HelpWindow hw(r, "Help");
  hw.draw();
  wnoutrefresh(hw.win);
  doupdate();
  getch();                      /* wait for key and eat it */
}

void GUI::show_message(string msg) {
  WINDOW *win = message->win;
  wclear(win);
  message->make_fit(msg, 0);
  waddstr(win, msg.c_str());
  wrefresh(win);
  doupdate();
}

void GUI::clear_message() {
  WINDOW *win = message->win;
  wclear(win);
  wrefresh(win);
  doupdate();
}

void *clear_message_thread(void *gui_vptr) {
  GUI *gui = (GUI *)gui_vptr;
  int clear_message_id = gui->clear_message_id();

  sleep(gui->clear_message_seconds());

  // Only clear the window if the id hasn't changed
  if (gui->clear_message_id() == clear_message_id)
    gui->clear_message();
  return nullptr;
}

void GUI::clear_message_after(int secs) {
  clear_msg_secs = secs;
  clear_msg_id++;

  pthread_t pthread;
  pthread_create(&pthread, 0, clear_message_thread, this);
}

int GUI::max_name_len(vector<Named *> *list) {
  int maxlen = 0;
  for (auto& named : *list) {
    int len = named->name.length();
    if (len > maxlen)
      maxlen = len;
  }
  return maxlen;
}
