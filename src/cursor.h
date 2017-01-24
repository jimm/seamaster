#ifndef CURSOR_H
#define CURSOR_H

#include "patchmaster.h"
#include "song_list.h"
#include "song.h"
#include "patch.h"

typedef struct cursor {
  patchmaster *pm;
  int song_list_index;
  int song_index;
  int patch_index;
} cursor;

cursor *cursor_new(patchmaster *pm);
void cursor_free(cursor *);

void cursor_init(cursor *);

song_list *cursor_song_list(cursor *);
song *cursor_song(cursor *);
patch *cursor_patch(cursor *);

#ifdef DEBUG
void cursor_debug(cursor *);
#endif

#endif /* CURSOR_H */