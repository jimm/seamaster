#include <wx/persist/toplevel.h>
#include <wx/defs.h>
#include <wx/listctrl.h>
#include <wx/textctrl.h>
#include <wx/gbsizer.h>
#include "frame.h"
#include "set_list_box.h"
#include "set_list_list_box.h"
#include "song_box.h"
#include "patch_connections.h"
#include "message_list.h"
#include "trigger_list.h"
#include "instrument_dialog.h"
#include "monitor.h"
#include "set_list_editor.h"
#include "../patchmaster.h"
#include "../cursor.h"
#include "../loader.h"
#include "../editor.h"

#define POS(row, col) wxGBPosition(row, col)
#define SPAN(rowspan, colspan) wxGBSpan(rowspan, colspan)

#define FRAME_POS_X 20
#define FRAME_POS_Y 40
#define LIST_WIDTH 200
#define TALL_LIST_HEIGHT 300
#define SHORT_LIST_HEIGHT 200
#define NOTES_WIDTH 200
#define FRAME_NAME "seamaster_main_frame"

void *frame_clear_user_message_thread(void *gui_vptr) {
  Frame *gui = (Frame *)gui_vptr;
  int clear_user_message_id = gui->clear_user_message_id();

  sleep(gui->clear_user_message_seconds());

  // Only clear the window if the id hasn't changed
  if (gui->clear_user_message_id() == clear_user_message_id)
    gui->clear_user_message();
  return nullptr;
}

Frame::Frame(const wxString& title)
  : wxFrame(NULL, wxID_ANY, title, wxPoint(FRAME_POS_X, FRAME_POS_Y))
{
  make_frame_panels();
  make_menu_bar();
  CreateStatusBar();
  show_user_message("No SeaMaster file loaded", 15);
  wxPersistentRegisterAndRestore(this, FRAME_NAME); // not working?
}

void Frame::make_frame_panels() {
  wxPanel *p = new wxPanel(this, wxID_ANY);
  wxGridBagSizer * const main_sizer = new wxGridBagSizer();

  main_sizer->Add(make_set_list_panel(p), POS(0, 0), SPAN(3, 1), wxEXPAND);
  main_sizer->Add(make_song_panel(p), POS(0, 1), SPAN(3, 1), wxEXPAND);
  main_sizer->Add(make_notes_panel(p), POS(0, 2), SPAN(3, 1), wxEXPAND);

  main_sizer->Add(make_patch_panel(p), POS(3, 0), SPAN(1, 3), wxEXPAND);

  main_sizer->Add(make_set_list_list_panel(p), POS(4, 0), SPAN(1, 1), wxEXPAND);
  main_sizer->Add(make_message_panel(p), POS(4, 1), SPAN(1, 1), wxEXPAND);
  main_sizer->Add(make_trigger_panel(p), POS(4, 2), SPAN(1, 1), wxEXPAND);

  for (int row = 0; row < 5; ++row)
    main_sizer->AddGrowableRow(row);
  for (int col = 0; col < 3; ++col)
    main_sizer->AddGrowableCol(col);

  p->SetSizerAndFit(main_sizer);
  SetClientSize(p->GetSize());
}

wxWindow * Frame::make_set_list_panel(wxPanel *parent) {
  wxPanel *p = new wxPanel(parent, wxID_ANY);
  lc_set_list = new SetListBox(p, ID_SetListSongs,
                               wxSize(LIST_WIDTH, TALL_LIST_HEIGHT));

  wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
  sizer->Add(new wxStaticText(p, -1, "Songs"), wxSizerFlags().Align(wxALIGN_LEFT));
  sizer->Add(lc_set_list, wxSizerFlags(1).Expand().Border(wxALL));

  p->SetSizerAndFit(sizer);
  return p;
}

wxWindow * Frame::make_set_list_list_panel(wxPanel *parent) {
  wxPanel *p = new wxPanel(parent, wxID_ANY);
  lc_set_lists = new SetListListBox(p, ID_SetListList,
                                      wxSize(LIST_WIDTH, SHORT_LIST_HEIGHT));

  wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
  sizer->Add(new wxStaticText(p, -1, "Set Lists"), wxSizerFlags().Align(wxALIGN_LEFT));
  sizer->Add(lc_set_lists, wxSizerFlags(1).Expand().Border(wxALL));

  p->SetSizerAndFit(sizer);
  return p;
}

wxWindow * Frame::make_song_panel(wxPanel *parent) {
  wxPanel *p = new wxPanel(parent, wxID_ANY);
  lc_song_patches = new SongBox(p, ID_SongPatches,
                                wxSize(LIST_WIDTH, TALL_LIST_HEIGHT));

  wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
  sizer->Add(new wxStaticText(p, -1, "Patches"), wxSizerFlags().Align(wxALIGN_LEFT));
  sizer->Add(lc_song_patches, wxSizerFlags(1).Expand().Border(wxALL));

  p->SetSizerAndFit(sizer);
  return p;
}

