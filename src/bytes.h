#ifndef BYTES_H
#define BYTES_H

#include "types.h"

typedef struct bytes {
  int num_bytes;
  byte *data;
} bytes;

bytes *bytes_new();
void bytes_free(bytes *);

bytes *bytes_set(bytes *, int, byte *);
bytes *bytes_append(bytes *, int, byte *);

#endif /* BYTES_H */