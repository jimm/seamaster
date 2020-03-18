#include <wx/persist/toplevel.h>
#include <wx/listctrl.h>
#include "monitor.h"
#include "../patchmaster.h"

#define FRAME_NAME "seamaster_monitor_frame"
#define MONITOR_LIST_LEN 50

const char * const COLUMN_HEADERS[] = {
  "Inst", "Status", "Data 1", "Data 2"
};

Monitor::Monitor()
  : wxFrame(NULL, wxID_ANY, "MIDI Monitor", wxDefaultPosition, wxSize(480, 500))
{     
  wxPanel *p = new wxPanel(this, wxID_ANY);
  wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);

  sizer->Add(make_input_panel(p), wxEXPAND);
  sizer->Add(make_output_panel(p), wxEXPAND);

  for (int i = 0; i < sizeof(COLUMN_HEADERS) / sizeof(const char * const); ++i) {
    input_list->InsertColumn(i, COLUMN_HEADERS[i]);
    output_list->InsertColumn(i, COLUMN_HEADERS[i]);
  }

  p->SetSizerAndFit(sizer);
  SetClientSize(p->GetSize());
  Show(true);
  wxPersistentRegisterAndRestore(this, FRAME_NAME); // not working?

  PatchMaster *pm = PatchMaster_instance();
  for (auto& input : pm->inputs)
    input->set_monitor(this);
  for (auto& output : pm->outputs)
    output->set_monitor(this);
}

Monitor::~Monitor() {
  PatchMaster *pm = PatchMaster_instance();
  for (auto& input : pm->inputs)
    input->set_monitor(nullptr);
  for (auto& output : pm->outputs)
    output->set_monitor(nullptr);
}

void Monitor::monitor_input(Input *input, PmMessage msg) {
  add_message(input, input_list, msg, input_messages);
}

void Monitor::monitor_output(Output *output, PmMessage msg) {
  add_message(output, output_list, msg, output_messages);
}

void Monitor::add_message(Instrument *inst, wxListCtrl *list, PmMessage msg, vector<PmMessage> &message_list) {
  if (message_list.size() >= MONITOR_LIST_LEN)
    message_list.erase(message_list.begin());
  message_list.push_back(msg);

  int row = 0;
  for (auto& msg : message_list) {
    list->SetItem(row, 0, inst->name.c_str());
    list->SetItem(row, 1, wxString::Format("%02x", Pm_MessageStatus(msg)));
    list->SetItem(row, 2, wxString::Format("%02x", Pm_MessageData1(msg)));
    list->SetItem(row, 3, wxString::Format("%02x", Pm_MessageData2(msg)));
    ++row;
  }
  Refresh();
}

wxWindow *Monitor::make_input_panel(wxPanel *parent) {
  return make_panel(parent, "Inputs", &input_list);
}

wxWindow *Monitor::make_output_panel(wxPanel *parent) {
  return make_panel(parent, "Outputs", &output_list);
}

wxWindow *Monitor::make_panel(wxPanel *parent, const char * const title, wxListCtrl **list_ptr) {
  wxPanel *p = new wxPanel(parent, wxID_ANY);
  *list_ptr = new wxListCtrl(p, wxID_ANY, wxDefaultPosition, wxSize(200, 400),
                              wxLC_REPORT);

  for (int col = 0; col < sizeof(COLUMN_HEADERS) / sizeof(const char * const); ++col)
    (*list_ptr)->InsertColumn(col, COLUMN_HEADERS[col]);
  for (int row = 0; row < MONITOR_LIST_LEN; ++row) {
    (*list_ptr)->InsertItem(row, "");
    (*list_ptr)->SetItem(row, 1, "");
    (*list_ptr)->SetItem(row, 2, "");
    (*list_ptr)->SetItem(row, 3, "");
  }

  wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
  sizer->Add(new wxStaticText(p, wxID_ANY, title), wxSizerFlags().Align(wxALIGN_LEFT));
  sizer->Add(*list_ptr, wxSizerFlags(1).Expand().Border());

  p->SetSizerAndFit(sizer);
  return p;
}
