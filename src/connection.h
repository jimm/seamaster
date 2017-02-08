#ifndef CONNECTION_H
#define CONNECTION_H

#include <portmidi.h>
#include "list.h"

class Input;
class Output;

typedef struct program {
  int bank_msb;
  int bank_lsb;
  int prog;
} program;

typedef struct zone {
  int low;
  int high;
} zone;

class Connection {
public:
  Input *input;
  Output *output;
  int input_chan;
  int output_chan;
  program prog;
  zone zone;
  int xpose;
  int cc_maps[128];           // -1 == filter out, else dest. controller number

  Connection(Input *input, int input_chan, Output *output, int output_chan);
  ~Connection();

  void start(List<PmMessage> &);
  void stop(List<PmMessage> &);

  void midi_in(PmMessage msg);

  void debug();

private:
  int accept_from_input(PmMessage msg);
  int inside_zone(PmMessage msg);
  void midi_out(PmMessage msg);
};

#endif /* H */
