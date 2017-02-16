#ifndef GUI_H
#define GUI_H

#include "../patchmaster.h"
#include "list_window.h"

class InfoWindow;
class PatchWindow;
class TriggerWindow;
class Window;

typedef enum WindowLayout {
  NORMAL, PLAY
} WindowLayout;

class GUI {
public:
  GUI(PatchMaster *);
  ~GUI();

  void run();

  void clear_message();
  int clear_message_seconds() { return clear_msg_secs; }
  int clear_message_id() { return clear_msg_id; }

private:
  PatchMaster *pm;

  WindowLayout layout;
  Window *message;

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
  void free_windows();
  void refresh_all();
  void set_window_data();
  void set_normal_window_data();
  void set_play_window_data();
  void close_screen();
  void help();
  void load();
  void load(string path);
  void reload();
  void show_message(string);
  void clear_message_after(int);
  int max_name_len(List<Named *> *);
};

#endif /* GUI_H */
