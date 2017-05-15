#include <sstream>
#include <stdlib.h>
#include <string.h>
#include "../consts.h"
#include "../connection.h"
#include "../input.h"
#include "../formatter.h"
#include "program_change_window.h"
#include "info_window.h"

ProgramChangeWindow::ProgramChangeWindow(struct rect r, const char *pc_title, vector<Input *> &ilist)
  : Window(r, 0), inputs(ilist)
{
  title = pc_title;
}

ProgramChangeWindow::~ProgramChangeWindow() {
}

void ProgramChangeWindow::draw() {
  Window::draw();
  for (vector<Input *>::iterator i = inputs.begin(); i != inputs.end(); ++i)
    if (something_to_draw(*i))
      draw_input(*i);
}

void ProgramChangeWindow::draw_input(Input *in) {
  wmove(win, getcury(win) + 1, 1);
  wprintw(win, "Input %s", in->sym.c_str());
  for (int i = 0; i < MIDI_CHANNELS; ++i)
    if (something_to_draw(in->last_program_change_seen(i)))
      draw_program(in->last_program_change_seen(i), i+1);
}

void ProgramChangeWindow::draw_program(program p, int chan) {
  char buf[BUFSIZ];
  format_program(p, buf);

  wmove(win, getcury(win) + 1, 1);
  wprintw(win, "  Channel %d: %s", chan, buf);
}

bool ProgramChangeWindow::something_to_draw(Input *in) {
  for (int i = 0; i < MIDI_CHANNELS; ++i)
    if (something_to_draw(in->last_program_change_seen(i)))
      return true;
  return false;
}

bool ProgramChangeWindow::something_to_draw(program p) {
  return p.bank_msb != -1 || p.bank_lsb != -1 || p.prog != -1;
}
