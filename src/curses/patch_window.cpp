#include <stdlib.h>
#include <string.h>
#include "patch_window.h"
#include "../input.h"
#include "../output.h"

PatchWindow::PatchWindow(struct rect r, const char *title_prefix, int imaxlen, int omaxlen)
  : Window(r, title_prefix), patch(0), max_input_name_len(imaxlen), max_output_name_len(omaxlen)
{
}

PatchWindow::~PatchWindow() {
}

void PatchWindow::set_contents(Patch *p) {
  title = p == 0 ? 0 : p->name;
  patch = p;
}

void PatchWindow::draw() {
  Window::draw();
  wmove(win, 1, 1);
  draw_headers();
  if (patch == 0)
    return;

  for (int i = 0; i < patch->connections.length(); ++i) {
    wmove(win, i+2, 1);
    draw_connection(patch->connections.at(i));
  }
}

void PatchWindow::draw_headers() {
  wattron(win, A_REVERSE);
  string str = " Input";
  for (int i = 0; i < (max_input_name_len - 6); ++i)
    str += ' ';
  str += "  Chan | Output";
  for (int i = 0; i < (max_output_name_len - 7); ++i)
    str += ' ';
  str += "  Chan | Zone      | Xpose | Prog            | CC Filters/Maps";
  make_fit(str, 0);
  waddstr(win, str.c_str());
  for (int i = 0; i < getmaxx(win) - 2 - str.length(); ++i)
    waddch(win, ' ');
  wattroff(win, A_REVERSE);
}

void PatchWindow::draw_connection(Connection *conn) {  
  int vis_height = visible_height();
  char buf[BUFSIZ], fitbuf[BUFSIZ];

  format_chans(conn, buf);
  format_zone(conn, buf);
  format_xpose(conn, buf);
  format_prog(conn, buf);
  format_filters_and_maps(conn, buf);

  make_fit(buf, 1, fitbuf);
  waddstr(win, fitbuf);
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

  sprintf(buf, " %*s  %3s | %*s  %3s |",
          max_input_name_len, conn->input->name.c_str(), inchan,
          max_output_name_len, conn->output->name.c_str(), outchan);
}

void PatchWindow::format_zone(Connection *conn, char *buf) {
  if (conn->zone.low != -1 || conn->zone.high != -1)
    sprintf(buf + strlen(buf), " %3d - %3d |", conn->zone.low, conn->zone.high);
  else
    strcat(buf, "           |");
}

void PatchWindow::format_xpose(Connection *conn, char *buf) {
  if (conn->xpose != -1)
    sprintf(buf + strlen(buf), "   %c%2d |", conn->xpose < 0 ? '-' : ' ', abs(conn->xpose));
  else
    strcat(buf, "       |");
}

void PatchWindow::format_prog(Connection *conn, char *buf) {
  int has_msb = conn->prog.bank_msb != -1;
  int has_lsb = conn->prog.bank_lsb != -1;
  int has_bank = has_msb || has_lsb;

  buf += strlen(buf);

  sprintf(buf, " %c", has_bank ? '[' : ' ');
  buf += 2;

  if (has_msb)
    sprintf(buf, "%3d", conn->prog.bank_msb);
  else
    strcat(buf, "   ");
  buf += 3;

  sprintf(buf, "%c ", has_bank ? ',' : ' ');
  buf += 2;

  if (has_lsb)
    sprintf(buf, "%3d", conn->prog.bank_lsb);
  else
    strcat(buf, "   ");
  buf += 3;

  sprintf(buf, "%c ", has_bank ? ']' : ' ');
  buf += 2;

  if (conn->prog.prog != -1)
    sprintf(buf, " %3d |", conn->prog.prog);
  else
    strcat(buf, "     |");
}

void PatchWindow::format_filters_and_maps(Connection *conn, char *buf) {
  int first = true;

  buf += strlen(buf);
  strcat(buf, " ");
  buf += 1;
  for (int i = 0; i < 128; ++i) {
    int m = conn->cc_maps[i];
    if (m == -1) {
      if (first) first = false; else { strcat(buf, ", "); buf += 2; }
      format_filter(i, buf);
      buf += strlen(buf);
    }
    else if (m != i) {
      if (first) first = false; else { strcat(buf, ", "); buf += 2; }
      format_map(i, m, buf);
      buf += strlen(buf);
    }
  }
}

void PatchWindow::format_filter(int i, char *buf) {
  sprintf(buf, "%dx", i);
}

void PatchWindow::format_map(int from, int to, char *buf) {
  sprintf(buf, "%d->%d", from, to);
}
