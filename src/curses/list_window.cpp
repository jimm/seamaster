#include <stdlib.h>
#include "list_window.h"

ListWindow::ListWindow(struct rect r, const char *title_prefix)
  : Window(r, title_prefix), list(0), offset(0), curr_item(0)
{
}

ListWindow::~ListWindow() {
}

void ListWindow::set_contents(const char *title_str, List<Named *> *l,
                              Named *curr_item_ptr)
{
  title = title_str;
  if (list != l) {
    list = l;
    offset = 0;
  }
  curr_item = curr_item_ptr;
  draw();
}

void ListWindow::draw() {
  Window::draw();
  if (list == 0 || curr_item == 0)
    return;

  int curr_index = list->index_of(curr_item);
  int vis_height = visible_height();
  char fitted[BUFSIZ];

  if (curr_index < offset)
    offset = curr_index;
  else if (curr_index >= offset + vis_height)
    offset = curr_index - vis_height + 1;

  for (int i = offset, row = 1; i < list->length() && i < offset + vis_height; ++i, ++row) {
    Named *thing = list->at(i);
    wmove(win, row, 1);
    if (thing == curr_item)
      wattron(win, A_REVERSE);
    waddch(win, ' ');
    make_fit(thing->name.c_str(), 1, fitted);
    waddstr(win, fitted);
    waddch(win, ' ');
    if (thing == curr_item)
      wattroff(win, A_REVERSE);
  }
}

#ifdef DEBUG

void ListWindow::debug() {
  fprintf(stderr, "list_window %p, offset %d, ", this, offset);
  Window::debug();
  fprintf(stderr, "  list in list window %p:\n", this);
  list->debug("list in list window");
  fprintf(stderr, "  address of curr_item = %p\r\n", curr_item);
}

#endif
