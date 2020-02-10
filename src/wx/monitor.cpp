#include <wx/persist/toplevel.h>
#include <wx/listctrl.h>
#include "monitor.h"
#include "../patchmaster.h"

#define FRAME_NAME "seamaster_monitor_frame"

const char * const COLUMN_HEADERS[] = {
  "Inst", "Status", "Data 1", "Data 2"
};

// TODO max list size of GetCountPerPage(), delete old ones
// TODO thread for adding items so we don't block caller
// TODO link input with output?

Monitor::Monitor()
  : wxFrame(NULL, wxID_ANY, "Instruments", wxDefaultPosition, wxSize(480, 500))
{     
  wxPanel *p = new wxPanel(this, wxID_ANY);
  wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);

  sizer->Add(make_input_panel(p), wxEXPAND);
  sizer->Add(make_output_panel(p), wxEXPAND);
  input_list_len = output_list_len = 0;

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

// FIXME don't just append
void Monitor::monitor_input(Input *input, PmMessage msg) {
  add_message(input, input_list, input_list_len++, msg);
}

// FIXME don't just append
void Monitor::monitor_output(Output *output, PmMessage msg) {
  add_message(output, output_list, output_list_len++, msg);
}

void Monitor::add_message(Instrument *inst, wxListCtrl *list, int row, PmMessage msg) {
  list->InsertItem(row, inst->sym.c_str());
  list->SetItem(row, 1, wxString::Format("%02x", Pm_MessageStatus(msg)));
  list->SetItem(row, 2, wxString::Format("%02x", Pm_MessageData1(msg)));
  list->SetItem(row, 3, wxString::Format("%02x", Pm_MessageData2(msg)));
}

wxWindow *Monitor::make_input_panel(wxPanel *parent) {
  wxPanel *p = new wxPanel(parent, wxID_ANY);
  input_list = new wxListCtrl(p, wxID_ANY, wxDefaultPosition, wxSize(200, 400),
                              wxLC_REPORT);

  wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
  sizer->Add(new wxStaticText(p, -1, "Inputs"), wxSizerFlags().Align(wxALIGN_LEFT));
  sizer->Add(input_list, wxSizerFlags(1).Expand().Border(wxALL));

  p->SetSizerAndFit(sizer);
  return p;
}

wxWindow *Monitor::make_output_panel(wxPanel *parent) {
  wxPanel *p = new wxPanel(parent, wxID_ANY);
  output_list = new wxListCtrl(p, wxID_ANY, wxDefaultPosition, wxSize(200, 400),
                               wxLC_REPORT);

  wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
  sizer->Add(new wxStaticText(p, -1, "Outputs"), wxSizerFlags().Align(wxALIGN_LEFT));
  sizer->Add(output_list, wxSizerFlags(1).Expand().Border(wxALL));

  p->SetSizerAndFit(sizer);
  return p;
}
