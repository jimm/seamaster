#include <stdlib.h>
#include <string.h>
#include <portmidi.h>
#include "output.h"
#include "trigger.h"
#include "debug.h"

output *output_new(char *sym, char *name, int port_num) {
  output *out = malloc(sizeof(output));

  out->sym = malloc(strlen(sym) + 1);
  strcpy(out->sym, sym);
  out->name = malloc(strlen(name) + 1);
  strcpy(out->name, name);

  out->port_num = port_num;
  int err = Pm_OpenOutput(&out->stream, port_num, 0, 128, 0, 0, 0);
  // TODO check error
  return out;
}

void output_free(output *out) {
  Pm_Close(out->stream);
  free(out->sym);
  free(out->name);
  free(out);
}

void output_write(output *out, PmEvent *buf, int len) {
  Pm_Write(out->stream, buf, len);
}

void output_debug(output *out) {
  debug("output %s %s (%p)\n", out->sym, out->name, out);
  debug("  port_num %d stream %p\n", out->port_num, out->stream);
}
