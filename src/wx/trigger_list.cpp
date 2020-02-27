#include "trigger_list.h"
#include "../patchmaster.h"
#include "../formatter.h"

#define CW 24
#define LIST_WIDTH 400
#define SHORT_LIST_HEIGHT 200

const char * const COLUMN_HEADERS[] = {
  "Input", "Trigger", "Action", "Message"
};
const int COLUMN_WIDTHS[] = {
  3*CW, 3*CW, 4*CW, 6*CW
};

TriggerList::TriggerList(wxWindow *parent)
  : wxListCtrl(parent, wxID_ANY, wxDefaultPosition, wxSize(LIST_WIDTH, SHORT_LIST_HEIGHT),
               wxLC_REPORT)
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
      switch (trigger->action) {
      case NEXT_SONG:
        SetItem(row, 2, "next song");
        break;
      case PREV_SONG:
        SetItem(row, 2, "prev song");
        break;
      case NEXT_PATCH:
        SetItem(row, 2, "next patch");
        break;
      case PREV_PATCH:
        SetItem(row, 2, "prev patch");
        break;
      case MESSAGE:
        SetItem(row, 3, messages_to_wxstring(trigger->output_message->messages));
      }
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

wxString TriggerList::messages_to_wxstring(vector<PmMessage> messages) {
  if (messages.empty())
    return "";
  wxString str = message_to_wxstring(messages.at(0));
  if (messages.size() > 1)
    str += ", ...";
  return str;
}
