#ifndef LIST_WINDOW_H
#define LIST_WINDOW_H

#include "window.h"
#include "../named.h"
#include "../list.h"

class ListWindow : public Window {
public:
  List<Named *> *list;
  int offset;
  Named *curr_item;

  ListWindow(struct rect, const char *);
  ~ListWindow();

  void set_contents(const char *title, List<Named *> *, Named *curr_item);
  void draw();

#ifdef DEBUG
  void debug();
#endif
};

#endif /* LIST_WINDOW_H */
