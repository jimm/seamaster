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
#include "message_editor.h"
#include "trigger_editor.h"
#include "connection_editor.h"
#include "set_list_editor.h"
#include "../patchmaster.h"
#include "../cursor.h"
#include "../storage.h"
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

wxDEFINE_EVENT(Frame_Refresh, wxCommandEvent);

wxBEGIN_EVENT_TABLE(Frame, wxFrame)
  EVT_MENU(wxID_OPEN,  Frame::OnOpen)
  EVT_MENU(ID_GoNextSong, Frame::next_song)
  EVT_MENU(ID_GoPrevSong, Frame::prev_song)
  EVT_MENU(ID_GoNextPatch, Frame::next_patch)
  EVT_MENU(ID_GoPrevPatch, Frame::prev_patch)
  EVT_MENU(ID_FindSetList, Frame::find_set_list)
  EVT_MENU(ID_FindSong, Frame::find_song)
  EVT_MENU(ID_CreateMessage, Frame::create_message)
  EVT_MENU(ID_CreateTrigger, Frame::create_trigger)
  EVT_MENU(ID_CreateSong, Frame::create_song)
  EVT_MENU(ID_CreatePatch, Frame::create_patch)
  EVT_MENU(ID_CreateConnection, Frame::create_connection)
  EVT_MENU(ID_CreateSetList, Frame::create_set_list)
  EVT_MENU(ID_DestroyMessage, Frame::destroy_message)
  EVT_MENU(ID_DestroyTrigger, Frame::destroy_trigger)
  EVT_MENU(ID_DestroySong, Frame::destroy_song)
  EVT_MENU(ID_DestroyPatch, Frame::destroy_patch)
  EVT_MENU(ID_DestroyConnection, Frame::destroy_connection)
  EVT_MENU(ID_DestroySetList, Frame::destroy_set_list)
  EVT_MENU(ID_ListInstruments, Frame::OnListInstruments)
  EVT_MENU(ID_Monitor, Frame::OnMonitor)
  EVT_MENU(ID_RegularPanic, Frame::regular_panic)
  EVT_MENU(ID_SuperPanic, Frame::super_panic)
  EVT_MENU(wxID_EXIT,  Frame::OnExit)
  EVT_MENU(wxID_ABOUT, Frame::OnAbout)

  EVT_LISTBOX(ID_SetListList, Frame::jump_to_set_list)
  EVT_LISTBOX_DCLICK(ID_SetListList, Frame::edit_set_list)
  EVT_LISTBOX(ID_SetListSongs, Frame::jump_to_song)
  EVT_LISTBOX_DCLICK(ID_SetListSongs, Frame::edit_song)
  EVT_LISTBOX(ID_SongPatches, Frame::jump_to_patch)
  EVT_LISTBOX_DCLICK(ID_SongPatches, Frame::edit_patch)
  EVT_LISTBOX(ID_MessageList, Frame::send_message)
  EVT_LISTBOX_DCLICK(ID_MessageList, Frame::edit_message)

  EVT_LIST_ITEM_ACTIVATED(ID_TriggerList, Frame::edit_trigger)
  EVT_LIST_ITEM_ACTIVATED(ID_PatchConnections, Frame::edit_connection)

  EVT_TEXT(ID_SongNotes, Frame::set_song_notes)

  EVT_COMMAND(wxID_ANY, Frame_Refresh, Frame::update)
