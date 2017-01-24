#include <stdlib.h>
#include <string.h>
#include "patch_window.h"
#include "../input.h"
#include "../output.h"

void patch_window_draw_headers(patch_window *);
void patch_window_draw_connection(patch_window *, connection *);

patch_window *patch_window_new(rect r, char *title_prefix) {
  patch_window *pw = malloc(sizeof(patch_window));
  pw->w = window_new(r, title_prefix);
  pw->patch = 0;
  return pw;
}

void patch_window_free(patch_window * pw) {
  free(pw);
}

void patch_window_set_contents(patch_window *pw, patch *patch) {
  pw->w->title = patch == 0 ? 0 : patch->name;
  pw->patch = patch;
  patch_window_draw(pw);
}

void patch_window_draw(patch_window *pw) {
  window_draw(pw->w);
  wmove(pw->w->win, 1, 1);
  patch_window_draw_headers(pw);
  if (pw->patch == 0)
    return;

  list *conns = pw->patch->connections;
  for (int i = 0; i < list_length(conns); ++i) {
    wmove(pw->w->win, i+2, 1);
    patch_window_draw_connection(pw, (connection *)list_at(conns, i));
  }
}

void patch_window_draw_headers(patch_window *pw) {
  wattron(pw->w->win, A_REVERSE);
  char *str = " Input          Chan | Output         Chan | Prog | Zone      | Xpose | Filter";
  waddstr(pw->w->win, str);
  for (int i = 0; i < getmaxx(pw->w->win) - 2 - strlen(str); ++i)
    waddch(pw->w->win, ' ');
  wattroff(pw->w->win, A_REVERSE);
}

void patch_window_draw_connection(patch_window *pw, connection *conn) {  
  int vis_height = window_visible_height(pw->w);
  char buf[1024];

  sprintf(buf, " %16s %2d %16s %2d |",
          conn->input->name, conn->input_chan + 1,
          conn->output->name, conn->output_chan + 1);

  // FIXME banks, too
  if (conn->prog.prog != -1)
    sprintf((buf + strlen(buf)), "  %3d |", conn->prog.prog);
  else
    strcpy((buf + strlen(buf)), "      |");

  if (conn->zone.low != -1 || conn->zone.high != -1)
    sprintf((buf + strlen(buf)), " %3d - %3d |", conn->zone.low, conn->zone.high);
  else
    strcpy((buf + strlen(buf)), "           |");

  if (conn->xpose != -1)
    sprintf((buf + strlen(buf)), " %s%2d |", conn->xpose < 0 ? "" : " ", conn->xpose);
  else
    strcpy((buf + strlen(buf)), "       |");

  // TODO filter

  char *fitted = window_make_fit(pw->w, buf, 0);
  waddstr(pw->w->win, fitted);
  free(fitted);
}
