#include "wx/defs.h"
#include "wx/listctrl.h"
#include "wx/textctrl.h"
#include "wx/gbsizer.h"
#include "frame.h"
#include "song_list_box.h"
#include "song_list_list_box.h"
#include "song_box.h"
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
  CreateStatusBar();
  SetStatusText("No SeaMaster file loaded");
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
  wxPanel *p = new wxPanel(parent, wxID_ANY);
  lc_song_list = new SongListBox(p, ID_JumpToSong, wxSize(100, 150));

  wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
  sizer->Add(new wxStaticText(p, -1, "Songs"), wxSizerFlags().Align(wxALIGN_LEFT));
  sizer->Add(lc_song_list, wxSizerFlags(1).Expand().Border(wxALL));

  p->SetSizerAndFit(sizer);
  return p;
}

wxWindow * Frame::make_song_list_list_panel(wxPanel *parent) {
  wxPanel *p = new wxPanel(parent, wxID_ANY);
  lc_song_lists = new SongListListBox(p, ID_JumpToSongList, wxSize(100, 100));

  wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
  sizer->Add(new wxStaticText(p, -1, "Song Lists"), wxSizerFlags().Align(wxALIGN_LEFT));
  sizer->Add(lc_song_lists, wxSizerFlags(1).Expand().Border(wxALL));

  p->SetSizerAndFit(sizer);
  return p;
}

wxWindow * Frame::make_song_panel(wxPanel *parent) {
  wxPanel *p = new wxPanel(parent, wxID_ANY);
  lc_song = new SongBox(p, ID_JumpToPatch, wxSize(100, 150));

  wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
  sizer->Add(new wxStaticText(p, -1, "Patches"), wxSizerFlags().Align(wxALIGN_LEFT));
  sizer->Add(lc_song, wxSizerFlags(1).Expand().Border(wxALL));

  p->SetSizerAndFit(sizer);
  return p;
}

wxWindow * Frame::make_trigger_panel(wxPanel *parent) {
  wxPanel *p = new wxPanel(parent, wxID_ANY);
  lc_triggers = new wxListCtrl(p, wxID_ANY, wxDefaultPosition, wxSize(100, 100));

  wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
  sizer->Add(new wxStaticText(p, -1, "Triggers"), wxSizerFlags().Align(wxALIGN_LEFT));
  sizer->Add(lc_triggers, wxSizerFlags(1).Expand().Border(wxALL));

  p->SetSizerAndFit(sizer);
  return p;
}

wxWindow * Frame::make_notes_panel(wxPanel *parent) {
  wxPanel *p = new wxPanel(parent, wxID_ANY);
  lc_notes = new wxTextCtrl(p, wxID_ANY, "", wxDefaultPosition, wxSize(100, 250), wxTE_MULTILINE);

  wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
  sizer->Add(new wxStaticText(p, -1, "Notes"), wxSizerFlags().Align(wxALIGN_LEFT));
  sizer->Add(lc_notes, wxSizerFlags(1).Expand().Border(wxALL));

  p->SetSizerAndFit(sizer);
  return p;
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
#if defined(__WXMAC__)
  menuBar->OSXGetAppleMenu()->SetTitle("SeaMaster");
#endif
}

void Frame::OnExit(wxCommandEvent& event) {
  if (PatchMaster_instance() != nullptr)
    PatchMaster_instance()->start();
  Close(true);
  exit(0);
}

void Frame::jump_to_song_list(wxCommandEvent& event) {
  if (event.GetEventType() == wxEVT_LISTBOX && event.IsSelection()) {
    lc_song_lists->jump();
    load_data_into_windows();
  }
}

void Frame::jump_to_song(wxCommandEvent& event) {
  if (event.GetEventType() == wxEVT_LISTBOX && event.IsSelection()) {
    lc_song_list->jump();
    load_data_into_windows();
  }
}

void Frame::jump_to_patch(wxCommandEvent& event) {
  if (event.GetEventType() == wxEVT_LISTBOX && event.IsSelection()) {
    lc_song->jump();
    load_data_into_windows();
  }
}

void Frame::OnAbout(wxCommandEvent& event) {
  wxMessageBox("This is SeaMaster, the MIDI processing and patching system.",
                "About SeaMaster", wxOK | wxICON_INFORMATION);
}

void Frame::OnOpen(wxCommandEvent& event) {
  wxFileDialog openFileDialog(this, _("Open SeaMaster file"), "", "",
                              "SeaMaster files (*.org;*.md)|*.org;*.md",
                              wxFD_OPEN|wxFD_FILE_MUST_EXIST);
  if (openFileDialog.ShowModal() != wxID_CANCEL)
    load(openFileDialog.GetPath());
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

void Frame::load(wxString path) {
  PatchMaster *old_pm = PatchMaster_instance();
  bool testing = old_pm != nullptr && old_pm->testing;
  Loader loader;
  PatchMaster *pm = loader.load(path, testing);
  if (loader.has_error()) {
    wxLogError("Cannot open file '%s': %s.", path, loader.error());
    return;
  }

  SetStatusText(wxString::Format("Loaded %s", path));
  if (old_pm != nullptr) {
    old_pm->stop();
    delete old_pm;
  }
  pm->start();                  // initializes cursor
  load_data_into_windows();     // must come after start
}

void Frame::load_data_into_windows() {
  PatchMaster *pm = PatchMaster_instance();
  Cursor *cursor = pm->cursor;
  int i;

  lc_song_list->update();
  lc_song_lists->update();
  lc_song->update();
  lc_patch->update();

  Song *song = cursor->song();
  lc_notes->Clear();
  if (song != nullptr) {
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

}
