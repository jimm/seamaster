#ifndef GUI_H
#define GUI_H

#include "../patchmaster.h"
#include "list_window.h"

using namespace std;

class InfoWindow;
class PatchWindow;
class TriggerWindow;
class MIDIMonitorWindow;
class Window;

typedef enum WindowLayout {
  CURSES_LAYOUT_NORMAL, CURSES_LAYOUT_PLAY
} WindowLayout;

class GUI {
public:
  GUI(PatchMaster * pm, WindowLayout layout=CURSES_LAYOUT_NORMAL);
  ~GUI();

  void run();

  void show_message(string);
  void clear_message();
  void clear_message_after(int);
  int clear_message_seconds() { return clear_msg_secs; }
  int clear_message_id() { return clear_msg_id; }

private:
  PatchMaster *pm;

  WindowLayout layout;
  Window *message;
  MIDIMonitorWindow *midi_monitor;

  // normal screen
  ListWindow *song_lists;
  ListWindow *song_list;
  ListWindow *song;
  PatchWindow *patch;
  TriggerWindow *trigger;
  InfoWindow *info;

  // play screen
  ListWindow *play_song;
  InfoWindow *play_notes;
  PatchWindow *play_patch;

  int clear_msg_secs;
  int clear_msg_id;

  void event_loop();
  void config_curses();
  void create_windows();
  void resize_windows();
  void toggle_view();
  void toggle_midi_monitor();
  void free_windows();
  void refresh_all();
  void set_window_data();
  void set_normal_window_data();
  void set_play_window_data();
  void close_screen();
  void help();
  void prog_changes_seen();
  void load();
  void load(string path);
  void reload();
  int max_name_len(vector<Named *> *);
};

GUI *gui_instance();

#endif /* GUI_H */
