#ifndef SET_LIST_H
#define SET_LIST_H

#include <vector>
#include "song.h"

using namespace std;

class SetList : public Named {
public:
  vector<Song *> songs;

  SetList(const char *name);
  ~SetList();
};

#endif /* SET_LIST_H */
