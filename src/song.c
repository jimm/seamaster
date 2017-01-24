#include <stdlib.h>
#include <string.h>
#ifdef DEBUG
#include <stdio.h>
#endif
#include "song.h"

song *song_new(int id, char *name, char *notes) {
  song *s = malloc(sizeof(song));
  s->id = id;
  s->patches = list_new();

  if (name != 0) {
    s->name = malloc(strlen(name)+1);
    strcpy(s->name, name);
  }
  else
    s->name = 0;

  if (notes != 0) {
    s->notes = malloc(strlen(notes)+1);
    strcpy(s->notes, notes);
  }
  else
    s->notes = 0;

  return s;
}

void song_free(song *s) {
  if (s->name)
    free(s->name);
  if (s->notes)
    free(s->notes);
  list_free(s->patches, patch_free);
  free(s);
}

char *song_name(song *s) {
  return s->name;
}

#ifdef DEBUG

void song_debug(song *s) {
  fprintf(stderr, "song %p, name %s, notes %s\n", s, s->name, s->notes ? s->notes : "(null)");
  list_debug(s->patches, "patches for that song");
}

#endif
