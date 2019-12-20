#include <stdlib.h>
#include <string.h>
#include <portmidi.h>
#include "output.h"
#include "trigger.h"

Output::Output(const char *sym, const char *name, int port_num)
  : Instrument(sym, name, port_num)
{
  if (real_port()) {
    PmError err = Pm_OpenOutput(&stream, port_num, 0, 128, 0, 0, 0);
    if (err != 0) {
      fprintf(stderr, "error opening output stream %s: %s\n", name,
              Pm_GetErrorText(err));
      exit(1);
    }
  }
}

Output::~Output() {
}

void Output::write(PmEvent *buf, int len) {
  if (real_port()) {
    PmError err = Pm_Write(stream, buf, len);
    if (err != 0)
      fprintf(stderr, "error writing MIDI to %s: %s\n", name.c_str(),
              Pm_GetErrorText(err));
  }
  else {
    for (int i = 0; i < len && num_io_messages < MIDI_BUFSIZ-1; ++i)
      io_messages[num_io_messages++] = buf[i].message;
  }
}
