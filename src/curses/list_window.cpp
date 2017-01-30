#include <stdlib.h>
#include "list_window.h"
#include "../song.h"            /* DEBUG */

list_window *list_window_new(rect r, const char *title_prefix,
                             void *(*curr_item_func)(void *))
{
  list_window *lw = (list_window *)malloc(sizeof(list_window));
  lw->w = window_new(r, title_prefix);
  lw->list = 0;
  lw->offset = 0;
  lw->curr_item_func = curr_item_func;
  return lw;
}

void list_window_free(list_window * lw) {
}

void list_window_set_contents(list_window *lw, const char *title, list *list,
                              void *curr_item_func_arg) {
  lw->w->title = title;
  lw->list = list;
  lw->curr_item_func_arg = curr_item_func_arg;
  list_window_draw(lw);
}

void list_window_draw(list_window *lw) {
  window_draw(lw->w);
  if (lw->list == 0)
    return;

  int vis_height = window_visible_height(lw->w);

  void *curr_item = 0;
  if (lw->curr_item_func != 0)
    curr_item = lw->curr_item_func(lw->curr_item_func_arg);

  int curr_index = list_index_of(lw->list, curr_item);
  if (curr_index == -1)
    curr_index = 0;
  if (curr_index < lw->offset)
    lw->offset = curr_index;
  else if (curr_index >= lw->offset + vis_height)
    lw->offset = curr_index - vis_height + 1;

  for (int i = lw->offset; i < list_length(lw->list) && i < lw->offset + vis_height; ++i) {
    Named *thing = (Named *)list_at(lw->list, i);
    wmove(lw->w->win, i+1, 1);

    if (thing == curr_item)
      wattron(lw->w->win, A_REVERSE);

    waddch(lw->w->win, ' ');
    waddstr(lw->w->win, thing->name);
    waddch(lw->w->win, ' ');

    if (thing == curr_item)
      wattroff(lw->w->win, A_REVERSE);
  }
}

#ifdef DEBUG

void list_window_debug(list_window *lw) {
  fprintf(stderr, "list_window %p, offset %d, ", lw, lw->offset);
  window_debug(lw->w);
  fprintf(stderr, "  list in list window %p:\n", lw);
  list_debug(lw->list, "list in list window");
  fprintf(stderr, "  address of str_func = %p\r\n", lw->str_func); /* DEBUG */
  fprintf(stderr, "  address of curr_item_func = %p\r\n", lw->curr_item_func); /* DEBUG */
}
#endif
