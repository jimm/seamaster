#include <stdlib.h>
#include <string.h>
#include "set_list.h"

SetList::SetList(int id, const char *name)
  : DBObj(id), Named(name)
{
}

SetList::~SetList() {
}
