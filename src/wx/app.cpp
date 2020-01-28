#include "wx/listctrl.h"        // for wxListCtrl
#include "wx/textctrl.h"        // for wxListCtrl
#include "app.h"
#include "../cursor.h"
#include "../loader.h"

static App *a_instance = nullptr;

App *app_instance() {
  return a_instance;
}

App::App() {
  a_instance = this;
}

App::~App() {
  if (a_instance == this)
  a_instance = nullptr;
}

void App::show_message(string msg) {
  frame->SetStatusText(msg.c_str());
}

void App::clear_message() {
  frame->SetStatusText("");
}

void *app_clear_message_thread(void *gui_vptr) {
  App *gui = (App *)gui_vptr;
  int clear_message_id = gui->clear_message_id();

  sleep(gui->clear_message_seconds());

  // Only clear the window if the id hasn't changed
  if (gui->clear_message_id() == clear_message_id)
    gui->clear_message();
  return nullptr;
}

void App::clear_message_after(int secs) {
  clear_msg_secs = secs;
  clear_msg_id++;

  pthread_t pthread;
  pthread_create(&pthread, 0, app_clear_message_thread, this);
}

class MyFrame: public wxFrame {
public:
  MyFrame(PatchMaster *pm, const wxString& title);

private:
  PatchMaster *pm;
  wxListCtrl *lc_song_lists;
  wxListCtrl *lc_song_list;
  wxListCtrl *lc_song;
  wxListCtrl *lc_patch;
  wxListCtrl *lc_triggers;
  wxTextCtrl *lc_notes;

  void OnOpen(wxCommandEvent& event);
  void OnMonitor(wxCommandEvent& event);
  void OnExit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);
  wxDECLARE_EVENT_TABLE();

  void make_frame_panels();
  void make_menu_bar();
  wxWindow * make_song_list_panel();
  wxWindow * make_song_list_list_panel();
  wxWindow * make_song_panel();
  wxWindow * make_trigger_panel();
  wxWindow * make_notes_panel();
  wxWindow * make_patch_panel();
  void load_data_into_windows();
};

enum {
  ID_Monitor = 1
};

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_MENU(wxID_OPEN,  MyFrame::OnOpen)
EVT_MENU(ID_Monitor, MyFrame::OnMonitor)
EVT_MENU(wxID_EXIT,  MyFrame::OnExit)
EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
wxEND_EVENT_TABLE()
wxIMPLEMENT_APP(App);

bool App::OnInit()
{
  pm = PatchMaster_instance();
  frame = new MyFrame(pm, "SeaMaster");
  frame->Show(true);
  return true;
}

MyFrame::MyFrame(PatchMaster *patchmaster, const wxString& title)
  : pm(patchmaster), wxFrame(NULL, wxID_ANY, title)
{
  make_frame_panels();
  make_menu_bar();
}

void MyFrame::make_frame_panels() {
  wxSizer * const main_sizer = new wxBoxSizer(wxVERTICAL);

  wxSizer * const top_sizer = new wxBoxSizer(wxHORIZONTAL);
  wxSizer * const top_left_sizer = new wxBoxSizer(wxVERTICAL);
  top_left_sizer->Add(make_song_list_panel(), wxSizerFlags().Expand().Border());
  top_left_sizer->Add(make_song_list_list_panel(), wxSizerFlags().Expand().Border());
  top_sizer->Add(top_left_sizer, wxSizerFlags().Expand().Border());

  wxSizer * const top_middle_sizer = new wxBoxSizer(wxVERTICAL);
  top_middle_sizer->Add(make_song_panel(), wxSizerFlags().Expand().Border());
  top_middle_sizer->Add(make_trigger_panel(), wxSizerFlags().Expand().Border());
  top_sizer->Add(top_middle_sizer, wxSizerFlags().Expand().Border());

  top_sizer->Add(make_notes_panel(), wxSizerFlags().Expand().Border(wxALL));

  main_sizer->Add(top_sizer, wxSizerFlags().Expand().Border());
  main_sizer->Add(make_patch_panel(), wxSizerFlags().Expand().Border());

  SetSizerAndFit(main_sizer);
}

wxWindow * MyFrame::make_song_list_panel() {
  lc_song_list = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(100, 150));
  lc_song_list->InsertItem(0, wxString("Song List"));
  return lc_song_list;
}

wxWindow * MyFrame::make_song_list_list_panel() {
  lc_song_lists = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(100, 100));
  lc_song_lists->InsertItem(0, wxString("Song Lists"));
  return lc_song_lists;
}

wxWindow * MyFrame::make_song_panel() {
  lc_song = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(100, 150));
  lc_song->InsertItem(0, wxString("Song"));
  return lc_song;
}

wxWindow * MyFrame::make_trigger_panel() {
  lc_triggers = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(100, 100));
  lc_triggers->InsertItem(0, wxString("Triggers"));
  return lc_triggers;
}

