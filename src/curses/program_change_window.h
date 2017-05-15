#ifndef PROGRAM_CHANGE_WINDOW_H
#define PROGRAM_CHANGE_WINDOW_H

#include <vector>
#include "window.h"
#include "../consts.h"

using namespace std;

class ProgramChangeWindow : public Window {
public:
  vector<Input *> &inputs;

  ProgramChangeWindow(struct rect, const char *, vector<Input *>&);
  ~ProgramChangeWindow();

  void draw();

private:
  void draw_input(Input *);
  void draw_program(program, int);

  bool something_to_draw(Input *);
  bool something_to_draw(program);
};

#endif /* PROGRAM_CHANGE_WINDOW_H */
