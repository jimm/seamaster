#include <stdlib.h>
#include <string.h>
#include "song.h"
#include "debug.h"

song *song_new(char *name) {
  song *s = malloc(sizeof(song));
  s->name = malloc(strlen(name)+1);
  strcpy(s->name, name);
  s->patches = list_new();
  s->notes = list_new();
  return s;
}

void song_free(song *s) {
  if (s->name)
    free(s->name);
  list_free(s->notes, free);
  list_free(s->patches, patch_free);
  free(s);
}

void song_append_notes(song *s, char *notes) {
  char *str = malloc(strlen(notes) + 1);
  strcpy(str, notes);
  list_append(s->notes, str);
}

char *song_name(song *s) {
  return s->name;
}

void song_debug(song *s) {
  if (s == 0) {
    debug("song NULL\n");
    return;
  }

  debug("song %p, name \"%s\"\n", s, s->name);
  if (s->notes != 0) {
    for (int i = 0; i < list_length(s->notes); ++i)
      debug("  notes: %s\n", list_at(s->notes, i));
  }
  for (int i = 0; i < list_length(s->patches); ++i)
    patch_debug(list_at(s->patches, i));
}
