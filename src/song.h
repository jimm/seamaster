#ifndef SONG_H
#define SONG_H

#include "list.h"
#include "patch.h"

typedef struct song {
  char *name;
  list *notes;
  list *patches;
} song;

song *song_new(char *name);
void song_free(song *);

void song_append_notes(song *, char *);

char *song_name(song *song);

#ifdef DEBUG
void song_debug(song *);
#endif

#endif /* SONG_H */
