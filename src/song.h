#ifndef SONG_H
#define SONG_H

#include <vector>
#include "patch.h"

using namespace std;

class Song : public Named {
public:
  vector<Patch *> patches;
  string notes;

  Song(const char *name);
  ~Song();
};

#endif /* SONG_H */
