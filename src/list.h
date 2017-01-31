#ifndef LIST_H
#define LIST_H

#include <stdbool.h>

class List {
public:
  List();
  ~List();

  void copy(const List &src);

  int length();
  void *at(int i);
  void at_set(int i, void *);
  int index_of(void *); // -1 if not found

  void *first();
  void *last();

  void clear(void (*content_freeing_func)(void *)); // func may be 0

  void append(void *);
  void append_list(const List &);
  void insert(int, void *); // insert before index
  void remove(void *);
  void remove_at(int);
  bool includes(void *);

  void apply(void (*f)(void *));
  List *map(void *(*f)(void *));

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
