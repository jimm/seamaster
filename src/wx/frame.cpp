#include "wx/listctrl.h"
#include "wx/textctrl.h"
#include "wx/gbsizer.h"
#include "frame.h"
#include "patch_list.h"
#include "../patchmaster.h"
#include "../cursor.h"
#include "../loader.h"

#define POS(row, col) wxGBPosition(row, col)
#define SPAN(rowspan, colspan) wxGBSpan(rowspan, colspan)

Frame::Frame(const wxString& title)
  : wxFrame(NULL, wxID_ANY, title)
{
  make_frame_panels();
  make_menu_bar();
}

void Frame::make_frame_panels() {
  wxPanel *p = new wxPanel(this, wxID_ANY);
  wxGridBagSizer * const main_sizer = new wxGridBagSizer();

  main_sizer->Add(make_song_list_panel(p), POS(0, 0), SPAN(3, 1), wxEXPAND);
  main_sizer->Add(make_song_list_list_panel(p), POS(3, 0), SPAN(1, 1), wxEXPAND);
  main_sizer->Add(make_song_panel(p), POS(0, 1), SPAN(3, 1), wxEXPAND);
  main_sizer->Add(make_trigger_panel(p), POS(3, 1), SPAN(1, 1), wxEXPAND);
  main_sizer->Add(make_notes_panel(p), POS(0, 2), SPAN(4, 1), wxEXPAND);
  main_sizer->Add(make_patch_panel(p), POS(4, 0), SPAN(1, 3), wxEXPAND);

  for (int row = 0; row < 5; ++row)
    main_sizer->AddGrowableRow(row);
  for (int col = 0; col < 3; ++col)
    main_sizer->AddGrowableCol(col);

  p->SetSizerAndFit(main_sizer);
  SetClientSize(p->GetSize());
}

wxWindow * Frame::make_song_list_panel(wxPanel *parent) {
  lc_song_list = new wxListCtrl(parent, wxID_ANY, wxDefaultPosition, wxSize(100, 150));
  lc_song_list->InsertItem(0, wxString("Song List"));
  return lc_song_list;
}

wxWindow * Frame::make_song_list_list_panel(wxPanel *parent) {
  lc_song_lists = new wxListCtrl(parent, wxID_ANY, wxDefaultPosition, wxSize(100, 100));
  lc_song_lists->InsertItem(0, wxString("Song Lists"));
  return lc_song_lists;
}

wxWindow * Frame::make_song_panel(wxPanel *parent) {
  lc_song = new wxListCtrl(parent, wxID_ANY, wxDefaultPosition, wxSize(100, 150));
  lc_song->InsertItem(0, wxString("Song"));
  return lc_song;
}

wxWindow * Frame::make_trigger_panel(wxPanel *parent) {
  lc_triggers = new wxListCtrl(parent, wxID_ANY, wxDefaultPosition, wxSize(100, 100));
  lc_triggers->InsertItem(0, wxString("Triggers"));
  return lc_triggers;
}

wxWindow * Frame::make_notes_panel(wxPanel *parent) {
  lc_notes = new wxTextCtrl(parent, wxID_ANY, "", wxDefaultPosition, wxSize(100, 250), wxTE_MULTILINE);
  lc_notes->AppendText("Notes");
  return lc_notes;
}

wxWindow * Frame::make_patch_panel(wxPanel *parent) {
  lc_patch = new PatchList(parent);
  return lc_patch;
}

void Frame::make_menu_bar() {
  wxMenu *menuFile = new wxMenu;
  menuFile->Append(wxID_OPEN);
  menuFile->AppendSeparator();
  menuFile->Append(wxID_EXIT);

  wxMenu *menuView = new wxMenu;
  menuView->Append(ID_ListDevices, "&List MIDI Devices\tCtrl-L",
                   "Displays MIDI devices detected by PortMidi");
  menuView->Append(ID_Monitor, "&MIDI Monitor\tCtrl-M",
                   "Open the MIDI Monitor window");

  wxMenu *menuHelp = new wxMenu;
  menuHelp->Append(wxID_ABOUT);

  wxMenuBar *menuBar = new wxMenuBar;
  menuBar->Append(menuFile, "&File");
  menuBar->Append(menuView, "&View");
  menuBar->Append(menuHelp, "&Help");
  SetMenuBar(menuBar);

  CreateStatusBar();
  SetStatusText("No SeaMaster file loaded");
}

