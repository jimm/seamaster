#include <stdlib.h>
#include <string.h>
#include <portmidi.h>
#include "mock_output.h"
#include "../src/trigger.h"
#include "../src/debug.h"

output *output_new(char *sym, char *name, int port_num) {
  output *out = malloc(sizeof(output));

  out->sym = malloc(strlen(sym) + 1);
  strcpy(out->sym, sym);
  out->name = malloc(strlen(name) + 1);
  strcpy(out->name, name);
  out->port_num = port_num;

  out->sent_messages = list_new();
  return out;
}

void output_free(output *out) {
  free(out->sym);
  free(out->name);
  free(out);
}

void output_write(output *out, PmEvent *buf, int len) {
  for (int i = 0; i < len; ++i)
    list_append(out->sent_messages, (void *)buf->message);
}

void output_debug(output *out) {
  debug("output %s %s (%p)\n", out->sym, out->name, out);
  debug("  port_num %d stream %p\n", out->port_num, out->stream);
}

void output_clear(output *out) {
  list_clear(out->sent_messages, 0);
}
