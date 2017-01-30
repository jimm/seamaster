#include <stdlib.h>
#include <string.h>
#include "song.h"
#include "debug.h"

song *song_new(char *name) {
  song *s = (song *)malloc(sizeof(song));
  s->name = (char *)malloc(strlen(name)+1);
  strcpy(s->name, name);
  s->patches = list_new();
  s->notes = list_new();
  return s;
}

void song_free(song *s) {
  if (s->name)
    free(s->name);
  list_free(s->notes, free);
  for (int i = 0; i < list_length(s->patches); ++i)
    patch_free((patch *)list_at(s->patches, i));
  list_free(s->patches, 0);
  free(s);
}

void song_append_notes(song *s, char *notes) {
  char *str = (char *)malloc(strlen(notes) + 1);
  strcpy(str, notes);
  list_append(s->notes, str);
}

char *song_name(song *s) {
  return s->name;
}

void song_debug(song *s) {
  if (s == 0) {
    vdebug("song NULL\n");
    return;
  }

  vdebug("song %p, name \"%s\"\n", s, s->name);
  if (s->notes != 0) {
    for (int i = 0; i < list_length(s->notes); ++i)
      vdebug("  notes: %s\n", list_at(s->notes, i));
  }
  for (int i = 0; i < list_length(s->patches); ++i)
    patch_debug((patch *)list_at(s->patches, i));
}
