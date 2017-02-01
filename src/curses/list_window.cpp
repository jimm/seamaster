#include <stdlib.h>
#include "list_window.h"

ListWindow::ListWindow(struct rect r, const char *title_prefix)
  : Window(r, title_prefix)
{
  list = 0;
  offset = 0;
  curr_item = 0;
}

ListWindow::~ListWindow() {
}

void ListWindow::set_contents(const char *title_str, List *l,
                              Named *curr_item_ptr)
{
  title = title_str;
  list = l;
  curr_item = curr_item_ptr;
  draw();
}

void ListWindow::draw() {
  Window::draw();
  if (list == 0)
    return;

  int vis_height = visible_height();

  int curr_index = list->index_of(curr_item);
  if (curr_index == -1)
    curr_index = 0;
  if (curr_index < offset)
    offset = curr_index;
  else if (curr_index >= offset + vis_height)
    offset = curr_index - vis_height + 1;

  for (int i = offset; i < list->length() && i < offset + vis_height; ++i) {
    Named *thing = (Named *)list->at(i);
    wmove(win, i+1, 1);

    if (thing == curr_item)
      wattron(win, A_REVERSE);

    waddch(win, ' ');
    waddstr(win, thing->name.c_str());
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
