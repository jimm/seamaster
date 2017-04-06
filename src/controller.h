#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <portmidi.h>

class Controller {
public:
  int cc_num;
  int translated_cc_num;        // -1 means no translation
  int min;
  int max;
  bool filtered;

  Controller();
  ~Controller();

  // Returns true if this controller will modify the original by filtering,
  // translating, or clamping.
  bool will_modify();

  // Returns -1 if nothing to send.
  PmMessage process(PmMessage msg, int output_channel);

private:
  int clamp(int val);
};

#endif /* CONTROLLER_H */