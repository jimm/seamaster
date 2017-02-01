#ifndef GUI_H
#define GUI_H

#include "../patchmaster.h"
#include "list_window.h"

class InfoWindow;
class PatchWindow;
class TriggerWindow;
class Window;

class GUI {
public:
  GUI(PatchMaster &);
  ~GUI();

  void run();

private:
  PatchMaster &pm;
  ListWindow *song_lists;
  ListWindow *song_list;
  ListWindow *song;
  PatchWindow *patch;
  Window *message;
  TriggerWindow *trigger;
  InfoWindow *info;

  void event_loop();
  void config_curses();
  void create_windows();
  void resize_windows();
  void free_windows();
  void refresh_all();
  void set_window_data();
  void close_screen();
  void gui_help();
  void show_message(const char *);
};

#endif /* GUI_H */
