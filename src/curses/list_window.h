#ifndef LIST_WINDOW_H
#define LIST_WINDOW_H

#include "window.h"
#include "../named.h"
#include "../list.h"

typedef struct list_window {
  Window *w;
  list *list;
  int offset;
  Named *curr_item;
} list_window;

list_window *list_window_new(rect, const char *);
void list_window_free(list_window *);

void list_window_set_contents(list_window *, const char *title, list *list,
                              Named *curr_item);
void list_window_draw(list_window *);

#ifdef DEBUG
void list_window_debug(list_window *);
#endif

#endif /* LIST_WINDOW_H */
