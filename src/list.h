#ifndef LIST_H
#define LIST_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "debug.h"

#define LIST_INITIAL_SIZE 8

template <class T>
class List {
public:
  List() : nodes(0), num_allocated(0), num_elements(0) {}
  ~List() { free(nodes); }

  void copy(const List &src) {
    if (num_allocated < src.num_elements)
      grow(src.num_elements);
    memcpy(nodes, src.nodes, src.num_elements * sizeof(T));
    num_elements = src.num_elements;
  }

  int length() { return num_elements; }

  T at(int i) {
    if (i < 0)
      i = num_elements + i;
    return (i >= 0 && i < num_elements) ? nodes[i] : 0;
  }

  void at_set(int i, T val) {
    grow(i);
    nodes[i] = val;
  }

  // -1 if not found
  int index_of(T node) {
    for (int i = 0; i < num_elements; ++i)
      if (node == nodes[i])
        return i;
    return -1;
  }

  T first() { return nodes[0]; }
  T last() { return num_elements > 0 ? nodes[num_elements-1] : 0; }

  /*
   * Returns a new list containing elements from start for length len. Start
   * may be negative. If start is out of bounds, returns 0.
   *
   * As a special case, if len is negative the rest of the array is returned.
   */
  List *slice(int start, int length) {
    if (start < 0)
      start = num_elements - start;
    if (start < 0 || start >= num_elements)
      return 0;

    int end = length < 0 ? num_elements : start + length;
    if (end > num_elements)
      end = num_elements;

    List *l = new List();
    for (int i = start; i < end; ++i)
      l->append(nodes[i]);
    return l;
  }

  // func may be 0
  void clear(void (*content_freeing_func)(T)) {
    if (content_freeing_func != 0)
      for (int i = 0; i < num_elements; ++i)
        content_freeing_func(nodes[i]);
    num_elements = 0;
  }

  void append(T node) {
    if (num_allocated == num_elements)
      grow(num_elements + 1);
    nodes[num_elements++] = node;
  }

  void append_list(const List &other) {
    if (other.num_elements == 0)
      return;

    if (num_allocated < num_elements + other.num_elements)
      grow(num_elements + other.num_elements);
    memcpy(nodes, other.nodes, other.num_elements * sizeof(T));
  }

  // insert before index
  void insert(int index, T node) {
    if (num_allocated == num_elements)
      grow(num_elements + 1);
    memcpy(&nodes[index+1], &nodes[index],
           (num_elements - index) * sizeof(T));
    ++num_elements;
    nodes[index] = node;
  }

  void remove_at(int index) {
    if (index < 0 || index >= num_elements)
      return;

    T node = nodes[index];
    memcpy(&nodes[index], &nodes[index+1], (num_elements - index - 1) * sizeof(T));
    --num_elements;
  }

  void remove(T node) {
    remove_at(index_of(node));
  }

  bool includes(T node) {
    for (int i = 0; i < num_elements; ++i)
      if (nodes[i] == node)
        return true;
    return false;
  }


  void apply(void (*f)(T)) {
    for (int i = 0; i < num_elements; ++i)
      f(nodes[i]);
  }

  List *map(T (*f)(T)) {
    List *list = new List();
    for (int i = 0; i < num_elements; ++i)
      list->append(f(nodes[i]));
    return list;
  }

  char *join(const char *sep) {
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

  void debug(const char *msg) {
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


  List &operator=(const List &src) { copy(src); return *this; }
  T operator[](int i) { return at(i); }
  void operator<<(T ptr) { append(ptr); }

private:
  int num_elements;
  int num_allocated;
  T *nodes;

  void grow(int min_size) {
    if (num_allocated >= min_size)
      return;

    if (nodes == 0)
      num_allocated = LIST_INITIAL_SIZE;
    while (num_allocated < min_size)
      num_allocated *= 2;

    if (nodes == 0)
      nodes = (T *)malloc(num_allocated * sizeof(T));
    else
      nodes = (T *)realloc(nodes, num_allocated * sizeof(T));
  }
};

#endif /* H */
