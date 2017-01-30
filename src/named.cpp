#include <stdlib.h>
#include <string.h>
#include "named.h"

Named::Named(const char *str) {
  name = (char *)malloc(strlen(str)+1);
  strcpy(name, str);
}

Named::~Named() {
  free(name);
}
