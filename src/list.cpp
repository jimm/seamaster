#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "debug.h"

#define INITIAL_SIZE 8

List::List() {
  nodes = 0;
  num_allocated = num_elements = 0;
}

List::~List() {
  free(nodes);
}

void List::copy(const List &src) {
  if (num_allocated < src.num_elements)
    grow(src.num_elements);
  memcpy(nodes, src.nodes, src.num_elements * sizeof(void *));
  num_elements = src.num_elements;
}

int List::index_of(void *node) {
  for (int i = 0; i < num_elements; ++i)
    if (node == nodes[i])
      return i;
  return -1;
}

/*
 * Returns a new list containing elements from start for length len. Start
 * may be negative. If start is out of bounds, returns 0.
 *
 * As a special case, if len is negative the rest of the array is returned.
 */
List *List::slice(int start, int len) {
  if (start < 0)
    start = num_elements - start;
  if (start < 0 || start >= num_elements)
    return 0;

  int end = len < 0 ? num_elements : start + len;
  if (end > num_elements)
    end = num_elements;

  List *l = new List();
  for (int i = start; i < end; ++i)
    l->append(nodes[i]);
  return l;
}

// func may be 0
void List::clear(void (*content_freeing_func)(void *)) {
  if (content_freeing_func != 0)
    for (int i = 0; i < num_elements; ++i)
      content_freeing_func(nodes[i]);
  num_elements = 0;
}  

void List::append(void *node) {
  if (num_allocated == num_elements)
    grow(num_elements + 1);
  nodes[num_elements++] = node;
}

void List::append_list(const List &other) {
  if (other.num_elements == 0)
    return;

  if (num_allocated < num_elements + other.num_elements)
    grow(num_elements + other.num_elements);
  memcpy(nodes, other.nodes, other.num_elements * sizeof(void *));
}

// insert before index
void List::insert(int index, void *node) {
  if (num_allocated == num_elements)
    grow(num_elements + 1);
  memcpy(&nodes[index+1], &nodes[index],
         (num_elements - index) * sizeof(void *));
  ++num_elements;
  nodes[index] = node;
}

void List::remove_at(int index) {
  if (index < 0 || index >= num_elements)
    return;

  void *node = nodes[index];
  memcpy(&nodes[index], &nodes[index+1], (num_elements - index - 1) * sizeof(void *));
  --num_elements;
}

void List::remove(void *node) {
  return remove_at(index_of(node));
}

bool List::includes(void *node) {
  for (int i = 0; i < num_elements; ++i)
    if (nodes[i] == node)
      return true;
  return false;
}

void List::apply(void (*f)(void *)) {
  for (int i = 0; i < num_elements; ++i)
    f(nodes[i]);
}

List *List::map(void *(*f)(void *)) {
  List *list = new List();
  for (int i = 0; i < num_elements; ++i)
    list->append(f(nodes[i]));
  return list;
}

char *List::join(const char *sep) {
  int sep_len = strlen(sep);
  int len = sep_len * num_elements;

  for (int i = 0; i < num_elements; ++i)
    len += strlen((char *)nodes[i]);

  char *joined = (char *)malloc(len + 1);
  char *p = joined;
  for (int i = 0; i < num_elements; ++i) {
    if (i > 0) {
      strcpy(p, sep);
      p += sep_len;
    }
    char *node_str = (char *)nodes[i];
    int node_len = strlen(node_str);
    strcpy(p, node_str);
    p += node_len;
  }
  return joined;
}

void List::grow(int min_size) {
  if (num_allocated >= min_size)
    return;

  if (nodes == 0)
    num_allocated = INITIAL_SIZE;
  while (num_allocated < min_size)
    num_allocated *= 2;

  if (nodes == 0)
    nodes = (void **)malloc(num_allocated * sizeof(void *));
  else
    nodes = (void **)realloc(nodes, num_allocated * sizeof(void *));
}

void List::debug(const char *msg) {
  vdebug("%s%slist %p, num_alloc %d, num_elems %d, nodes %p\n",
         msg == 0 ? "" : msg, msg == 0 ? "" : ": ",
         this, num_allocated, num_elements,
         nodes);
  if (num_elements < 0) {
    vdebug("ERROR: num_elements is negative\n");
    return;
  }
  for (int i = 0; i < num_elements && i < 10; ++i)
    vdebug("  %3i %p\n", i, nodes[i]);
  if (num_elements > 10)
    vdebug("  ...\n");
}