wxEND_EVENT_TABLE()

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
  : wxFrame(NULL, wxID_ANY, title, wxPoint(FRAME_POS_X, FRAME_POS_Y)),
    updating_notes(false)
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
  lc_notes = new wxTextCtrl(p, ID_SongNotes, "", wxDefaultPosition,
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
  wxMenu *menu_file = new wxMenu;
  menu_file->Append(wxID_OPEN);
  menu_file->AppendSeparator();
  menu_file->Append(wxID_EXIT);

  wxMenu *menu_edit = new wxMenu;
  menu_edit->Append(ID_CreateMessage, "New Message\tCtrl-Shift-M", "Create a new message");
  menu_edit->Append(ID_CreateTrigger, "New Trigger\tCtrl-Shift-T", "Create a new trigger");
  menu_edit->Append(ID_CreateSong, "New Song\tCtrl-Shift-S", "Create a new song");
  menu_edit->Append(ID_CreatePatch, "New Patch\tCtrl-Shift-P", "Create a new patch");
  menu_edit->Append(ID_CreateConnection, "New Connection\tCtrl-Shift-C", "Create a new connection");
  menu_edit->Append(ID_CreateSetList, "New Set List\tCtrl-Shift-L", "Create a new set list");
  menu_edit->AppendSeparator();
  menu_edit->Append(ID_DestroyMessage, "Delete Message\tCtrl-Alt-M", "Delete the current message");
  menu_edit->Append(ID_DestroyTrigger, "Delete Trigger\tCtrl-Alt-T", "Delete the current trigger");
  menu_edit->Append(ID_DestroySong, "Delete Song\tCtrl-Alt-S", "Delete the current song");
  menu_edit->Append(ID_DestroyPatch, "Delete Patch\tCtrl-Alt-P", "Delete the current patch");
  menu_edit->Append(ID_DestroyConnection, "Delete Connection\tCtrl-Alt-C", "Delete the current connection");
  menu_edit->Append(ID_DestroySetList, "Delete Set List\tCtrl-Alt-L", "Delete the current set list");

  wxMenu *menu_go = new wxMenu;
  menu_go->Append(ID_GoNextSong, "Next Song\tCtrl-Right", "Move to the next song");
  menu_go->Append(ID_GoPrevSong, "Prev Song\tCtrl-Left", "Move to the previous song");
  menu_go->Append(ID_GoNextPatch, "Next Patch\tCtrl-Down", "Move to the next patch");
  menu_go->Append(ID_GoPrevPatch, "Prev Patch\tCtrl-Up", "Move to the previous patch");
  menu_go->AppendSeparator();
  menu_go->Append(ID_FindSong, "Find Song...\tCtrl-F", "Find song by name");
  menu_go->Append(ID_FindSetList, "Find Set List...\tCtrl-T", "Find set list by name");

  wxMenu *menu_windows = new wxMenu;
  menu_windows->Append(ID_ListInstruments, "&Instruments\tCtrl-I",
                      "Displays input and output instruments");
  menu_windows->Append(ID_Monitor, "MIDI &Monitor\tCtrl-M",
                      "Open the MIDI Monitor window");

  wxMenu *menu_midi = new wxMenu;
  menu_midi->Append(ID_RegularPanic, "&Send All Notes Off\tCtrl-A",
                   "Send All Notes Off controller message on all channels");
  menu_midi->Append(ID_SuperPanic, "&Send Super-Panic\t\e",
                   "Send Notes Off messages, all notes, all channels");

  wxMenu *menu_help = new wxMenu;
  menu_help->Append(wxID_ABOUT);

  menu_bar = new wxMenuBar;
  menu_bar->Append(menu_file, "&File");
  menu_bar->Append(menu_edit, "&Edit");
  menu_bar->Append(menu_go, "&Go");
  menu_bar->Append(menu_midi, "&MIDI");
  menu_bar->Append(menu_windows, "&Windows");
  menu_bar->Append(menu_help, "&Help");
  SetMenuBar(menu_bar);
#if defined(__WXMAC__)
  menu_bar->OSXGetAppleMenu()->SetTitle("SeaMaster");
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
  update();
}

void Frame::prev_song() {
  PatchMaster *pm = PatchMaster_instance();
  pm->prev_song();
  update();
}

void Frame::next_patch() {
  PatchMaster *pm = PatchMaster_instance();
  pm->next_patch();
  update();
}

void Frame::prev_patch() {
  PatchMaster *pm = PatchMaster_instance();
  pm->prev_patch();
  update();
}

void Frame::find_set_list() {
  PatchMaster *pm = PatchMaster_instance();
  wxTextEntryDialog prompt(this, "Find Set List");
  if (prompt.ShowModal() == wxID_OK) {
    wxString str = prompt.GetValue();
    if (!str.IsEmpty()) {
      pm->goto_set_list(str.ToStdString());
      update();
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
      update();
    }
  }
}

void Frame::jump_to_set_list(wxCommandEvent &event) {
  if (event.GetEventType() == wxEVT_LISTBOX && event.IsSelection()) {
    lc_set_lists->jump();
    update();
  }
}

void Frame::jump_to_song(wxCommandEvent &event) {
  if (event.GetEventType() == wxEVT_LISTBOX && event.IsSelection()) {
    lc_set_list->jump();
    update();
  }
}

