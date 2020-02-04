#ifndef CONNECTION_H
#define CONNECTION_H

#include <vector>
#include <portmidi.h>
#include "controller.h"

using namespace std;

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
  Controller cc_maps[128];

  Connection(Input *input, int input_chan, Output *output, int output_chan);
  ~Connection();

  void start();
  void stop();

  void midi_in(PmMessage msg);

private:
  int input_channel_ok(PmMessage msg);
  int inside_zone(PmMessage msg);
  void midi_out(PmMessage msg);
};

#endif /* CONNECTION_H */
