#include <wx/persist/toplevel.h>
#include <wx/defs.h>
#include <wx/listctrl.h>
#include <wx/textctrl.h>
#include <wx/gbsizer.h>
#include "frame.h"
#include "set_list_box.h"
#include "set_list_list_box.h"
#include "song_box.h"
#include "patch_list.h"
#include "trigger_list.h"
#include "instrument_dialog.h"
#include "monitor.h"
#include "../patchmaster.h"
#include "../cursor.h"
#include "../loader.h"

#define POS(row, col) wxGBPosition(row, col)
#define SPAN(rowspan, colspan) wxGBSpan(rowspan, colspan)

#define FRAME_POS_X 20
#define FRAME_POS_Y 40
#define LIST_WIDTH 200
#define TALL_LIST_HEIGHT 300
#define SHORT_LIST_HEIGHT 200
#define NOTES_WIDTH 200
#define FRAME_NAME "seamaster_main_frame"

void *frame_clear_message_thread(void *gui_vptr) {
  Frame *gui = (Frame *)gui_vptr;
  int clear_message_id = gui->clear_message_id();

  sleep(gui->clear_message_seconds());

  // Only clear the window if the id hasn't changed
  if (gui->clear_message_id() == clear_message_id)
    gui->clear_message();
  return nullptr;
}

Frame::Frame(const wxString& title)
  : wxFrame(NULL, wxID_ANY, title, wxPoint(FRAME_POS_X, FRAME_POS_Y))
{
  make_frame_panels();
  make_menu_bar();
  CreateStatusBar();
  SetStatusText("No SeaMaster file loaded");
  wxPersistentRegisterAndRestore(this, FRAME_NAME); // not working?
}

void Frame::make_frame_panels() {
  wxPanel *p = new wxPanel(this, wxID_ANY);
  wxGridBagSizer * const main_sizer = new wxGridBagSizer();

  main_sizer->Add(make_set_list_panel(p), POS(0, 0), SPAN(3, 1), wxEXPAND);
  main_sizer->Add(make_set_list_list_panel(p), POS(3, 0), SPAN(1, 1), wxEXPAND);
  main_sizer->Add(make_song_panel(p), POS(0, 1), SPAN(3, 1), wxEXPAND);
  main_sizer->Add(make_trigger_panel(p), POS(3, 1), SPAN(1, 2), wxEXPAND);
  main_sizer->Add(make_notes_panel(p), POS(0, 2), SPAN(3, 1), wxEXPAND);
  main_sizer->Add(make_patch_panel(p), POS(4, 0), SPAN(1, 3), wxEXPAND);

  for (int row = 0; row < 5; ++row)
    main_sizer->AddGrowableRow(row);
  for (int col = 0; col < 3; ++col)
    main_sizer->AddGrowableCol(col);

  p->SetSizerAndFit(main_sizer);
  SetClientSize(p->GetSize());
}

wxWindow * Frame::make_set_list_panel(wxPanel *parent) {
  wxPanel *p = new wxPanel(parent, wxID_ANY);
  lc_set_list = new SetListBox(p, ID_JumpToSong,
                                 wxSize(LIST_WIDTH, TALL_LIST_HEIGHT));

  wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
  sizer->Add(new wxStaticText(p, -1, "Songs"), wxSizerFlags().Align(wxALIGN_LEFT));
  sizer->Add(lc_set_list, wxSizerFlags(1).Expand().Border(wxALL));

  p->SetSizerAndFit(sizer);
  return p;
}

wxWindow * Frame::make_set_list_list_panel(wxPanel *parent) {
  wxPanel *p = new wxPanel(parent, wxID_ANY);
  lc_set_lists = new SetListListBox(p, ID_JumpToSetList,
                                      wxSize(LIST_WIDTH, SHORT_LIST_HEIGHT));

  wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
  sizer->Add(new wxStaticText(p, -1, "Set Lists"), wxSizerFlags().Align(wxALIGN_LEFT));
  sizer->Add(lc_set_lists, wxSizerFlags(1).Expand().Border(wxALL));

  p->SetSizerAndFit(sizer);
  return p;
}