void Frame::jump_to_patch(wxCommandEvent &event) {
  if (event.GetEventType() == wxEVT_LISTBOX && event.IsSelection()) {
    lc_song_patches->jump();
    update();
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
  update();
  edit_message(message);
}

void Frame::create_trigger(wxCommandEvent& event) {
  Editor e;
  PatchMaster *pm = PatchMaster_instance();
  Trigger *trigger = e.create_trigger(pm->inputs.front());
  update();
  edit_trigger(trigger);
}

void Frame::create_song(wxCommandEvent& event) {
  Editor e;
  Song *song = e.create_song();
  update();
  edit_song(song);
}

void Frame::create_patch(wxCommandEvent& event) {
  Editor e;
  Patch *patch = e.create_patch();
  update();
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
  Patch *patch = pm->cursor->patch();
  if (patch == nullptr) {
    wxMessageBox("Please select a patch", "New Connection",
                 wxOK | wxICON_INFORMATION);
    return;
  }

  Connection *conn = e.create_connection(patch, pm->inputs.front(), pm->outputs.front());
  update();
  edit_connection(conn);
}

void Frame::create_set_list(wxCommandEvent& event) {
  Editor e;
  SetList *set_list = e.create_set_list();
  update();
  edit_set_list(set_list);
}

void Frame::edit_message(wxCommandEvent& event) {
  int message_num = lc_messages->GetSelection();
  if (message_num != wxNOT_FOUND)
    edit_message(PatchMaster_instance()->messages[message_num]);
}

void Frame::edit_message(Message *message) {
  if (message != nullptr)
    new MessageEditor(this, message);
}

void Frame::edit_trigger(wxListEvent& event) {
  long trigger_num = selected_trigger_index();
  if (trigger_num == wxNOT_FOUND)
    return;

  edit_trigger(trigger_from_index(trigger_num));
}

void Frame::edit_trigger(Trigger *trigger) {
  if (trigger != nullptr)
    new TriggerEditor(this, trigger);
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
      update();
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
      update();
    }
  }
}

void Frame::edit_connection(wxListEvent& event) {
  PatchMaster *pm = PatchMaster_instance();
  Patch *patch = pm->cursor->patch();
  if (patch == nullptr)
    return;

  int connection_num = selected_connection_index();
  Connection *conn = patch->connections[connection_num];
  if (conn == nullptr)
    return;

  edit_connection(conn);
}

void Frame::edit_connection(Connection *conn) {
  if (conn != nullptr)
    new ConnectionEditor(this, conn);
}

void Frame::set_song_notes(wxCommandEvent& event) {
  if (updating_notes)
    return;
  Song *song = PatchMaster_instance()->cursor->song();
  if (song != nullptr)
    song->notes = lc_notes->GetValue();
}

void Frame::destroy_message(wxCommandEvent& event) {
  PatchMaster *pm = PatchMaster_instance();
  if (pm->messages.empty())
    return;

  Editor e;
  int message_num = lc_messages->GetSelection();
  if (message_num == wxNOT_FOUND)
    return;
  Message *message = pm->messages[message_num];
  e.destroy_message(message);
  update();
}

void Frame::destroy_trigger(wxCommandEvent& event) {
  long trigger_num = selected_trigger_index();
  if (trigger_num == wxNOT_FOUND)
    return;

  Editor e;
  e.destroy_trigger(trigger_from_index(trigger_num));
}

void Frame::destroy_song(wxCommandEvent& event) {
  Editor e;
  PatchMaster *pm = PatchMaster_instance();
  Song *song = pm->cursor->song();
  if (song != nullptr)
    e.destroy_song(song);
  update();
}

void Frame::destroy_patch(wxCommandEvent& event) {
  PatchMaster *pm = PatchMaster_instance();
  Patch *patch = pm->cursor->patch();
  if (patch == nullptr)
    return;

  Editor e;
  e.destroy_patch(pm->cursor->song(), patch);
  update();
}

void Frame::destroy_connection(wxCommandEvent& event) {
  PatchMaster *pm = PatchMaster_instance();
  Patch *patch = pm->cursor->patch();
  if (patch == nullptr)
    return;

  int connection_num = selected_connection_index();
  Connection *conn = patch->connections[connection_num];
  if (conn == nullptr)
    return;

  Editor e;
  e.destroy_connection(patch, conn);
  update();
}

