#include <stdlib.h>
#include <string.h>
#include "song.h"
#include "debug.h"

Song::Song(const char *name)
  : Named(name)
{
}

Song::~Song() {
  for (int i = 0; i < patches.length(); ++i)
    delete patches[i];
}

void Song::append_notes(char *text) {
  char *str = (char *)malloc(strlen(text) + 1);
  strcpy(str, text);
  notes << str;
}

void Song::debug() {
  vdebug("song %p, name \"%s\"\n", this, name.c_str());
  for (int i = 0; i < notes.length(); ++i)
    vdebug("  notes: %s\n", notes[i]);
  for (int i = 0; i < patches.length(); ++i)
    patches[i]->debug();
}
