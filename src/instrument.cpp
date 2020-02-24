#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "instrument.h"

Instrument::Instrument(const char *name, const char *pname, int portmidi_port_num)
  : Named(name), port_name(pname), midi_monitor(nullptr), enabled(false)
{
  port_num = portmidi_port_num;
  num_io_messages = 0;
}

Instrument::~Instrument() {
  if (real_port()) {
    PmError err = Pm_Close(stream);
    if (err != 0) {
      char buf[BUFSIZ];
      sprintf(buf, "error closing instrument %s: %s\n", name.c_str(),
              Pm_GetErrorText(err));
      error_message(buf);
    }
  }
}

bool Instrument::real_port() {
  return port_num != pmNoDevice;
}

// only used during testing
void Instrument::clear() {
  num_io_messages = 0;
}
