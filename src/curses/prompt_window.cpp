#include <stdlib.h>
#include <ncurses.h>
#include "prompt_window.h"

#define MAX_WIDTH 30

static struct rect empty_rect = {0, 0, 0, 0};

PromptWindow::PromptWindow(const char *title_str, const char *prompt_str)
  : Window(empty_rect, 0)
{
  title = title_str;
  prompt = prompt_str;

  int width = COLS / 2;
  if (width > MAX_WIDTH)
    width = MAX_WIDTH;
  rect.height = 4;
  rect.width = width;
  rect.row = LINES / 3;
  rect.col = (COLS - width) / 2;
}

PromptWindow::~PromptWindow() {
}

string PromptWindow::gets() {
  draw();
  return read_string();
}

void PromptWindow::draw() {
  Window::draw();

  wmove(win, 0, 1);
  wattron(win, A_REVERSE);
  waddch(win, ' ');
  waddstr(win, title.c_str());
  waddch(win, ' ');
  wattroff(win, A_REVERSE);

  wmove(win, 1, 1);
  waddstr(win, prompt.c_str());

  wmove(win, 2, 1);
  refresh();
}

string PromptWindow::read_string() {
  char *str = (char *)malloc(BUFSIZ);

  nocbreak();
  echo();
  curs_set(1);
  wattron(win, A_REVERSE);
  wgetstr(win, str);
  wattroff(win, A_REVERSE);
  curs_set(0);
  noecho();
  cbreak();

  return string(str);
}
