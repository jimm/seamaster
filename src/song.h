#ifndef SONG_H
#define SONG_H

#include <vector>
#include "patch.h"

using namespace std;

class Song : public Named {
public:
  vector<Patch *> patches;
  vector<char *> notes;

  Song(const char *name);
  ~Song();

  void take_notes(vector<char *> &);
};

#endif /* SONG_H */
