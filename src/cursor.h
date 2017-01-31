#ifndef CURSOR_H
#define CURSOR_H

#include "patchmaster.h"
#include "song_list.h"
#include "song.h"
#include "patch.h"

class Cursor {
public:
  PatchMaster *pm;
  int song_list_index;
  int song_index;
  int patch_index;
  string song_list_name;
  string song_name;
  string patch_name;

  Cursor(PatchMaster *pm);
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

#endif /* CURSOR_H */
