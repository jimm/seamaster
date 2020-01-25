#include <sstream>
#include "midi_monitor_window.h"
#include "../formatter.h"

MIDIMonitorWindow::MIDIMonitorWindow(struct rect r, PatchMaster *patchmaster)
  : Window(r, nullptr), pm(patchmaster)
{
  title = "MIDI Monitor (press 'm' to close)";
  for (auto *input : pm->inputs)
    input->set_monitor(this);
  for (auto *output : pm->outputs)
    output->set_monitor(this);
}

MIDIMonitorWindow::~MIDIMonitorWindow() {
  for (auto *input : pm->inputs)
    input->set_monitor(nullptr);
  for (auto *output : pm->outputs)
    output->set_monitor(nullptr);
}

void MIDIMonitorWindow::monitor_input(Input *input, PmMessage msg) {
  add_message(input_lines, input->sym, msg);
  draw();
  wnoutrefresh(win);
  doupdate();
}

void MIDIMonitorWindow::monitor_output(Output *output, PmMessage msg) {
  add_message(output_lines, output->sym, msg);
  draw();
  wnoutrefresh(win);
  doupdate();
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
  int col = visible_width() / 2;
  for (int row = 0; row < visible_height(); ++row) {
    wmove(win, row, col);
    waddch(win, ACS_VLINE);
  }
  draw_lines(input_lines, 1);
  draw_lines(output_lines, visible_width() / 2 + 1);
  refresh();
}

void MIDIMonitorWindow::draw_lines(deque<string> &lines, int col) {
  int start_line_num = 0;
  if (lines.size() > visible_height())
    start_line_num = lines.size() - visible_height() - 1;
  int row = 1;
  int max_width = visible_width() / 2 - 4;
  for (int i = start_line_num; i < lines.size(); ++i) {
    string line = lines.at(i);
    make_fit(line, max_width);
    wmove(win, row++, col);
    waddstr(win, line.c_str());
  }
}
