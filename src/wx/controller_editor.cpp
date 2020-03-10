#include <wx/persist/toplevel.h>
#include "controller_editor.h"
#include "../connection.h"
#include "../controller.h"
#include "../formatter.h"

#define FRAME_NAME "seamaster_main_frame"

wxBEGIN_EVENT_TABLE(ControllerEditor, wxDialog)
  EVT_BUTTON(ID_CMAP_DoneButton, ControllerEditor::done)
wxEND_EVENT_TABLE()

ControllerEditor::ControllerEditor(wxWindow *parent, Connection *conn, Controller *cc)
  : wxDialog(parent, wxID_ANY, "Controller Editor", wxDefaultPosition, wxSize(480, 500)),
    connection(conn), controller(cc), orig_cc_num(cc->cc_num)
{
  wxPanel *p = new wxPanel(this, wxID_ANY);
  wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

  sizer->Add(make_numbers_panel(p));
  sizer->Add(make_minmax_panel(p));
  sizer->Add(make_filtered_panel(p));

  // TODO better button padding
  sizer->Add(new wxButton(this, ID_CMAP_DoneButton, "Done"),
             wxSizerFlags().Right());

  p->SetSizerAndFit(sizer);
  SetClientSize(p->GetSize());
  Show(true);
  wxPersistentRegisterAndRestore(this, FRAME_NAME); // not working?
}

wxWindow *ControllerEditor::make_numbers_panel(wxPanel *parent) {
  wxPanel *p = new wxPanel(parent, wxID_ANY);
  wxBoxSizer *outer_sizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *field_sizer = new wxBoxSizer(wxHORIZONTAL);

 field_sizer->Add(new wxStaticText(p, wxID_ANY, "CC Number"));
  cb_cc_number =
    make_cc_number_dropdown(p, ID_CMAP_CCNumber, controller->cc_num, false);
  field_sizer->Add(cb_cc_number);

  field_sizer->Add(new wxStaticText(p, wxID_ANY, "Translated CC Number"));
  cb_xlated_number =
    make_cc_number_dropdown(p, ID_CMAP_TranslatedNumber,
                            controller->translated_cc_num, false);
  field_sizer->Add(cb_xlated_number);

  outer_sizer->Add(new wxStaticText(p, wxID_ANY, "Controller Number"));
  outer_sizer->Add(field_sizer);

  p->SetSizerAndFit(outer_sizer);
  return p;
}

wxComboBox *ControllerEditor::make_cc_number_dropdown(
  wxPanel *parent, wxWindowID id, int curr_val, bool filter_out_existing)
{
  wxArrayString choices;
  for (int i = 0; i < 128; ++i)
    if (!filter_out_existing || i == curr_val || connection->cc_maps[i] == nullptr)
      choices.Add(wxString::Format("%d", i));

  wxString curr_choice = wxString::Format("%d", curr_val);
  return new wxComboBox(parent, id, curr_choice,
                        wxDefaultPosition, wxDefaultSize, choices,
                        wxCB_READONLY);
}

wxWindow *ControllerEditor::make_minmax_panel(wxPanel *parent) {
  wxPanel *p = new wxPanel(parent, wxID_ANY);
  wxBoxSizer *outer_sizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *field_sizer = new wxBoxSizer(wxHORIZONTAL);

  field_sizer->Add(new wxStaticText(p, wxID_ANY, "Min"));
  tc_min = new wxTextCtrl(p, ID_CMAP_Min,
                          wxString::Format("%d", controller->min));
  field_sizer->Add(tc_min);

  field_sizer->Add(new wxStaticText(p, wxID_ANY, "Max"));
  tc_max = new wxTextCtrl(p, ID_CMAP_Max,
                          wxString::Format("%d", controller->max));
  field_sizer->Add(tc_max);

  outer_sizer->Add(new wxStaticText(p, wxID_ANY, "Min/Max Values"));
  outer_sizer->Add(field_sizer);

  p->SetSizerAndFit(outer_sizer);
  return p;
}

wxWindow *ControllerEditor::make_filtered_panel(wxPanel *parent) {
  wxPanel *p = new wxPanel(parent, wxID_ANY);
  wxBoxSizer *outer_sizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *field_sizer = new wxBoxSizer(wxHORIZONTAL);

  cb_filtered = new wxCheckBox(p, ID_CMAP_Filtered, "Filter (Block) Controller");
  field_sizer->Add(cb_filtered);

  outer_sizer->Add(new wxStaticText(p, wxID_ANY, "Filter"));
  outer_sizer->Add(field_sizer);

  p->SetSizerAndFit(outer_sizer);
  return p;
}

void ControllerEditor::done(wxCommandEvent& event) {
  controller->cc_num = int_from_chars(cb_cc_number->GetValue());
  controller->translated_cc_num = int_from_chars(cb_xlated_number->GetValue());
  controller->min = int_from_chars(tc_min->GetValue());
  controller->max = int_from_chars(tc_max->GetValue());
  controller->filtered = cb_filtered->IsChecked();

  if (controller->cc_num != orig_cc_num) {
    connection->cc_maps[orig_cc_num] = nullptr;
    connection->set_controller(controller);
  }

  wxCommandEvent e(Connection_Refresh, GetId());
  wxPostEvent(GetParent(), e);
  Close();
}
