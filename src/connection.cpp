#include <stdlib.h>
#include "consts.h"
#include "connection.h"
#include "input.h"
#include "output.h"
#include "debug.h"

Connection::Connection(Input *in, int in_chan, Output *out, int out_chan)
  : input(in), input_chan(in_chan), output(out), output_chan(out_chan)
{
  prog.bank_msb = prog.bank_lsb = prog.prog = -1;
  zone.low = zone.high = -1;
  xpose = 0;
  for (int i = 0; i < 128; ++i)
    cc_maps[i] = i;
}

Connection::~Connection() {
}

void Connection::start(PmMessage *buf, int len) {
  vdebug("connection_start %p\n", this);
  for (int i = 0; i < len; ++i)
    midi_out(buf[i]);
  if (prog.bank_msb >= 0)
    midi_out(Pm_Message(CONTROLLER + output_chan, CC_BANK_SELECT_MSB, prog.bank_msb));
  if (prog.bank_lsb >= 0)
    midi_out(Pm_Message(CONTROLLER + output_chan, CC_BANK_SELECT_LSB, prog.bank_lsb));
  if (prog.prog >= 0)
    midi_out(Pm_Message(PROGRAM_CHANGE + output_chan, prog.prog, 0));

  input->add_connection(this);
}

void Connection::stop(PmMessage *buf, int len) {
  vdebug("connection_stop %p\n", this);
  for (int i = 0; i < len; ++i)
    midi_out(buf[i]);
  input->remove_connection(this);
}

void Connection::midi_in(PmMessage msg) {
  vdebug("connection_midi_in %p, message %p\n", this, msg);
  if (!accept_from_input(msg))
      return;

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
  case CONTROLLER: case PROGRAM_CHANGE: case CHANNEL_PRESSURE: case PITCH_BEND:
    if (output_chan != -1)
      status = high_nibble + output_chan;
    if (high_nibble == CONTROLLER) /* map controller number */
      data1 = cc_maps[data1]; /* won't be -1, that's already filtered */
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
  if (input_chan != (Pm_MessageStatus(msg) & 0x0f))
    return false;

  if ((status & 0xf0) == CONTROLLER) {
    unsigned char controller = Pm_MessageData1(msg);
    if (cc_maps[controller] == -1)
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
  vdebug("connection_midi_out %p, message %p\n", this, message);
  PmEvent event = {message, 0};
  output->write(&event, 1);
}

void Connection::debug() {
  vdebug("conn (%p), in [%s, %d], out [%s, %d], prog [%d, %d, %d], zone [%d, %d], xpose %d\n",
         this,
         input ? input->name : "(null input)",
         input ? input_chan : 0,
         output ? output->name : "(null output)",
         output ? output_chan : 0,
         prog.bank_msb, prog.bank_lsb, prog.prog,
         zone.low, zone.high,
         xpose);
}
