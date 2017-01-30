#ifndef LIST_H
#define LIST_H

#include <stdbool.h>

typedef struct list {
  int num_elements;
  int num_allocated;
  void **nodes;
} list;

list *list_new();
void list_free(list *, void (*content_freeing_func)(void *)); // func may be 0
void list_copy(list *dest, list *src);

int list_length(list *);
void *list_at(list *, int i);
void list_at_set(list *, int i, void *);
int list_index_of(list *, void *); // -1 if not found

void *list_first(list *);
void *list_last(list *);

void list_clear(list *, void (*content_freeing_func)(void *)); // func may be 0

list *list_append(list *, void *);
list *list_append_list(list *, list *);
list *list_insert(list *, int, void *); // insert before index
void *list_remove(list *, void *);
void *list_remove_at(list *, int);
bool includes(list *, void *);

void list_debug(list *, const char *);

#endif /* LIST_H */
