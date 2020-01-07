#include <stdlib.h>
#include <string.h>
#include "song.h"

Song::Song(const char *name)
  : Named(name)
{
}

Song::~Song() {
  for (auto& patch : patches)
    delete patch;
  for (auto& note : notes)
    free(note);
}

// Take over ownership of list of malloc'd char*.
void Song::take_notes(vector<char *> &strs) {
  notes = strs;
}
