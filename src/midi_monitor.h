#ifndef MIDI_MONITOR_H
#define MIDI_MONITOR_H

class Input;
class Output;

// An abstract class that defines two public methods for monitoring input
// and output MIDI messages.
class MIDIMonitor {
public:
  void monitor_input(Input *input, PmMessage msg) {}
  void monitor_output(Output *output, PmMessage msg) {}
};

#endif /* MIDI_MONITOR_H */
