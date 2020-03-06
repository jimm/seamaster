#include "trigger_list.h"
#include "../patchmaster.h"
#include "../formatter.h"

#define CW 24
#define LIST_WIDTH 400
#define SHORT_LIST_HEIGHT 200

const char * const COLUMN_HEADERS[] = {
  "Input", "Trigger", "Action / Message"
};
const int COLUMN_WIDTHS[] = {
  3*CW, 3*CW, 8*CW
};

TriggerList::TriggerList(wxWindow *parent, wxWindowID id)
  : wxListCtrl(parent, id, wxDefaultPosition,
               wxSize(LIST_WIDTH, SHORT_LIST_HEIGHT),
               wxLC_REPORT | wxLC_SINGLE_SEL)
{
  set_headers();
}

void TriggerList::update() {
  PatchMaster *pm = PatchMaster_instance();

  ClearAll();
  set_headers();

  int row = 0;
  for (auto* input : pm->inputs) {
    for (auto * trigger : input->triggers) {
      InsertItem(row, input->name.c_str());
      SetItem(row, 1, message_to_wxstring(trigger->trigger_message));
      wxString str;
      switch (trigger->action) {
      case NEXT_SONG:
        str = "next song";
        break;
      case PREV_SONG:
        str = "prev song";
        break;
      case NEXT_PATCH:
        str = "next patch";
        break;
      case PREV_PATCH:
        str = "prev patch";
        break;
      case MESSAGE:
        str = trigger->output_message->name;
        break;
      }
      SetItem(row, 2, str);
      ++row;
    }
  }
}

void TriggerList::set_headers() {
  for (int i = 0; i < sizeof(COLUMN_HEADERS) / sizeof(const char * const); ++i) {
    InsertColumn(i, COLUMN_HEADERS[i]);
    SetColumnWidth(i, COLUMN_WIDTHS[i]);
  }
}

wxString TriggerList::message_to_wxstring(PmMessage msg) {
  wxString str = wxString::Format("%02x %02x %02x",
                                  Pm_MessageStatus(msg),
                                  Pm_MessageData1(msg),
                                  Pm_MessageData2(msg));
  return str;
}
