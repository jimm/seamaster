#ifndef INFO_WINDOW_H
#define INFO_WINDOW_H

#include "window.h"
#include "../list.h"

class InfoWindow : public Window {
public:
  List *text_lines;
  List *help_lines;
  List *display_list;

  InfoWindow(struct rect, const char *);
  ~InfoWindow();

  void set_contents(List *text_lines);

  void draw();
};

List *info_window_text_to_lines(const char *);
void info_window_free_lines(List *);

#endif /* INFO_WINDOW_H */
