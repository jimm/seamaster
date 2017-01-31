#ifndef CURSOR_H
#define CURSOR_H

#include "patchmaster.h"
#include "song_list.h"
#include "song.h"
#include "patch.h"

class Cursor {
public:
  patchmaster *pm;
  int song_list_index;
  int song_index;
  int patch_index;
  char *song_list_name;
  char *song_name;
  char *patch_name;

  Cursor(patchmaster *pm);
  ~Cursor();

  void clear();
  void init();

  SongList *song_list();
  Song *song();
  Patch *patch();

  void next_song();
  void prev_song();
  void next_patch();
  void prev_patch();

  void debug();
};

#endif /* H */