#include <stdlib.h>
#include <string.h>
#include "patch_window.h"
#include "../input.h"
#include "../output.h"

PatchWindow::PatchWindow(struct rect r, const char *title_prefix)
  : Window(r, title_prefix)
{
  patch = 0;
}

PatchWindow::~PatchWindow() {
}

void PatchWindow::set_contents(Patch *p) {
  title = p == 0 ? 0 : p->name;
  patch = p;
  draw();
}

void PatchWindow::draw() {
  Window::draw();
  wmove(win, 1, 1);
  draw_headers();
  if (patch == 0)
    return;

  list *conns = patch->connections;
  for (int i = 0; i < list_length(conns); ++i) {
    wmove(win, i+2, 1);
    draw_connection((Connection *)list_at(conns, i));
  }
}

void PatchWindow::draw_headers() {
  wattron(win, A_REVERSE);
  const char *str = " Input            Chan | Output           Chan | Zone      | Xpose | Prog";
  waddstr(win, str);
  for (int i = 0; i < getmaxx(win) - 2 - strlen(str); ++i)
    waddch(win, ' ');
  wattroff(win, A_REVERSE);
}

void PatchWindow::draw_connection(Connection *conn) {  
  int vis_height = visible_height();
  char buf[1024];

  format_chans(conn, buf);
  format_zone(conn, buf);
  format_xpose(conn, buf);
  format_prog(conn, buf);

  char *fitted = make_fit(buf, 0);
  waddstr(win, fitted);
  free(fitted);
}

void PatchWindow::format_chans(Connection *conn, char *buf) {
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
          conn->input->name.c_str(), inchan,
          conn->output->name.c_str(), outchan);
}

void PatchWindow::format_prog(Connection *conn, char *buf) {
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

void PatchWindow::format_zone(Connection *conn, char *buf) {
  if (conn->zone.low != -1 || conn->zone.high != -1)
    sprintf(buf + strlen(buf), " %3d - %3d |", conn->zone.low, conn->zone.high);
  else
    strcat(buf, "           |");
}

void PatchWindow::format_xpose(Connection *conn, char *buf) {
  if (conn->xpose != -1)
    sprintf(buf + strlen(buf), "  %s%2d |", conn->xpose < 0 ? "-" : " ", abs(conn->xpose));
  else
    strcat(buf, "      |");
}
