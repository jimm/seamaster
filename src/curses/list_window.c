#include <stdlib.h>
#include "list_window.h"

list_window *list_window_new(rect r, char * title_prefix, char *(*str_func)(),
                             void *(*curr_item_func)()) {
  list_window *lw = malloc(sizeof(list_window));
  lw->w = window_new(r, title_prefix);
  lw->list = 0;
  lw->offset = 0;
  lw->str_func = str_func;
  lw->curr_item_func = curr_item_func;
  return lw;
}

void list_window_free(list_window * lw) {
}

void list_window_set_contents(list_window *lw, char *title, list *list) {
  lw->w->title = title;
  lw->list = list;
  list_window_draw(lw);
}

void list_window_draw(list_window *lw) {
  window_draw(lw->w);
  if (lw->list == 0)
    return;

  int vis_height = window_visible_height(lw->w);
  // FIXME
  /* void *curr_item = lw->curr_item_func(); */
  void *curr_item = 0;
  int curr_index = list_index_of(lw->list, curr_item);
  if (curr_index == -1)         /* should never happen */
    curr_index = 0;
  if (curr_index < lw->offset)
    lw->offset = curr_index;
  else if (curr_index >= lw->offset + vis_height)
    lw->offset = curr_index - vis_height + 1;

  fprintf(stderr, "lw->offset = %d\n", lw->offset); /* DEBUG */
  for (int i = lw->offset; i < list_length(lw->list) && i < lw->offset + vis_height; ++i) {
    void *thing = list_at(lw->list, i);
    fprintf(stderr, "list_window_draw i = %d, thing = %p\r\n", i, thing); /* DEBUG */
    wmove(lw->w->win, i+1, 1);

    if (thing == curr_item)
      wattron(lw->w->win, A_REVERSE);

    waddch(lw->w->win, ' ');
    char *str = lw->str_func(thing);
    waddstr(lw->w->win, str ? window_make_fit(lw->w, str, 2) : "(null)"); /* DEBUG (null) */
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