wxWindow * Frame::make_song_panel(wxPanel *parent) {
  wxPanel *p = new wxPanel(parent, wxID_ANY);
  lc_song = new SongBox(p, ID_JumpToPatch,
                        wxSize(LIST_WIDTH, TALL_LIST_HEIGHT));

  wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
  sizer->Add(new wxStaticText(p, -1, "Patches"), wxSizerFlags().Align(wxALIGN_LEFT));
  sizer->Add(lc_song, wxSizerFlags(1).Expand().Border(wxALL));

  p->SetSizerAndFit(sizer);
  return p;
}

wxWindow * Frame::make_trigger_panel(wxPanel *parent) {
  wxPanel *p = new wxPanel(parent, wxID_ANY);
  lc_triggers = new TriggerList(p);

  wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
  sizer->Add(new wxStaticText(p, -1, "Triggers"), wxSizerFlags().Align(wxALIGN_LEFT));
  sizer->Add(lc_triggers, wxSizerFlags(1).Expand().Border(wxALL));

  p->SetSizerAndFit(sizer);
  return p;
}

wxWindow * Frame::make_notes_panel(wxPanel *parent) {
  wxPanel *p = new wxPanel(parent, wxID_ANY);
  lc_notes = new wxTextCtrl(p, wxID_ANY, "", wxDefaultPosition,
                            wxSize(NOTES_WIDTH, TALL_LIST_HEIGHT), wxTE_MULTILINE);

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

  wxMenu *menuGo = new wxMenu;
  menuGo->Append(ID_GoNextSong, "Next Song\tN", "Move to the next song");
  menuGo->Append(ID_GoPrevSong, "Prev Song\tP", "Move to the previous song");
  menuGo->Append(ID_GoNextPatch, "Next Patch\tJ", "Move to the next patch");
  menuGo->Append(ID_GoPrevPatch, "Prev Patch\tK", "Move to the previous patch");
  menuGo->AppendSeparator();
  menuGo->Append(ID_FindSong, "Find Song...\tCtrl-F", "Find song by name");
  menuGo->Append(ID_FindSetList, "Find Set List...\tCtrl-T", "Find set list by name");

  wxMenu *menuView = new wxMenu;
  menuView->Append(ID_ListInstruments, "&List Instruments\tCtrl-I",
                   "Displays input and output instruments");
  menuView->Append(ID_ListDevices, "&List MIDI Devices\tCtrl-L",
                   "Displays MIDI devices detected by PortMidi");
  menuView->Append(ID_Monitor, "&MIDI Monitor\tCtrl-M",
                   "Open the MIDI Monitor window");

  wxMenu *menuMIDI = new wxMenu;
  menuMIDI->Append(ID_RegularPanic, "&Send All Notes Off\t\e",
                   "Send All Notes Off controller message on all channels");
  menuMIDI->Append(ID_SuperPanic, "&Send Super-Panic\t.",
                   "Send Notes Off messages, all notes, all channels");

  wxMenu *menuHelp = new wxMenu;
  menuHelp->Append(wxID_ABOUT);

  wxMenuBar *menuBar = new wxMenuBar;
  menuBar->Append(menuFile, "&File");
  menuBar->Append(menuGo, "&Go");
  menuBar->Append(menuView, "&View");
  menuBar->Append(menuMIDI, "&MIDI");
  menuBar->Append(menuHelp, "&Help");
  SetMenuBar(menuBar);
#if defined(__WXMAC__)
  menuBar->OSXGetAppleMenu()->SetTitle("SeaMaster");
#endif
}

void Frame::OnExit(wxCommandEvent &_event) {
  if (PatchMaster_instance() != nullptr)
    PatchMaster_instance()->start();
  Close(true);
  exit(0);
}

void Frame::show_message(string msg) {
  SetStatusText(msg.c_str());
}

void Frame::show_message(string msg, int secs) {
  SetStatusText(msg.c_str());
  clear_message_after(secs);
}

void Frame::clear_message() {
  SetStatusText("");
}

void Frame::clear_message_after(int secs) {
  clear_msg_secs = secs;
  clear_msg_id++;

  pthread_t pthread;
  pthread_create(&pthread, 0, frame_clear_message_thread, this);
}

void Frame::next_song() {
  PatchMaster *pm = PatchMaster_instance();
  if (pm == nullptr)
    return;
  pm->next_song();
  load_data_into_windows();
}

void Frame::prev_song() {
  PatchMaster *pm = PatchMaster_instance();
  if (pm == nullptr)
    return;
  pm->prev_song();
  load_data_into_windows();
}

