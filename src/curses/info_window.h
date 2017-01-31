#ifndef INFO_WINDOW_H
#define INFO_WINDOW_H

#include "window.h"
#include "../list.h"

class InfoWindow : public Window {
public:
  list *text_lines;
  list *help_lines;
  list *display_list;

  InfoWindow(struct rect, const char *);
  ~InfoWindow();

  void set_contents(list *text_lines);

  void draw();
};

list *info_window_text_to_lines(const char *);
void info_window_free_lines(list *);

#endif /* INFO_WINDOW_H */
