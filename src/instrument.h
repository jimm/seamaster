#ifndef INSTRUMENT_H
#define INSTRUMENT_H

#include <portmidi.h>
#include "named.h"

#define MIDI_BUFSIZ 128

class Instrument : public Named {
public:
  string sym;
  int port_num;
  PortMidiStream *stream;

  PmMessage io_messages[MIDI_BUFSIZ]; // testing only
  int num_io_messages;                // ditto

  Instrument(const char *sym, const char *name, int port_num);
  virtual ~Instrument();

  bool real_port();

  void clear();                 // testing only
};

#endif /* INSTRUMENT_H */
