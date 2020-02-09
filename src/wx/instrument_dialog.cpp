#include <wx/listctrl.h>
#include "instrument_dialog.h"
#include "../patchmaster.h"

const char * const COLUMN_HEADERS[] = {
  "Symbol", "Name", "MIDI Port", "Status"
};

InstrumentDialog::InstrumentDialog(wxWindow *parent, PatchMaster *patchmaster)
  : wxDialog(parent, wxID_ANY, "Instruments"), pm(patchmaster)
{     
  wxListCtrl *inputs = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(600, 150), wxLC_REPORT);
  wxListCtrl *outputs = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(600, 150), wxLC_REPORT);

  for (int i = 0; i < sizeof(COLUMN_HEADERS) / sizeof(const char * const); ++i) {
    inputs->InsertColumn(i, COLUMN_HEADERS[i]);
    outputs->InsertColumn(i, COLUMN_HEADERS[i]);
  }

  int i = 0;
  for (auto* inst : pm->inputs)
    add_instrument(inputs, inst, i++);
  i = 0;
  for (auto* inst : pm->outputs)
    add_instrument(outputs, inst, i++);

  wxSizer *buttons = CreateButtonSizer(wxOK);

  wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
  sizer->Add(new wxStaticText(this, -1, "Inputs"), wxSizerFlags().Align(wxALIGN_LEFT));
  sizer->Add(inputs, wxSizerFlags(1).Expand());
  sizer->Add(new wxStaticText(this, -1, "Outputs"), wxSizerFlags().Align(wxALIGN_LEFT));
  sizer->Add(outputs, wxSizerFlags(1).Expand());
  sizer->Add(buttons, wxSizerFlags().Expand().DoubleBorder());

  SetSizerAndFit(sizer);
}

void InstrumentDialog::run() {
  ShowModal();
  Destroy();
}

void InstrumentDialog::add_instrument(wxListCtrl *list_box, Instrument *inst, int i) {
  list_box->InsertItem(i, inst->sym.c_str());
  list_box->SetItem(i, 1, inst->name.c_str());
  list_box->SetItem(i, 2, inst->port_name.c_str());
  list_box->SetItem(i, 3, inst->enabled ? "enabled" : "<disabled>");
}