wxWindow * MyFrame::make_notes_panel() {
  lc_notes = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(100, 250), wxTE_MULTILINE);
  lc_notes->AppendText("Notes");
  return lc_notes;
}

wxWindow * MyFrame::make_patch_panel() {
  lc_patch = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(600, 150), wxLC_REPORT);
  int i = 0;
  lc_patch->InsertColumn(i++, "Input");
  lc_patch->InsertColumn(i++, "Chan");
  lc_patch->InsertColumn(i++, "Output");
  lc_patch->InsertColumn(i++, "Chan");
  lc_patch->InsertColumn(i++, "Zone");
  lc_patch->InsertColumn(i++, "Xpose");
  lc_patch->InsertColumn(i++, "Prog");
  lc_patch->InsertColumn(i++, "CC Filt/Map");
  return lc_patch;
}

void MyFrame::make_menu_bar() {
  wxMenu *menuFile = new wxMenu;
  menuFile->Append(wxID_OPEN);
  menuFile->AppendSeparator();
  menuFile->Append(wxID_EXIT);

  wxMenu *menuView = new wxMenu;
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

void MyFrame::OnExit(wxCommandEvent& event) {
  Close(true);
  exit(0);
}

void MyFrame::OnAbout(wxCommandEvent& event) {
  wxMessageBox("This is SeaMaster, the MIDI processing and patching system.",
                "About SeaMaster", wxOK | wxICON_INFORMATION);
}

void MyFrame::OnOpen(wxCommandEvent& event) {
  wxFileDialog openFileDialog(this, _("Open SeaMaster file"), "", "",
                              "SeaMaster files (*.org;*.md)|*.org;*.md",
                              wxFD_OPEN|wxFD_FILE_MUST_EXIST);
  if (openFileDialog.ShowModal() == wxID_CANCEL)
    return;

  bool testing = pm != nullptr && pm->testing;
  PatchMaster *old_pm = pm;
  Loader loader;
  pm = loader.load(openFileDialog.GetPath(), testing);

  if (loader.has_error()) {
    wxLogError("Cannot open file '%s': %s.", openFileDialog.GetPath(),
               loader.error());
    pm = old_pm;
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

void MyFrame::OnMonitor(wxCommandEvent &event) {
  wxMessageBox("MIDI Monitor not yet implemented.", "MIDI Monitor",
               wxOK | wxICON_INFORMATION);
  // TODO open monitor frame if it's not already open
  // if already exists return
  // MonitorFrame *frame = new MonitorFrame(pm, wxPoint(50, 50), wxSize(450, 340));
  // frame->Show(true);
}

void MyFrame::load_data_into_windows() {
  int i;

  Cursor *cursor = pm->cursor;

  SongList *song_list = cursor->song_list();
  lc_song_list->ClearAll();
  if (song_list != nullptr) {
    i = 0;
    for (auto& song : song_list->songs)
      lc_song_list->InsertItem(i++, song->name.c_str());
  }

  lc_song_lists->ClearAll();
  i = 0;
  for (auto* song_list : pm->song_lists)
    lc_song_lists->InsertItem(i++, song_list->name.c_str());

  Song *song = cursor->song();
  lc_song->ClearAll();
  lc_notes->Clear();
  if (song != nullptr) {
    i = 0;
    for (auto* patch : song->patches)
      lc_song->InsertItem(i++, patch->name.c_str());

    i = 0;
    for (auto* line : song->notes) {
      lc_notes->AppendText(line);
      lc_notes->AppendText("\n");
    }
  }

  lc_triggers->ClearAll();
  i = 0;
  for (auto *input : pm->inputs)
    for (auto* trigger : input->triggers)
      lc_triggers->InsertItem(i++, "TODO --- display trigger");

  Patch *patch = cursor->patch();
  lc_patch->ClearAll();
  i = 0;
  lc_patch->InsertColumn(i++, "Input");
  lc_patch->InsertColumn(i++, "Chan");
  lc_patch->InsertColumn(i++, "Output");
  lc_patch->InsertColumn(i++, "Chan");
  lc_patch->InsertColumn(i++, "Zone");
  lc_patch->InsertColumn(i++, "Xpose");
  lc_patch->InsertColumn(i++, "Prog");
  lc_patch->InsertColumn(i++, "CC Filt/Map");
  if (patch != nullptr) {
    i = 0;
    for (auto* conn : patch->connections) {
      lc_patch->InsertItem(i, conn->input->name.c_str());
      lc_patch->SetItem(i, 1, conn->input_chan == -1 ? "all" : wxString::Format("%d", conn->input_chan));
      lc_patch->SetItem(i, 2, conn->output->name.c_str());
      lc_patch->SetItem(i, 3, conn->output_chan == -1 ? "all" : wxString::Format("%d", conn->output_chan));
      lc_patch->SetItem(i, 4, "TODO");
      lc_patch->SetItem(i, 5, "TODO");
      lc_patch->SetItem(i, 6, "TODO");
      lc_patch->SetItem(i, 7, "TODO");
      ++i;
    }
  }
}
