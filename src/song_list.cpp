#include <stdlib.h>
#include <string.h>
#include "song_list.h"

SongList::SongList(const char *name)
  : Named(name)
{
  songs = list_new();
}

SongList::~SongList() {
  list_free(songs, 0);
}
