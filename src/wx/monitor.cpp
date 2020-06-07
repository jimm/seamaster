#include <wx/listctrl.h>
#include "monitor.h"
#include "../seamaster.h"

#define LIST_WIDTH 335
#define LIST_HEIGHT 500
#define MONITOR_LIST_LEN 50
#define TIMER_MILLISECS 100
#define TIMER_ID 10000

const char * const COLUMN_HEADERS[] = {
  "Inst", "Status", "Data 1", "Data 2"
};

wxBEGIN_EVENT_TABLE(Monitor, wxFrame)
  EVT_TIMER(TIMER_ID, Monitor::on_timer)
wxEND_EVENT_TABLE()


Monitor::Monitor()
  : wxFrame(NULL, wxID_ANY, "MIDI Monitor"),
    modified(false), timer(this, TIMER_ID)
{     
  wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);

  sizer->Add(make_input_panel(this), wxEXPAND);
  sizer->Add(make_output_panel(this), wxEXPAND);

  wxBoxSizer * const outer_border_sizer = new wxBoxSizer(wxVERTICAL);
  outer_border_sizer->Add(sizer, wxSizerFlags().Expand().Border());
  SetSizerAndFit(outer_border_sizer);
  Show(true);

  SeaMaster *sm = SeaMaster_instance();
  for (auto& input : sm->inputs)
    input->set_monitor(this);
  for (auto& output : sm->outputs)
    output->set_monitor(this);

  timer.Start(TIMER_MILLISECS);
}

Monitor::~Monitor() {
  SeaMaster *sm = SeaMaster_instance();
  for (auto& input : sm->inputs)
    input->set_monitor(nullptr);
  for (auto& output : sm->outputs)
    output->set_monitor(nullptr);
}

void Monitor::monitor_input(Input *input, PmMessage msg) {
  add_message(input, input_list, msg, input_messages);
}

void Monitor::monitor_output(Output *output, PmMessage msg) {
  add_message(output, output_list, msg, output_messages);
}

void Monitor::add_message(Instrument *inst, wxListCtrl *list, PmMessage msg, deque<MonitorMessage> &message_list) {
  if (message_list.size() >= MONITOR_LIST_LEN)
    message_list.pop_front();
  message_list.push_back(MonitorMessage(inst, msg));

  modified = true;
}

wxWindow *Monitor::make_input_panel(wxWindow *parent) {
  return make_panel(parent, "Inputs", &input_list);
}

wxWindow *Monitor::make_output_panel(wxWindow *parent) {
  return make_panel(parent, "Outputs", &output_list);
}

wxWindow *Monitor::make_panel(wxWindow *parent, const char * const title,
                              wxListCtrl **list_ptr)
{
  wxPanel *p = new wxPanel(parent, wxID_ANY);
  *list_ptr = new wxListCtrl(p, wxID_ANY, wxDefaultPosition, wxSize(LIST_WIDTH, LIST_HEIGHT),
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

void Monitor::on_timer(wxTimerEvent &event) {
  if (modified) {
    update_list(input_list, input_messages);
    update_list(output_list, output_messages);
    Refresh();
    Update();
    modified = false;
  }
}

void Monitor::update_list(wxListCtrl *list, deque<MonitorMessage> &message_list) {
  int row = 0;
  for (auto& mon_message : message_list) {
    list->SetItem(row, 0, mon_message.instrument->name.c_str());
    PmMessage msg = mon_message.message;
    list->SetItem(row, 1, wxString::Format("%02x", Pm_MessageStatus(msg)));
    list->SetItem(row, 2, wxString::Format("%02x", Pm_MessageData1(msg)));
    list->SetItem(row, 3, wxString::Format("%02x", Pm_MessageData2(msg)));
    ++row;
  }
}
