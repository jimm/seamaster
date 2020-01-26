#include <stdlib.h>
#include "consts.h"
#include "connection.h"
#include "input.h"
#include "output.h"

Connection::Connection(Input *in, int in_chan, Output *out, int out_chan)
  : input(in), input_chan(in_chan), output(out), output_chan(out_chan)
{
  prog.bank_msb = prog.bank_lsb = prog.prog = -1;
  zone.low = zone.high = -1;
  xpose = 0;
  for (int i = 0; i < 128; ++i)
    cc_maps[i].cc_num = i;
}

Connection::~Connection() {
}

void Connection::start() {
  if (output_chan != -1) {
    if (prog.bank_msb >= 0)
      midi_out(Pm_Message(CONTROLLER + output_chan, CC_BANK_SELECT_MSB, prog.bank_msb));
    if (prog.bank_lsb >= 0)
      midi_out(Pm_Message(CONTROLLER + output_chan, CC_BANK_SELECT_LSB, prog.bank_lsb));
    if (prog.prog >= 0)
      midi_out(Pm_Message(PROGRAM_CHANGE + output_chan, prog.prog, 0));
  }

  input->add_connection(this);
}

void Connection::stop() {
  input->remove_connection(this);
}

void Connection::midi_in(PmMessage msg) {
  if (!accept_from_input(msg))
      return;

  PmMessage cc_msg;
  int status = Pm_MessageStatus(msg);
  int high_nibble = status & 0xf0;
  int data1 = Pm_MessageData1(msg);
  int data2 = Pm_MessageData2(msg);

  switch (high_nibble) {
  case NOTE_ON: case NOTE_OFF: case POLY_PRESSURE:
    if (!inside_zone(msg))
      return;
    if (output_chan != -1)
      status = high_nibble + output_chan;
    data1 += xpose;
    midi_out(Pm_Message(status, data1, data2));
    break;
  case CONTROLLER:
    cc_msg = cc_maps[data1].process(msg, output_chan);
    if (cc_msg != -1)
      midi_out(cc_msg);
    break;
  case PROGRAM_CHANGE: case CHANNEL_PRESSURE: case PITCH_BEND:
    if (output_chan != -1)
      status = high_nibble + output_chan;
    midi_out(Pm_Message(status, data1, data2));
    break;
  default:
    midi_out(msg);
    break;
  }
}

int Connection::accept_from_input(PmMessage msg) {
  if (input_chan == -1)
    return true;
  unsigned char status = Pm_MessageStatus(msg);

  if (status >= SYSEX)
    return true;
  if (input_chan != (status & 0x0f))
    return false;

  if ((status & 0xf0) == CONTROLLER) {
    unsigned char controller = Pm_MessageData1(msg);
    if (cc_maps[controller].filtered)
      return false;
  }
  return true;
}

int Connection::inside_zone(PmMessage msg) {
  int note = Pm_MessageData1(msg);
  if (note < zone.low)
    return false;
  if (zone.high != -1 && note > zone.high)
    return false;
  return true;
}

void Connection::midi_out(PmMessage message) {
  PmEvent event = {message, 0};
  output->write(&event, 1);
}
