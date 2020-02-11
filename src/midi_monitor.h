#ifndef MIDI_MONITOR_H
#define MIDI_MONITOR_H

#include <portmidi.h>

class Input;
class Output;

// An abstract class that defines two public methods for monitoring input
// and output MIDI messages. Those methods by default call the do_* versions
// in a separate thread.
class MIDIMonitor {
public:
  virtual ~MIDIMonitor() {}

  virtual void monitor_input(Input *input, PmMessage msg) {}
  virtual void monitor_output(Output *output, PmMessage msg) {}
};

// An abstract class that defines two public methods for monitoring input
// and output MIDI messages asynchronourly. Those methods by default call
// the do_* versions in a separate thread.
class AsyncMIDIMonitor : public MIDIMonitor {
public:
  virtual ~AsyncMIDIMonitor() {}

  virtual void monitor_input(Input *input, PmMessage msg);
  virtual void monitor_output(Output *output, PmMessage msg);

  virtual void do_monitor_input(Input *input, PmMessage msg) {}
  virtual void do_monitor_output(Output *input, PmMessage msg) {}
};

#endif /* MIDI_MONITOR_H */
