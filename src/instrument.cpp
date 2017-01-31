#include <stdlib.h>
#include <string.h>
#include "instrument.h"
#include "debug.h"

Instrument::Instrument(const char *sym_str, const char *name, int portmidi_port_num)
  : Named(name)
{
  sym = (char *)malloc(strlen(sym_str) + 1);
  strcpy(sym, sym_str);
  port_num = portmidi_port_num;
  num_io_messages = 0;
}

Instrument::~Instrument() {
  if (real_port())
    Pm_Close(stream);
  free(sym);
}

bool Instrument::real_port() {
  return port_num != pmNoDevice;
}

void Instrument::debug() {
  vdebug("instrument %s %s (%p)\n", sym, name, this);
  vdebug("  port_num %d stream %p\n", port_num, stream);
}

// only used during testing
void Instrument::clear() {
  num_io_messages = 0;
}
