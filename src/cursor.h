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
  char *song_list_name;
  char *song_name;
  char *patch_name;
} cursor;

cursor *cursor_new(patchmaster *pm);
void cursor_free(cursor *);

void cursor_clear(cursor *);
void cursor_init(cursor *);

song_list *cursor_song_list(cursor *);
song *cursor_song(cursor *);
patch *cursor_patch(cursor *);

void cursor_next_song(cursor *);
void cursor_prev_song(cursor *);
void cursor_next_patch(cursor *);
void cursor_prev_patch(cursor *);

void cursor_debug(cursor *);

#endif /* CURSOR_H */