void Frame::OnExit(wxCommandEvent& event) {
  if (PatchMaster_instance() != nullptr)
    PatchMaster_instance()->start();
  Close(true);
  exit(0);
}

void Frame::OnAbout(wxCommandEvent& event) {
  wxMessageBox("This is SeaMaster, the MIDI processing and patching system.",
                "About SeaMaster", wxOK | wxICON_INFORMATION);
}

void Frame::OnOpen(wxCommandEvent& event) {
  wxFileDialog openFileDialog(this, _("Open SeaMaster file"), "", "",
                              "SeaMaster files (*.org;*.md)|*.org;*.md",
                              wxFD_OPEN|wxFD_FILE_MUST_EXIST);
  if (openFileDialog.ShowModal() == wxID_CANCEL)
    return;

  PatchMaster *old_pm = PatchMaster_instance();
  bool testing = old_pm != nullptr && old_pm->testing;
  Loader loader;
  PatchMaster *pm = loader.load(openFileDialog.GetPath(), testing);
  if (loader.has_error()) {
    wxLogError("Cannot open file '%s': %s.", openFileDialog.GetPath(),
               loader.error());
    return;
  }

  SetStatusText(wxString::Format("Loaded %s", openFileDialog.GetPath()));
  if (old_pm != nullptr) {
    old_pm->stop();
    delete old_pm;
  }
  pm->start();                  // initializes cursor
  load_data_into_windows();     // must come after start
}

void Frame::OnListDevices(wxCommandEvent& event) {
  wxMessageBox("List MIDI Devices not yet implemented.", "MIDI Devices",
               wxOK | wxICON_WARNING);
}

void Frame::OnMonitor(wxCommandEvent &event) {
  wxMessageBox("MIDI Monitor not yet implemented.", "MIDI Monitor",
               wxOK | wxICON_WARNING);
  // TODO open monitor frame if it's not already open
  // if already exists return
  // MonitorFrame *frame = new MonitorFrame(PatchMaster_instance(), wxPoint(50, 50), wxSize(450, 340));
  // frame->Show(true);
}

void Frame::load_data_into_windows() {
  PatchMaster *pm = PatchMaster_instance();
  Cursor *cursor = pm->cursor;
  int i;

  SongList *song_list = cursor->song_list();
  lc_song_list->ClearAll();
  if (song_list != nullptr) {
    i = 0;
    for (auto& song : song_list->songs) {
      lc_song_list->InsertItem(i, song->name.c_str());
      if (song == cursor->song())
        lc_song_list->SetItemState(i, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
      ++i;
    }
  }

  lc_song_lists->ClearAll();
  i = 0;
  for (auto& song_list : pm->song_lists) {
    lc_song_lists->InsertItem(i, song_list->name.c_str());
    if (song_list == cursor->song_list())
        lc_song_lists->SetItemState(i, wxLIST_STATE_SELECTED,
                                    wxLIST_STATE_SELECTED);
    ++i;
  }

  Song *song = cursor->song();
  lc_song->ClearAll();
  lc_notes->Clear();
  if (song != nullptr) {
    i = 0;
    for (auto& patch : song->patches) {
      lc_song->InsertItem(i, patch->name.c_str());
      if (patch == cursor->patch())
        lc_song->SetItemState(i, wxLIST_STATE_SELECTED,
                              wxLIST_STATE_SELECTED);
      ++i;
    }

    i = 0;
    for (auto& line : song->notes) {
      lc_notes->AppendText(line);
      lc_notes->AppendText("\n");
    }
  }

  lc_triggers->ClearAll();
  i = 0;
  for (auto& input : pm->inputs)
    for (auto& trigger : input->triggers)
      lc_triggers->InsertItem(i++, "TODO --- display trigger");

  lc_patch->set_patch(cursor->patch());
}
