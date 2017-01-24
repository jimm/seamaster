#ifndef LIST_WINDOW_H
#define LIST_WINDOW_H

#include "window.h"
#include "../list.h"

typedef struct list_window {
  window *w;
  list *list;
  int offset;
  char *(*str_func)(void *);
  void *(*curr_item_func)();
} list_window;

list_window *list_window_new(rect, char *, char *(*str_func)(void *),
                             void *(*curr_item_func)());
void list_window_free(list_window *);

void list_window_set_contents(list_window *, char *title, list *list);
void list_window_draw(list_window *);

#ifdef DEBUG
void list_window_debug(list_window *);
#endif

#endif /* LIST_WINDOW_H */
