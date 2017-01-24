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

  fprintf(stderr, "drawing patch id %d, name %s\n", pw->patch->id, pw->patch->name); /* DEBUG */

  list *conns = pw->patch->connections;
  fprintf(stderr, "there are %d connections in that patch\n", list_length(conns)); /* DEBUG */
  for (int i = 0; i < list_length(conns); ++i) {
    fprintf(stderr, "about to draw conn %d\n", i); /* DEBUG */
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
  fprintf(stderr, "patch_window_draw_connection\n"); /* DEBUG */
  int vis_height = window_visible_height(pw->w);
  char buf[1024];

  fprintf(stderr, "conn input %p\n", conn->input); /* DEBUG */
  fprintf(stderr, "conn output %p\n", conn->output); /* DEBUG */
  sprintf(buf, " %16s %2d %16s %2d |",
          conn->input->name, conn->input_chan + 1,
          conn->output->name, conn->output_chan + 1);
  fprintf(stderr, "buf = %s\n", buf); /* DEBUG */

  // FIXME banks, too
  if (conn->pc_prog != -1)
    sprintf((buf + strlen(buf)), "  %3d |", conn->pc_prog);
  else
    strcpy((buf + strlen(buf)), "      |");

  if (conn->zone_low != -1 || conn->zone_high != -1)
    sprintf((buf + strlen(buf)), " %3d - %3d |", conn->zone_low, conn->zone_high);
  else
    strcpy((buf + strlen(buf)), "           |");

  if (conn->xpose != -1)
    sprintf((buf + strlen(buf)), " %s%2d |", conn->xpose < 0 ? "" : " ", conn->xpose);
  else
    strcpy((buf + strlen(buf)), "       |");

  // TODO filter

  fprintf(stderr, "here we go, making fit\n"); /* DEBUG */
  char *fitted = window_make_fit(pw->w, buf, 0);
  fprintf(stderr, "fitted = %s\n", fitted); /* DEBUG */
  waddstr(pw->w->win, fitted);
  fprintf(stderr, "done, freeing fitted\n"); /* DEBUG */
  free(fitted);
  fprintf(stderr, "done freeing fitted, returning\n"); /* DEBUG */
}
