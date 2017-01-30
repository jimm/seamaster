#include <stdlib.h>
#include <string.h>
#include <portmidi.h>
#include "output.h"
#include "trigger.h"
#include "debug.h"

bool output_real_port(output *);

output *output_new(char *sym, char *name, int port_num) {
  output *out = malloc(sizeof(output));

  out->sym = malloc(strlen(sym) + 1);
  strcpy(out->sym, sym);
  out->name = malloc(strlen(name) + 1);
  strcpy(out->name, name);

  out->port_num = port_num;
  if (output_real_port(out)) {
    int err = Pm_OpenOutput(&out->stream, port_num, 0, 128, 0, 0, 0);
    // TODO check error
  }

  out->num_sent_messages = 0;

  return out;
}

void output_free(output *out) {
  if (output_real_port(out))
    Pm_Close(out->stream);
  free(out->sym);
  free(out->name);
  free(out);
}

bool output_real_port(output *out) {
  return out->port_num != pmNoDevice;
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wint-to-void-pointer-cast"

void output_write(output *out, PmEvent *buf, int len) {
  if (output_real_port(out))
    Pm_Write(out->stream, buf, len);
  else {
    for (int i = 0; i < len && out->num_sent_messages < MIDI_BUFSIZ-1; ++i) {
      list_append(out->sent_messages, buf[i].message);
      ++out->num_sent_messages;
    }
  }
}

#pragma clang diagnostic pop

void output_debug(output *out) {
  if (out == 0) {
    debug("output NULL\n");
    return;
  }

  debug("output %s %s (%p)\n", out->sym, out->name, out);
  debug("  port_num %d stream %p\n", out->port_num, out->stream);
}

// only used during testing
void output_clear(output *out) {
  out->num_sent_messages = 0;
}
