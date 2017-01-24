#ifndef LIST_H
#define LIST_H

#include <stdbool.h>

typedef struct list {
  int num_elements;
  int num_allocated;
  void **nodes;
} list;

list *list_new();
void list_free(list *, void (*)()); // func may be 0

int list_length(list *);
void *list_at(list *, int i);
int list_index_of(list *, void *); // -1 if not found

list *list_append(list *, void *);
list *list_insert(list *, int, void *); // insert before index
void *list_remove(list *, void *);
void *list_remove_at(list *, int);
bool includes(list *, void *);

#ifdef DEBUG
void list_debug(list *, char *);
#endif

#endif /* LIST_H */
