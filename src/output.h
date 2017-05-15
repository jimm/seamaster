#ifndef OUTPUT_H
#define OUTPUT_H

#include <portmidi.h>
#include "instrument.h"
#include "input.h"

class Output : public Instrument {
public:
  Output(const char *sym, const char *name, int port_num);
  ~Output();

  void write(PmEvent *buf, int len);
};

#endif /* OUTPUT_H */
