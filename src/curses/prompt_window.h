#ifndef PROMPT_WINDOW_H
#define PROMPT_WINDOW_H

#include "window.h"

class PromptWindow : public Window {
public:
  string prompt;

  PromptWindow(const char *title, const char *prompt);
  ~PromptWindow();

  string gets();

private:
  void draw();
  string read_string();
};

#endif /* PROMPT_WINDOW_H */
