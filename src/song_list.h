#ifndef SONG_LIST_H
#define SONG_LIST_H

#include "list.h"
#include "song.h"

class SongList : public Named {
public:
  List<Song *> songs;

  SongList(const char *name);
  ~SongList();
};

#endif /* SONGLIST_LIST_H */
