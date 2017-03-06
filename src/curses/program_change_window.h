#ifndef PROGRAM_CHANGE_WINDOW_H
#define PROGRAM_CHANGE_WINDOW_H

#include "window.h"
#include "../consts.h"
#include "../list.h"

class ProgramChangeWindow : public Window {
public:
  List<Input *> &inputs;

  ProgramChangeWindow(struct rect, const char *, List<Input *>&);
  ~ProgramChangeWindow();

  void draw();

private:
  void draw_input(Input *);
  void draw_program(program, int);

  bool something_to_draw(Input *);
  bool something_to_draw(program);
};

#endif /* PROGRAM_CHANGE_WINDOW_H */
