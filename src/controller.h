#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <portmidi.h>
#include "db_obj.h"

class Controller : public DBObj {
public:
  int cc_num;
  int translated_cc_num;
  int min;
  int max;
  bool filtered;

  Controller(sqlite3_int64 id, int cc_num);
  ~Controller();

  // Returns true if this controller will modify the original by filtering,
  // translating, or clamping.
  bool will_modify();

  // Returns CONTROLLER_BLOCK if nothing to send.
  PmMessage process(PmMessage msg, int output_channel);

private:
  int clamp(int val);
};

#endif /* CONTROLLER_H */