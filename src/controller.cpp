#include "consts.h"
#include "controller.h"

Controller::Controller(int id, int num)
  : DBObj(id),
    cc_num(num),
    translated_cc_num(CONTROLLER_NO_XLATE),
    min(0), max(127),
    filtered(false)
{
}

Controller::~Controller() {
}

bool Controller::will_modify() {
  return filtered || translated_cc_num != CONTROLLER_NO_XLATE || min != 0 || max != 127;
}

PmMessage Controller::process(PmMessage msg, int output_chan) {
  if (filtered)
    return CONTROLLER_BLOCK;

  int status = CONTROLLER;
  int data1 = Pm_MessageData1(msg);
  int data2 = Pm_MessageData2(msg);

  if (output_chan != CONNECTION_ALL_CHANNELS)
    status += output_chan;
  else
    status += Pm_MessageStatus(msg) & 0x0f;

  if (translated_cc_num != CONTROLLER_NO_XLATE)
    data1 = translated_cc_num;

  return Pm_Message(status, data1, clamp(data2));
}

int Controller::clamp(int val) {
  if (val < min)
    val = min;
  if (val > max)
    val = max;
  return val;
}
