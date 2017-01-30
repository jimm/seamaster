#include <stdlib.h>
#include <string.h>
#include "patch_window.h"
#include "../input.h"
#include "../output.h"

void patch_window_draw_headers(patch_window *);
void patch_window_draw_connection(patch_window *, Connection *);

patch_window *patch_window_new(rect r, const char *title_prefix) {
  patch_window *pw = (patch_window *)malloc(sizeof(patch_window));
  pw->w = window_new(r, title_prefix);
  pw->patch = 0;
  return pw;
}

void patch_window_free(patch_window * pw) {
  free(pw);
}

void patch_window_set_contents(patch_window *pw, Patch *patch) {
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
    patch_window_draw_connection(pw, (Connection *)list_at(conns, i));
  }
}

void patch_window_draw_headers(patch_window *pw) {
  wattron(pw->w->win, A_REVERSE);
  const char *str = " Input            Chan | Output           Chan | Zone      | Xpose | Prog";
  waddstr(pw->w->win, str);
  for (int i = 0; i < getmaxx(pw->w->win) - 2 - strlen(str); ++i)
    waddch(pw->w->win, ' ');
  wattroff(pw->w->win, A_REVERSE);
}

void format_chans(patch_window *pw, Connection *conn, char *buf) {
  char inchan[4], outchan[4];

  if (conn->input_chan == -1)
    strcpy(inchan, "all");
  else
    sprintf(inchan, "%3d", conn->input_chan + 1);
  if (conn->output_chan == -1)
    strcpy(outchan, "all");
  else
    sprintf(outchan, "%3d", conn->output_chan + 1);

  sprintf(buf, " %16s  %3s | %16s  %3s |",
          conn->input->name, inchan, conn->output->name, outchan);
}

void format_prog(patch_window *pw, Connection *conn, char *buf) {
  int has_msb = conn->prog.bank_msb != -1;
  int has_lsb = conn->prog.bank_lsb != -1;
  int has_bank = has_msb || has_lsb;

  if (has_bank)
    strcat(buf, " [");
  if (has_msb)
    sprintf(buf+strlen(buf), "%d", conn->prog.bank_msb);
  if (has_bank)
    strcat(buf, ",");
  if (has_lsb)
    sprintf(buf+strlen(buf), "%d", conn->prog.bank_lsb);
  if (has_bank)
    strcat(buf, "]");

  if (conn->prog.prog != -1)
    sprintf(buf + strlen(buf), " %d", conn->prog.prog);
}

void format_zone(patch_window *pw, Connection *conn, char *buf) {
  if (conn->zone.low != -1 || conn->zone.high != -1)
    sprintf(buf + strlen(buf), " %3d - %3d |", conn->zone.low, conn->zone.high);
  else
    strcat(buf, "           |");
}

void format_xpose(patch_window *pw, Connection *conn, char *buf) {
  if (conn->xpose != -1)
    sprintf(buf + strlen(buf), "  %s%2d |", conn->xpose < 0 ? "-" : " ", abs(conn->xpose));
  else
    strcat(buf, "      |");
}

void patch_window_draw_connection(patch_window *pw, Connection *conn) {  
  int vis_height = window_visible_height(pw->w);
  char buf[1024];

  format_chans(pw, conn, buf);
  format_zone(pw, conn, buf);
  format_xpose(pw, conn, buf);
  format_prog(pw, conn, buf);

  char *fitted = window_make_fit(pw->w, buf, 0);
  waddstr(pw->w->win, fitted);
  free(fitted);
}
