#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "debug.h"

#define INITIAL_SIZE 8

void list_grow(list *);

list *list_new() {
  list *l = malloc(sizeof(list));
  l->nodes = (void **)malloc(INITIAL_SIZE * sizeof(void *));
  l->num_elements = 0;
  l->num_allocated = INITIAL_SIZE;
  return l;
}

void list_free(list *l, void (*content_freeing_func)()) {
  if (content_freeing_func != 0)
    for (int i = 0; i < l->num_elements; ++i)
      content_freeing_func(l->nodes[i]);
  free(l->nodes);
  free(l);
}

int list_length(list *l) {
  return l->num_elements;
}

void *list_at(list *l, int i) {
  return (i >= 0 && i < l->num_elements) ? l->nodes[i] : 0;
}

int list_index_of(list *l, void *node) {
  for (int i = 0; i < l->num_elements; ++i)
    if (node == l->nodes[i])
      return i;
  return -1;
}

void *list_first(list *l) {
  return l->nodes[0];
}

void *list_last(list *l) {
  return l->num_elements > 0 ? l->nodes[l->num_elements-1] : 0;
}

// func may be 0
void list_clear(list *l, void (*content_freeing_func)()) {
  if (content_freeing_func != 0)
    for (int i = 0; i < l->num_elements; ++i)
      content_freeing_func(l->nodes[i]);
  l->num_elements = 0;
}  

list *list_append(list *l, void *node) {
  if (l->num_allocated == l->num_elements)
    list_grow(l);
  l->nodes[l->num_elements++] = node;
  return l;
}

list *list_append_list(list *l, list *other) {
  if (other->num_elements == 0)
    return l;

  while (l->num_allocated < l->num_elements + other->num_elements)
    list_grow(l);
  memcpy(l->nodes, other->nodes, other->num_elements * sizeof(void *));
  return l;
}

// insert before index
list *list_insert(list *l, int index, void *node) {
  if (l->num_allocated == l->num_elements)
    list_grow(l);
  memcpy(&l->nodes[index+1], &l->nodes[index],
         (l->num_elements - index) * sizeof(void *));
  ++l->num_elements;
  l->nodes[index] = node;
  return l;
}

void *list_remove_at(list *l, int index) {
  if (index < 0 || index >= l->num_elements)
    return 0;

  void *node = l->nodes[index];
  memcpy(&l->nodes[index], &l->nodes[index+1], (l->num_elements - index - 1) * sizeof(void *));
  --l->num_elements;
  return node;
}

void *list_remove(list *l, void *node) {
  return list_remove_at(l, list_index_of(l, node));
}

bool list_includes(list *l, void *node) {
  for (int i = 0; i < l->num_elements; ++i)
    if (l->nodes[i] == node)
      return true;
  return false;
}

void list_grow(list *l) {
  l->num_allocated = l->num_allocated * 2;
  l->nodes = realloc(l->nodes, l->num_allocated * sizeof(void *));
}

void list_debug(list *l, char *msg) {
  if (l == 0) {
    debug("list NULL\n");
    return;
  }

  debug("%s%slist %p, num_alloc %d (%p), num_elems %d (%p), nodes %p\n",
        msg == 0 ? "" : msg, msg == 0 ? "" : ": ",
        l,
        l->num_allocated, (void *)l->num_allocated,
        l->num_elements, (void *)l->num_elements,
        l->nodes);
  if (l->num_elements < 0) {
    debug("ERROR: l->num_elements is negative\n");
    return;
  }
  for (int i = 0; i < l->num_elements && i < 10; ++i)
    debug("  %3i: %p\n", i, l->nodes[i]);
  if (l->num_elements > 10)
    debug("  ...\n");
}
