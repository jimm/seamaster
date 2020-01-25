#include <sstream>
#include "midi_monitor_window.h"
#include "../formatter.h"

MIDIMonitorWindow::MIDIMonitorWindow(struct rect r, PatchMaster *pm)
  : Window(r, "MIDI Monitor")
{
  fprintf(stderr, "MIDIMonitorWindow constructor\n"); // DEBUG
  fprintf(stderr, "  rect (%d, %d, %d, %d)\n", r.row, r.col, r.height, r.width); // DEBUG
  fprintf(stderr, "  pm %p\n", pm);      // DEBUG
  fflush(stderr);                                    // DEBUG
  for (auto *input : pm->inputs)
    input->set_monitor(this);
  for (auto *output : pm->outputs)
    output->set_monitor(this);
}

MIDIMonitorWindow::~MIDIMonitorWindow() {
  fprintf(stderr, "MIDIMonitorWindow destructor\n"); // DEBUG
  fprintf(stderr, "  pm %p\n", pm);                  // DEBUG
  fflush(stderr);                                    // DEBUG
  for (auto *input : pm->inputs)
    input->set_monitor(nullptr);
  for (auto *output : pm->outputs)
    output->set_monitor(nullptr);
}

void MIDIMonitorWindow::monitor_input(Input *input, PmMessage msg) {
  add_message(input_lines, input->sym, msg);
  draw();
}

void MIDIMonitorWindow::monitor_output(Output *output, PmMessage msg) {
  add_message(output_lines, output->sym, msg);
  draw();
}

void MIDIMonitorWindow::add_message(deque<string> &lines, string sym, PmMessage msg) {
  if (lines.size() > getmaxx(stdscr) - 2)
    lines.pop_front();

  ostringstream ostr;
  ostr << sym
       << '\t' << byte_to_hex(Pm_MessageStatus(msg))
       << ' ' << byte_to_hex(Pm_MessageData1(msg))
       << ' ' << byte_to_hex(Pm_MessageData2(msg));
  lines.push_back(ostr.str());
}

void MIDIMonitorWindow::draw() {
  Window::draw();
  draw_lines(input_lines, 1);
  draw_lines(output_lines, visible_width() / 2 + 1);
}

void MIDIMonitorWindow::draw_lines(deque<string> &lines, int col) {
  int start_line_num = 0;
  if (lines.size() > visible_height())
    start_line_num = lines.size() - visible_height() - 1;
  int row = 1;
  for (int i = start_line_num; i < lines.size(); ++i) {
    wmove(win, row++, col);
    waddstr(win, lines.at(i).c_str());
  }
}