void Frame::next_patch() {
  PatchMaster *pm = PatchMaster_instance();
  if (pm == nullptr)
    return;
  pm->next_patch();
  load_data_into_windows();
}

void Frame::prev_patch() {
  PatchMaster *pm = PatchMaster_instance();
  if (pm == nullptr)
    return;
  pm->prev_patch();
  load_data_into_windows();
}

void Frame::find_set_list() {
  PatchMaster *pm = PatchMaster_instance();
  if (pm == nullptr)
    return;
  wxTextEntryDialog prompt(this, "Find Set List");
  if (prompt.ShowModal() == wxID_OK) {
    wxString str = prompt.GetValue();
    if (!str.IsEmpty()) {
      pm->goto_set_list(str.ToStdString());
      load_data_into_windows();
    }
  }
}

void Frame::find_song() {
  PatchMaster *pm = PatchMaster_instance();
  if (pm == nullptr)
    return;
  wxTextEntryDialog prompt(this, "Find Song");
  if (prompt.ShowModal() == wxID_OK) {
    wxString str = prompt.GetValue();
    if (!str.IsEmpty()) {
      pm->goto_song(str.ToStdString());
      load_data_into_windows();
    }
  }
}

void Frame::jump_to_set_list(wxCommandEvent &event) {
  if (event.GetEventType() == wxEVT_LISTBOX && event.IsSelection()) {
    lc_set_lists->jump();
    load_data_into_windows();
  }
}

void Frame::jump_to_song(wxCommandEvent &event) {
  if (event.GetEventType() == wxEVT_LISTBOX && event.IsSelection()) {
    lc_set_list->jump();
    load_data_into_windows();
  }
}

void Frame::jump_to_patch(wxCommandEvent &event) {
  if (event.GetEventType() == wxEVT_LISTBOX && event.IsSelection()) {
    lc_song->jump();
    load_data_into_windows();
  }
}

void Frame::regular_panic(wxCommandEvent &_event) {
  PatchMaster *pm = PatchMaster_instance();
  if (pm == nullptr)
    return;
  show_message("Sending panic...");
  pm->panic(false);
  show_message("Panic sent", 5);
}

void Frame::super_panic(wxCommandEvent &_event) {
  PatchMaster *pm = PatchMaster_instance();
  if (pm == nullptr)
    return;
  show_message("Sending \"super panic\": all notes off, all channels...");
  pm->panic(true);
  show_message("Panic sent (all notes off, all channels)", 5);
}

void Frame::OnAbout(wxCommandEvent &_event) {
  wxMessageBox("This is SeaMaster, the MIDI processing and patching system.",
                "About SeaMaster", wxOK | wxICON_INFORMATION);
}

void Frame::OnOpen(wxCommandEvent &_event) {
  wxFileDialog openFileDialog(this, _("Open SeaMaster file"), "", "",
                              "SeaMaster files (*.org;*.md)|*.org;*.md",
                              wxFD_OPEN|wxFD_FILE_MUST_EXIST);
  if (openFileDialog.ShowModal() != wxID_CANCEL)
    load(openFileDialog.GetPath());
}

void Frame::OnListInstruments(wxCommandEvent &_event) {
  InstrumentDialog(this, PatchMaster_instance()).run();
}

void Frame::OnListDevices(wxCommandEvent &_event) {
  wxMessageBox("List MIDI Devices not yet implemented.", "MIDI Devices",
               wxOK | wxICON_WARNING);
}

void Frame::OnMonitor(wxCommandEvent &event) {
  PatchMaster *pm = PatchMaster_instance();
  if (pm == nullptr)
    return;
  (new Monitor())->Show(true);
}

void Frame::initialize() {
  PatchMaster *pm = new PatchMaster();
  pm->initialize();
  pm->start();
  load_data_into_windows();
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
  if (pm == nullptr)
    return;
  Cursor *cursor = pm->cursor;
  int i;

  lc_set_list->update();
  lc_set_lists->update();
  lc_song->update();
  lc_patch->update();
  lc_triggers->update();

  Song *song = cursor->song();
  lc_notes->Clear();
  if (song != nullptr) {
    i = 0;
    for (auto& line : song->notes) {
      lc_notes->AppendText(line);
      lc_notes->AppendText("\n");
    }
  }
}
