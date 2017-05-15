#include <stdlib.h>
#include <string.h>
#include <portmidi.h>
#include "output.h"
#include "trigger.h"

Output::Output(const char *sym, const char *name, int port_num)
  : Instrument(sym, name, port_num)
{
  if (real_port()) {
    int err = Pm_OpenOutput(&stream, port_num, 0, 128, 0, 0, 0);
    // TODO check error
  }
}

Output::~Output() {
}

void Output::write(PmEvent *buf, int len) {
  if (real_port())
    Pm_Write(stream, buf, len);
  else {
    for (int i = 0; i < len && num_io_messages < MIDI_BUFSIZ-1; ++i)
      io_messages[num_io_messages++] = buf[i].message;
  }
}
