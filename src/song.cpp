#include <stdlib.h>
#include <string.h>
#include "song.h"

Song::Song(int id, const char *name)
  : DBObj(id), Named(name)
{
}

Song::~Song() {
  for (auto& patch : patches)
    delete patch;
}
