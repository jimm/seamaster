#ifndef SONG_LIST_H
#define SONG_LIST_H

#include <vector>
#include "song.h"

using namespace std;

class SongList : public Named {
public:
  vector<Song *> songs;

  SongList(const char *name);
  ~SongList();
};

#endif /* SONGLIST_LIST_H */
