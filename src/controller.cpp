#include "consts.h"
#include "controller.h"

Controller::Controller(sqlite3_int64 id, int num)
  : DBObj(id),
    cc_num(num),
    translated_cc_num(num),
    min(0), max(127),
    filtered(false)
{
}

Controller::~Controller() {
}

bool Controller::will_modify() {
  return filtered || translated_cc_num != cc_num || min != 0 || max != 127;
}

PmMessage Controller::process(PmMessage msg, int output_chan) {
  if (filtered)
    return CONTROLLER_BLOCK;

  int chan = (output_chan != CONNECTION_ALL_CHANNELS)
    ? output_chan
    : (Pm_MessageStatus(msg) & 0x0f);
  int data2 = Pm_MessageData2(msg);

  return Pm_Message(CONTROLLER + chan, translated_cc_num, clamp(data2));
}

int Controller::clamp(int val) {
  if (val < min)
    val = min;
  if (val > max)
    val = max;
  return val;
}
