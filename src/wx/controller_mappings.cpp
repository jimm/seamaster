#include "controller_mappings.h"
#include "../patchmaster.h"
#include "../cursor.h"
#include "../formatter.h"

#define CW 48

const char * const COLUMN_HEADERS[] = {
  "CC In", "CC Out", "Min", "Max", "Filtered"
};
const int COLUMN_WIDTHS[] = {
  CW, CW, CW, CW, 2*CW
};

ControllerMappings::ControllerMappings(wxWindow *parent, wxWindowID id, Connection *conn)
  : wxListCtrl(parent, id, wxDefaultPosition, wxSize(600, 150),
               wxLC_REPORT | wxLC_SINGLE_SEL),
    connection(conn)
{
  set_headers();
}

void ControllerMappings::update() {
  ClearAll();
  set_headers();
  if (connection == nullptr)
    return;

  int row = 0;
  for (int i = 0, row = 0; i < 128; ++i) {
    Controller *controller = connection->cc_maps[i];
    if (controller == nullptr)
      continue;

    InsertItem(row, wxString::Format("%d", controller->cc_num));
    SetItem(row, 1, wxString::Format("%d", controller->translated_cc_num));
    SetItem(row, 2, wxString::Format("%d", controller->min));
    SetItem(row, 3, wxString::Format("%d", controller->max));
    SetItem(row, 3, controller->filtered ? "yes" : "no");
    ++row;
  }
}

void ControllerMappings::set_headers() {
  for (int i = 0; i < sizeof(COLUMN_HEADERS) / sizeof(const char * const); ++i) {
    InsertColumn(i, COLUMN_HEADERS[i]);
    SetColumnWidth(i, COLUMN_WIDTHS[i]);
  }
}
