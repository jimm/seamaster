#ifndef LIST_H
#define LIST_H

#include <stdbool.h>

class List {
public:
  List();
  ~List();

  void copy(const List &src);

  inline int length() { return num_elements; }
  inline void *at(int i) {
    if (i < 0)
      i = num_elements + i;
    return (i >= 0 && i < num_elements) ? nodes[i] : 0;
  }
  inline void at_set(int i, void *val) {
    grow(i);
    nodes[i] = val;
  }
  int index_of(void *); // -1 if not found

  inline void *first() { return nodes[0]; }
  inline void *last() { return num_elements > 0 ? nodes[num_elements-1] : 0; }

  List *slice(int start, int length);

  void clear(void (*content_freeing_func)(void *)); // func may be 0

  void append(void *);
  void append_list(const List &);
  void insert(int, void *); // insert before index
  void remove(void *);
  void remove_at(int);
  bool includes(void *);

  void apply(void (*f)(void *));
  List *map(void *(*f)(void *));

  char *join(const char *);

  void debug(const char *);

  inline List &operator=(const List &src) { copy(src); return *this; }
  inline void *operator[](int i) { return at(i); }
  inline void operator<<(void *ptr) { append(ptr); }

private:
  int num_elements;
  int num_allocated;
  void **nodes;

  void grow(int);
};

#endif /* H */
