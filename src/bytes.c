#include <stdlib.h>
#include <string.h>
#include "bytes.h"

bytes *bytes_new() {
  bytes *b = malloc(sizeof(bytes));
  b->num_bytes = 0;
  b->data = 0;
  return b;
}

void bytes_free(bytes *b) {
  if (b->data != 0)
    free(b->data);
  free(b);
}

bytes *bytes_set(bytes *b, int len, byte *data) {
  if (b->data != 0)
    free(b->data);
  b->num_bytes = len;
  b->data = malloc(len);
  memcpy(b->data, data, len);
  return b;
}

bytes *bytes_append(bytes *b, int len, byte *data) {
  if (b->data == 0)
    return bytes_set(b, len, data);

  byte *new_data = malloc(b->num_bytes + len);
  memcpy(new_data, b->data, b->num_bytes);
  memcpy(new_data + b->num_bytes, data, len);
  free(b->data);
  b->data = new_data;

  return b;
}
