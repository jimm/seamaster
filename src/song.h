#ifndef SONG_H
#define SONG_H

#include <vector>
#include "db_obj.h"
#include "patch.h"

using namespace std;

class Song : public DBObj, public Named {
public:
  vector<Patch *> patches;
  string notes;

  Song(int id, const char *name);
  ~Song();
};

#endif /* SONG_H */
