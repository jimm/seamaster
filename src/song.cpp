#include <stdlib.h>
#include <string.h>
#include "song.h"

Song::Song(const char *name)
  : Named(name)
{
}

Song::~Song() {
  for (vector<Patch *>::iterator i = patches.begin(); i != patches.end(); ++i)
    delete *i;
  for (vector<char *>::iterator i = notes.begin(); i != notes.end(); ++i)
    free(*i);
}

// Take over ownership of list of malloc'd char*.
void Song::take_notes(vector<char *> &strs) {
  notes = strs;
}
