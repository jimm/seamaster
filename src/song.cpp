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
  for (int i = 0; i < notes.length(); ++i)
    free(notes[i]);
}

// Take over ownership of list of malloc'd char*.
void Song::take_notes(List<char *> &strs) {
  notes.copy(strs);
}

void Song::debug() {
  vdebug("song %p, name \"%s\"\n", this, name.c_str());
  for (int i = 0; i < notes.length(); ++i)
    vdebug("  notes: %s\n", notes[i]);
  for (int i = 0; i < patches.length(); ++i)
    patches[i]->debug();
}
