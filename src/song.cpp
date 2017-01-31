#include <stdlib.h>
#include <string.h>
#include "song.h"
#include "debug.h"

Song::Song(const char *name)
  : Named(name)
{
  patches = list_new();
  notes = list_new();
}

Song::~Song() {
  list_free(notes, free);
  for (int i = 0; i < list_length(patches); ++i)
    delete (Patch *)list_at(patches, i);
  list_free(patches, 0);
}

void Song::append_notes(char *text) {
  char *str = (char *)malloc(strlen(text) + 1);
  strcpy(str, text);
  list_append(notes, str);
}

void Song::debug() {
  vdebug("song %p, name \"%s\"\n", this, name.c_str());
  if (notes != 0) {
    for (int i = 0; i < list_length(notes); ++i)
      vdebug("  notes: %s\n", list_at(notes, i));
  }
  for (int i = 0; i < list_length(patches); ++i) {
    Patch *p =(Patch *)list_at(patches, i);
    p->debug();
  }
}
