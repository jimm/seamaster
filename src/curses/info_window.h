#ifndef INFO_WINDOW_H
#define INFO_WINDOW_H

#include "window.h"
#include "../list.h"

class InfoWindow : public Window {
public:
  List<char *> *text_lines;
  List<char *> *help_lines;
  List<char *> *display_list;

  InfoWindow(struct rect, const char *);
  ~InfoWindow();

  void set_contents(List<char *> *text_lines);

  void draw();
};

List<char *> *info_window_text_to_lines(const char *);
void info_window_free_lines(List<char *> *);

#endif /* INFO_WINDOW_H */
