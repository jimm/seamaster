#include "error.h"
#include "curses/gui.h"

void error_message(const char * const msg) {
  GUI *gui = gui_instance();
  if (gui != 0) {
    gui->show_message(msg);
    gui->clear_message_after(10);
  }
  else
    fprintf(stderr, "%s\n", msg);
}
