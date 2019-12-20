#include <stdlib.h>
#include <string.h>
#include "instrument.h"

Instrument::Instrument(const char *sym_str, const char *name, int portmidi_port_num)
  : Named(name), sym(sym_str)
{
  port_num = portmidi_port_num;
  num_io_messages = 0;
}

Instrument::~Instrument() {
  if (real_port()) {
    PmError err = Pm_Close(stream);
    if (err != 0)
      fprintf(stderr, "error closing instrument %s: %s\n", name.c_str(),
              Pm_GetErrorText(err));
  }
}

bool Instrument::real_port() {
  return port_num != pmNoDevice;
}

// only used during testing
void Instrument::clear() {
  num_io_messages = 0;
}
