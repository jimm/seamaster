#include <stdlib.h>
#include <string.h>
#include <portmidi.h>
#include "output.h"
#include "trigger.h"

output *output_new(int id, char *sym, char *name, int port_num) {
  output *out = malloc(sizeof(output));
  out->id = id;
  out->sym = malloc(strlen(sym) + 1);
  strcpy(out->sym, sym);
  out->name = malloc(strlen(name) + 1);
  strcpy(out->name, sym);
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

void output_start(output *out) {
  // TODO
}

void output_stop(output *out) {
  // TODO
}

void output_write(output *out, PmEvent *buf, int len) {
  // TODO
}
