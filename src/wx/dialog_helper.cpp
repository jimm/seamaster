#include "dialog_helper.h"

wxSizer *DialogHelper::make_ok_cancel_buttons(wxDialog *parent) {
  return parent->CreateStdDialogButtonSizer(wxOK | wxCANCEL);
  // wxPanel *p = new wxPanel(parent, wxID_ANY);
  // wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
  // sizer->Add(new wxButton(p, wxID_CANCEL, "Cancel"),
  //                   wxSizerFlags().Right().Border());
  // wxButton *save_button = new wxButton(p, wxID_OK, "Save");
  // save_button->SetDefault();
  // sizer->Add(save_button, wxSizerFlags().Right().Border());
  // p->SetSizer(sizer);
  // return p;
}