void Frame::destroy_set_list(wxCommandEvent& event) {
  Editor e;
  PatchMaster *pm = PatchMaster_instance();
  SetList *set_list = pm->cursor->set_list();
  if (set_list != pm->all_songs)
    e.destroy_set_list(set_list);
  update();
}

// ================ keypress handling ================

int Frame::handle_global_key_event(wxKeyEvent &event) {
  if (FindFocus() == lc_notes)
    return -1;

  switch (event.GetKeyCode()) {
  case WXK_LEFT: case 'K':
    prev_song();
    break;
  case WXK_RIGHT: case 'J':
    next_song();
    break;
  case WXK_UP: case 'P':
    prev_patch();
    break;
  case WXK_DOWN: case 'N':
    next_patch();
    break;
  case ' ':
    if (event.ShiftDown())
      prev_patch();
    else
      next_patch();
    break;
  default:
    return -1;
  }

  update();
  return true;
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
  update();
}

void Frame::load(wxString path) {
  PatchMaster *old_pm = PatchMaster_instance();
  bool testing = old_pm != nullptr && old_pm->testing;
  Storage storage(path);
  PatchMaster *pm = storage.load(testing);
  if (storage.has_error()) {
    wxLogError("Cannot open file '%s': %s.", path, storage.error());
    return;
  }

  show_user_message(string(wxString::Format("Loaded %s", path).c_str()), 15);
  if (old_pm != nullptr) {
    old_pm->stop();
    delete old_pm;
  }
  pm->start();                  // initializes cursor
  update();                     // must come after start
}

long Frame::selected_trigger_index() {
  return lc_triggers->GetNextItem(wxNOT_FOUND, wxLIST_NEXT_ALL,
                                  wxLIST_STATE_SELECTED);
}

long Frame::selected_connection_index() {
  return lc_patch_conns->GetNextItem(wxNOT_FOUND, wxLIST_NEXT_ALL,
                                     wxLIST_STATE_SELECTED);
}

Trigger *Frame::trigger_from_index(long index) {
  int row = 0;
  for (auto* input : PatchMaster_instance()->inputs) {
    for (auto * trigger : input->triggers) {
      if (row == index)
        return trigger;
      ++row;
    }
  }
  return nullptr;
}

void Frame::update() {
  update_lists();
  update_song_notes();
  update_menu_items();
}

void Frame::update_lists() {
  lc_set_list->update();
  lc_set_lists->update();
  lc_song_patches->update();
  lc_patch_conns->update();
  lc_messages->update();
  lc_triggers->update();
}

void Frame::update_song_notes() {
  PatchMaster *pm = PatchMaster_instance();
  Cursor *cursor = pm->cursor;

  Song *song = cursor->song();
  updating_notes = true;
  lc_notes->Clear();
  lc_notes->AppendText(song->notes);
  updating_notes = false;
}

void Frame::update_menu_items() {
  PatchMaster *pm = PatchMaster_instance();
  Cursor *cursor = pm->cursor;

  // edit menu
  menu_bar->FindItem(ID_DestroyMessage, nullptr)
    ->Enable(!pm->messages.empty() && lc_messages->GetSelection() != wxNOT_FOUND);

  menu_bar->FindItem(ID_DestroyTrigger, nullptr)
    ->Enable(selected_trigger_index() != wxNOT_FOUND);

  menu_bar->FindItem(ID_DestroySong, nullptr)
    ->Enable(pm->cursor->song() != nullptr);

  menu_bar->FindItem(ID_DestroyPatch, nullptr)
    ->Enable(pm->cursor->patch() != nullptr);

  menu_bar->FindItem(ID_DestroyConnection, nullptr)
    ->Enable(pm->cursor->patch() != nullptr && selected_connection_index() != wxNOT_FOUND);

  SetList *set_list = pm->cursor->set_list();
  menu_bar->FindItem(ID_DestroySetList, nullptr)
    ->Enable(set_list != nullptr && set_list != pm->all_songs);

  // go menu
  menu_bar->FindItem(ID_GoNextSong, nullptr)->Enable(cursor->has_next_song());
  menu_bar->FindItem(ID_GoPrevSong, nullptr)->Enable(cursor->has_prev_song());
  menu_bar->FindItem(ID_GoNextPatch, nullptr)->Enable(cursor->has_next_patch());
  menu_bar->FindItem(ID_GoPrevPatch, nullptr)->Enable(cursor->has_prev_patch());
}
