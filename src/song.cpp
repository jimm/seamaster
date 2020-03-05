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
}
