#ifndef LIST_WINDOW_H
#define LIST_WINDOW_H

#include <vector>
#include "window.h"
#include "../named.h"

using namespace std;

class ListWindow : public Window {
public:
  vector<Named *> *list;
  vector<Named *>::iterator offset;
  Named *curr_item;

  ListWindow(struct rect, const char *);
  ~ListWindow();

  void set_contents(const char *title, vector<Named *> *, Named *curr_item);
  void draw();

#ifdef DEBUG
  void debug();
#endif
};

#endif /* LIST_WINDOW_H */
