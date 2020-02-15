#include <stdlib.h>
#include "consts.h"
#include "connection.h"
#include "input.h"
#include "output.h"

Connection::Connection(Input *in, int in_chan, Output *out, int out_chan)
  : input(in), input_chan(in_chan), output(out), output_chan(out_chan),
    pass_through_sysex(false)
{
  prog.bank_msb = prog.bank_lsb = prog.prog = UNDEFINED;
  zone.low = zone.high = UNDEFINED;
  xpose = 0;
  for (int i = 0; i < 128; ++i)
    cc_maps[i] = nullptr;
}

Connection::~Connection() {
  for (int i = 0; i < 128; ++i)
    if (cc_maps[i] != nullptr)
      delete cc_maps[i];
}

void Connection::start() {
  if (output_chan != CONNECTION_ALL_CHANNELS) {
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

// Takes a MIDI message `msg` from an input, processes it, and sends it to
// an output (unless it's been filtered out).
void Connection::midi_in(PmMessage msg) {
  // See if the message should even be processed, or if we should stop here.
  if (!input_channel_ok(msg))
      return;

  PmMessage cc_msg;
  int status = Pm_MessageStatus(msg);
  int high_nibble = status & 0xf0;
  int data1 = Pm_MessageData1(msg);
  int data2 = Pm_MessageData2(msg);
  Controller *cc;

  switch (high_nibble) {
  case NOTE_ON: case NOTE_OFF: case POLY_PRESSURE:
    if (!inside_zone(msg))
      return;
    if (output_chan != CONNECTION_ALL_CHANNELS)
      status = high_nibble + output_chan;
    data1 += xpose;
    midi_out(Pm_Message(status, data1, data2));
    break;
  case CONTROLLER:
    cc = cc_maps[data1];
    if (cc != nullptr) {
      cc_msg = cc->process(msg, output_chan);
      if (cc_msg != CONTROLLER_BLOCK)
        midi_out(cc_msg);
    }
    else {
      if (output_chan != CONNECTION_ALL_CHANNELS)
        status = high_nibble + output_chan;
      midi_out(Pm_Message(status, data1, data2));
    }
    break;
  case PROGRAM_CHANGE: case CHANNEL_PRESSURE: case PITCH_BEND:
    if (output_chan != CONNECTION_ALL_CHANNELS)
      status = high_nibble + output_chan;
    midi_out(Pm_Message(status, data1, data2));
    break;
  default:
    midi_out(msg);
    break;
  }
}

void Connection::add_controller(Controller *controller) {
  cc_maps[controller->cc_num] = controller;
}

// Returns `true` if any one of the following are true:
// - we accept any input channel
// - it's a system message, not a channel message
// - the input channel matches our selected `input_chan`
int Connection::input_channel_ok(PmMessage msg) {
  if (input_chan == CONNECTION_ALL_CHANNELS)
    return true;

  unsigned char status = Pm_MessageStatus(msg);
  return status >= SYSEX || input_chan == (status & 0x0f);
}

int Connection::inside_zone(PmMessage msg) {
  int note = Pm_MessageData1(msg);
  if (note < zone.low)
    return false;
  if (zone.high != UNDEFINED && note > zone.high)
    return false;
  return true;
}

void Connection::midi_out(PmMessage message) {
  PmEvent event = {message, 0};
  output->write(&event, 1);
}
