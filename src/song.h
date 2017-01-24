#ifndef SONG_H
#define SONG_H

#include "list.h"
#include "patch.h"

typedef struct song {
  int id;
  char *name;
  char *notes;
  list *patches;
} song;

song *song_new(int id, char *name, char *notes);
void song_free(song *);

char *song_name(song *song);

#ifdef DEBUG
void song_debug(song *);
#endif

#endif /* SONG_H */
