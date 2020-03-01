#include "patch_connections.h"
#include "../patchmaster.h"
#include "../cursor.h"
#include "../formatter.h"

#define CW 36

const char * const COLUMN_HEADERS[] = {
  "Input", "Chan", "Output", "Chan", "Zone", "Xpose", "Prog", "Sysex", "CC Filt/Map"
};
const int COLUMN_WIDTHS[] = {
  3*CW, 1*CW, 3*CW, 1*CW, 2*CW, 1*CW, 2*CW, int(1.5*CW), 6*CW
};

PatchConnections::PatchConnections(wxWindow *parent, wxWindowID id)
  : wxListCtrl(parent, id, wxDefaultPosition, wxSize(600, 150), wxLC_REPORT),
    patch(nullptr)
{
  set_headers();
}

void PatchConnections::update() {
  PatchMaster *pm = PatchMaster_instance();
  Cursor *cursor = pm->cursor;
  Patch *curr_patch = cursor->patch();

  // if (patch == curr_patch)
  //   return;

  patch = curr_patch;
  ClearAll();
  set_headers();
  if (patch == nullptr)
    return;

  int i = 0;
  for (auto* conn : patch->connections) {
    InsertItem(i, conn->input->name.c_str());
    SetItem(i, 1, conn->input_chan == -1 ? "all" : wxString::Format("%d", conn->input_chan + 1));
    SetItem(i, 2, conn->output->name.c_str());
    SetItem(i, 3, conn->output_chan == -1 ? "all" : wxString::Format("%d", conn->output_chan + 1));
    if (conn->zone.low != -1 || conn->zone.high != -1)
      SetItem(i, 4, wxString::Format("%3d - %3d", conn->zone.low, conn->zone.high));
    if (conn->xpose != -1)
      SetItem(i, 5, wxString::Format("%c%2d", conn->xpose < 0 ? '-' : ' ', abs(conn->xpose)));

    char buf[BUFSIZ];
    format_program(conn->prog, buf);
    SetItem(i, 6, buf);

    SetItem(i, 7, conn->pass_through_sysex ? "yes" : "no");

    format_controllers(conn, buf);
    SetItem(i, 8, buf);
    ++i;
  }
}

void PatchConnections::set_headers() {
  for (int i = 0; i < sizeof(COLUMN_HEADERS) / sizeof(const char * const); ++i) {
    InsertColumn(i, COLUMN_HEADERS[i]);
    SetColumnWidth(i, COLUMN_WIDTHS[i]);
  }
}