wxWindow * Frame::make_message_panel(wxPanel *parent) {
  wxPanel *p = new wxPanel(parent, wxID_ANY);
  lc_messages = new MessageList(p, ID_MessageList,
                                wxSize(LIST_WIDTH, SHORT_LIST_HEIGHT));

  wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
  sizer->Add(new wxStaticText(p, -1, "Messages"), wxSizerFlags().Align(wxALIGN_LEFT));
  sizer->Add(lc_messages, wxSizerFlags(1).Expand().Border(wxALL));

  p->SetSizerAndFit(sizer);
  return p;
}

wxWindow * Frame::make_trigger_panel(wxPanel *parent) {
  wxPanel *p = new wxPanel(parent, wxID_ANY);
  lc_triggers = new TriggerList(p, ID_TriggerList);

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
  lc_patch_conns = new PatchConnections(parent, ID_PatchConnections);
  return lc_patch_conns;
}

void Frame::make_menu_bar() {
  wxMenu *menuFile = new wxMenu;
  menuFile->Append(wxID_OPEN);
  menuFile->AppendSeparator();
  menuFile->Append(wxID_EXIT);

  wxMenu *menuEdit = new wxMenu;
  menuEdit->Append(ID_CreateMessage, "New Message\tCtrl-Shift-M", "Create a new message");
  menuEdit->Append(ID_CreateTrigger, "New Trigger\tCtrl-Shift-T", "Create a new trigger");
  menuEdit->Append(ID_CreateSong, "New Song\tCtrl-Shift-S", "Create a new song");
  menuEdit->Append(ID_CreatePatch, "New Patch\tCtrl-Shift-P", "Create a new patch");
  menuEdit->Append(ID_CreateConnection, "New Connection\tCtrl-Shift-C", "Create a new connection");
  menuEdit->Append(ID_CreateSetList, "New Set List\tCtrl-Shift-L", "Create a new set list");
  menuEdit->AppendSeparator();
  menuEdit->Append(ID_DestroyMessage, "Delete Message\tCtrl-Alt-M", "Delete the current message");
  menuEdit->Append(ID_DestroyTrigger, "Delete Trigger\tCtrl-Alt-T", "Delete the current trigger");
  menuEdit->Append(ID_DestroySong, "Delete Song\tCtrl-Alt-S", "Delete the current song");
  menuEdit->Append(ID_DestroyPatch, "Delete Patch\tCtrl-Alt-P", "Delete the current patch");
  menuEdit->Append(ID_DestroyConnection, "Delete Connection\tCtrl-Alt-C", "Delete the current connection");
  menuEdit->Append(ID_DestroySetList, "Delete Set List\tCtrl-Alt-L", "Delete the current set list");

  wxMenu *menuGo = new wxMenu;
  menuGo->Append(ID_GoNextSong, "Next Song\tN", "Move to the next song");
  menuGo->Append(ID_GoPrevSong, "Prev Song\tP", "Move to the previous song");
  menuGo->Append(ID_GoNextPatch, "Next Patch\tJ", "Move to the next patch");
  menuGo->Append(ID_GoPrevPatch, "Prev Patch\tK", "Move to the previous patch");
  menuGo->AppendSeparator();
  menuGo->Append(ID_FindSong, "Find Song...\tCtrl-F", "Find song by name");
  menuGo->Append(ID_FindSetList, "Find Set List...\tCtrl-T", "Find set list by name");

  wxMenu *menuWindows = new wxMenu;
  menuWindows->Append(ID_ListInstruments, "&Instruments\tCtrl-I",
                      "Displays input and output instruments");
  menuWindows->Append(ID_Monitor, "MIDI &Monitor\tCtrl-M",
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
  menuBar->Append(menuEdit, "&Edit");
  menuBar->Append(menuGo, "&Go");
  menuBar->Append(menuMIDI, "&MIDI");
  menuBar->Append(menuWindows, "&Windows");
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

// ================ messaging ================

void Frame::show_user_message(string msg) {
  SetStatusText(msg.c_str());
}

void Frame::show_user_message(string msg, int secs) {
  SetStatusText(msg.c_str());
  clear_user_message_after(secs);
}

void Frame::clear_user_message() {
  SetStatusText("");
}

void Frame::clear_user_message_after(int secs) {
  clear_msg_secs = secs;
  clear_msg_id++;

  pthread_t pthread;
  pthread_create(&pthread, 0, frame_clear_user_message_thread, this);
}

// ================ movement ================

void Frame::next_song() {
  PatchMaster *pm = PatchMaster_instance();
  pm->next_song();
  load_data_into_windows();
}

void Frame::prev_song() {
  PatchMaster *pm = PatchMaster_instance();
  pm->prev_song();
  load_data_into_windows();
}

void Frame::next_patch() {
  PatchMaster *pm = PatchMaster_instance();
  pm->next_patch();
  load_data_into_windows();
}

void Frame::prev_patch() {
  PatchMaster *pm = PatchMaster_instance();
  pm->prev_patch();
  load_data_into_windows();
}

void Frame::find_set_list() {
  PatchMaster *pm = PatchMaster_instance();
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
    lc_song_patches->jump();
    load_data_into_windows();
  }
}

// ================ messages ================

void Frame::send_message(wxCommandEvent& event) {
  PatchMaster *pm = PatchMaster_instance();
  int message_num = lc_messages->GetSelection();
  Message *message = pm->messages[message_num];
  for (auto& output : pm->outputs)
    message->send(*output);
}

// ================ create, edit, destroy ================

void Frame::create_message(wxCommandEvent& event) {
  Editor e;
  Message *message = e.create_message();
  load_data_into_windows();
  edit_message(message);
}

void Frame::create_trigger(wxCommandEvent& event) {
  Editor e;
  PatchMaster *pm = PatchMaster_instance();
  Trigger *trigger = e.create_trigger(pm->inputs.front());
  load_data_into_windows();
  edit_trigger(trigger);
}

void Frame::create_song(wxCommandEvent& event) {
  Editor e;
  Song *song = e.create_song();
  load_data_into_windows();
  edit_song(song);
}

void Frame::create_patch(wxCommandEvent& event) {
  Editor e;
  Patch *patch = e.create_patch();
  load_data_into_windows();
  edit_patch(patch);
}

void Frame::create_connection(wxCommandEvent& event) {
  Editor e;
  PatchMaster *pm = PatchMaster_instance();
  if (pm->inputs.empty() || pm->outputs.empty()) {
    wxMessageBox("There must be at least one input and one\noutput to create a connection",
                "New Connection", wxOK | wxICON_INFORMATION);
    return;
  }
  Connection *conn = e.create_connection(pm->inputs.front(), pm->outputs.front());
  load_data_into_windows();
  edit_connection(conn);
}

void Frame::create_set_list(wxCommandEvent& event) {
  Editor e;
  SetList *set_list = e.create_set_list();
  load_data_into_windows();
  edit_set_list(set_list);
}

void Frame::edit_message(wxCommandEvent& event) {
  fprintf(stderr, "TODO edit_message (command)\n"); // DEBUG
}

void Frame::edit_message(Message *message) {
  fprintf(stderr, "TODO edit_message (message)\n"); // DEBUG
}

void Frame::edit_trigger(wxListEvent& event) {
  fprintf(stderr, "TODO edit_trigger (list event)\n"); // DEBUG
}

void Frame::edit_trigger(Trigger *trigger) {
  fprintf(stderr, "TODO edit_trigger (trigger)\n"); // DEBUG
}

void Frame::edit_set_list(wxCommandEvent& event) {
  PatchMaster *pm = PatchMaster_instance();
  edit_set_list(pm->cursor->set_list());
}

void Frame::edit_set_list(SetList *set_list) {
  if (set_list == nullptr)
    return;

  PatchMaster *pm = PatchMaster_instance();
  if (set_list == pm->all_songs) {
    wxMessageBox("Can't edit the master list of all songs",
                "Set List Editor", wxOK | wxICON_INFORMATION);
    return;
  }
  new SetListEditor(this, set_list);
}

void Frame::edit_song(wxCommandEvent& event) {
  PatchMaster *pm = PatchMaster_instance();
  edit_song(pm->cursor->song());
}

void Frame::edit_song(Song *song) {
  if (song == nullptr)
    return;

  wxTextEntryDialog prompt(this, "Song Name");
  if (prompt.ShowModal() == wxID_OK) {
    wxString str = prompt.GetValue();
    if (!str.IsEmpty()) {
      song->name = str.ToStdString();
      load_data_into_windows();
    }
  }
}

void Frame::edit_patch(wxCommandEvent& event) {
  PatchMaster *pm = PatchMaster_instance();
  edit_patch(pm->cursor->patch());
}

void Frame::edit_patch(Patch *patch) {
  if (patch == nullptr)
    return;

  wxTextEntryDialog prompt(this, "Patch Name");
  if (prompt.ShowModal() == wxID_OK) {
    wxString str = prompt.GetValue();
    if (!str.IsEmpty()) {
      patch->name = str.ToStdString();
      load_data_into_windows();
    }
  }
}

void Frame::edit_connection(wxListEvent& event) {
  fprintf(stderr, "TODO edit_connection (list event)\n"); // DEBUG
}

void Frame::edit_connection(Connection *conn) {
  fprintf(stderr, "TODO edit_connection (connection)\n"); // DEBUG
}

void Frame::destroy_message(wxCommandEvent& event) {
  PatchMaster *pm = PatchMaster_instance();
  if (pm->messages.empty())
    return;

  Editor e;
  int message_num = lc_messages->GetSelection();
  Message *message = pm->messages[message_num];
  e.destroy_message(message);
  load_data_into_windows();
}

void Frame::destroy_trigger(wxCommandEvent& event) {
  long item_index = wxNOT_FOUND;
  while ((item_index = lc_triggers->GetNextItem(item_index,
          wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED)) == wxNOT_FOUND)
    ;
  if (item_index == wxNOT_FOUND)
    return;

  int trigger_num = item_index;
  int row = 0;
  for (auto* input : PatchMaster_instance()->inputs) {
    for (auto * trigger : input->triggers) {
      if (row == trigger_num) {
        Editor e;
        e.destroy_trigger(trigger);
        load_data_into_windows();
        return;
      }
      ++row;
    }
  }
}

void Frame::destroy_song(wxCommandEvent& event) {
  Editor e;
  PatchMaster *pm = PatchMaster_instance();
  Song *song = pm->cursor->song();
  if (song != nullptr)
    e.destroy_song(song);
  load_data_into_windows();
}

void Frame::destroy_patch(wxCommandEvent& event) {
  PatchMaster *pm = PatchMaster_instance();
  Patch *patch = pm->cursor->patch();
  if (patch == nullptr)
    return;

  Editor e;
  e.destroy_patch(pm->cursor->song(), patch);
  load_data_into_windows();
}

void Frame::destroy_connection(wxCommandEvent& event) {
  PatchMaster *pm = PatchMaster_instance();
  Patch *patch = pm->cursor->patch();
  if (patch == nullptr)
    return;

  long item_index = wxNOT_FOUND;
  while ((item_index = lc_patch_conns->GetNextItem(item_index,
          wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED)) == wxNOT_FOUND)
    ;
  if (item_index == wxNOT_FOUND)
    return;

  int connection_num = item_index;
  Connection *conn = patch->connections[connection_num];
  if (conn == nullptr)
    return;

  Editor e;
  e.destroy_connection(patch, conn);
  load_data_into_windows();
}

void Frame::destroy_set_list(wxCommandEvent& event) {
  Editor e;
  PatchMaster *pm = PatchMaster_instance();
  SetList *set_list = pm->cursor->set_list();
  if (set_list != pm->all_songs)
    e.destroy_set_list(set_list);
  load_data_into_windows();
}

// ================ MIDI panic ================

void Frame::regular_panic(wxCommandEvent &_event) {
  PatchMaster *pm = PatchMaster_instance();
  show_user_message("Sending panic...");
  pm->panic(false);
  show_user_message("Panic sent", 5);
}

void Frame::super_panic(wxCommandEvent &_event) {
  PatchMaster *pm = PatchMaster_instance();
  show_user_message("Sending \"super panic\": all notes off, all channels...");
  pm->panic(true);
  show_user_message("Panic sent (all notes off, all channels)", 5);
}

// ================ standard menu items ================

void Frame::OnAbout(wxCommandEvent &_event) {
  wxMessageBox("SeaMaster, the MIDI processing and patching system.\nJim Menard, jim@jimmenard.com\nhttps://github.com/jimm/seamaster/wiki",
                "About SeaMaster", wxOK | wxICON_INFORMATION);
}

void Frame::OnOpen(wxCommandEvent &_event) {
  wxFileDialog openFileDialog(this, _("Open SeaMaster file"), "", "",
                              "SeaMaster files (*.org;*.md)|*.org;*.md",
                              wxFD_OPEN|wxFD_FILE_MUST_EXIST);
  if (openFileDialog.ShowModal() != wxID_CANCEL)
    load(openFileDialog.GetPath());
}

// ================ windows ================

void Frame::OnListInstruments(wxCommandEvent &_event) {
  InstrumentDialog(this, PatchMaster_instance()).run();
}

void Frame::OnMonitor(wxCommandEvent &event) {
  new Monitor();
}

// ================ helpers ================

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

  show_user_message(string(wxString::Format("Loaded %s", path).c_str()), 15);
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

  lc_set_list->update();
  lc_set_lists->update();
  lc_song_patches->update();
  lc_patch_conns->update();
  lc_messages->update();
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
