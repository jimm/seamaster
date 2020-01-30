#include "patch_list.h"
#include "../input.h"
#include "../output.h"
#include "../formatter.h"

const char * const COLUMN_HEADERS[] = {
  "Input", "Chan", "Output", "Chan", "Zone", "Xpose", "Prog", "CC Filt/Map"
};

PatchList::PatchList(wxWindow *parent)
  : wxListCtrl(parent, wxID_ANY, wxDefaultPosition, wxSize(600, 150), wxLC_REPORT),
    patch(nullptr)
{
  set_headers();
}

void PatchList::set_patch(Patch *p) {
  patch = p;
  ClearAll();
  set_headers();
  if (patch == nullptr)
    return;

  int i = 0;
  for (auto* conn : patch->connections) {
    InsertItem(i, conn->input->name.c_str());
    SetItem(i, 1, conn->input_chan == -1 ? "all" : wxString::Format("%d", conn->input_chan));
    SetItem(i, 2, conn->output->name.c_str());
    SetItem(i, 3, conn->output_chan == -1 ? "all" : wxString::Format("%d", conn->output_chan));
    if (conn->zone.low != -1 || conn->zone.high != -1)
      SetItem(i, 4, wxString::Format("%3d - %3d", conn->zone.low, conn->zone.high));
    if (conn->xpose != -1)
      SetItem(i, 5, wxString::Format("%c%2d", conn->xpose < 0 ? '-' : ' ', abs(conn->xpose)));

    char buf[BUFSIZ];
    format_program(conn->prog, buf);
    SetItem(i, 6, buf);

    format_controllers(conn, buf);
    SetItem(i, 7, buf);
    ++i;
  }
}

void PatchList::set_headers() {
  for (int i = 0; i < sizeof(COLUMN_HEADERS) / sizeof(const char * const); ++i)
    InsertColumn(i, COLUMN_HEADERS[i]);
}
