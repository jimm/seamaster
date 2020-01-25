#ifndef MIDI_MONITOR_WINDOW_H
#define MIDI_MONITOR_WINDOW_H

#include <deque>
#include "window.h"
#include "../patchmaster.h"
#include "../midi_monitor.h"
#include "../input.h"
#include "../output.h"

class MIDIMonitorWindow : public Window, MIDIMonitor {
public:
  MIDIMonitorWindow(struct rect, PatchMaster *);
  ~MIDIMonitorWindow();

  void monitor_input(Input *input, PmMessage msg);
  void monitor_output(Output *output, PmMessage msg);

  void draw();

private:
  PatchMaster *pm;
  deque<string> input_lines;
  deque<string> output_lines;

  void add_message(deque<string> &lines, string sym, PmMessage msg);
  void draw_lines(deque<string> &lines, int col);
};

#endif /* MIDI_MONITOR_WINDOW_H */
