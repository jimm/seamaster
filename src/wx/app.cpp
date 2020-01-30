#include "portmidi.h"
#include "app.h"
#include "frame.h"
#include "../patchmaster.h"

wxBEGIN_EVENT_TABLE(Frame, wxFrame)
EVT_MENU(wxID_OPEN,  Frame::OnOpen)
EVT_MENU(ID_ListDevices, Frame::OnListDevices)
EVT_MENU(ID_Monitor, Frame::OnMonitor)
EVT_MENU(wxID_EXIT,  Frame::OnExit)
EVT_MENU(wxID_ABOUT, Frame::OnAbout)
wxEND_EVENT_TABLE()

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

bool App::OnInit() {
  prev_cmd = '\0';
  Pm_Initialize();
  frame = new Frame("SeaMaster");
  frame->Show(true);
  return true;
}

int App::OnExit() {
  Pm_Terminate();
  return wxApp::OnExit();
}

int App::FilterEvent(wxEvent &event) {
  if (event.GetEventType() != wxEVT_KEY_DOWN || PatchMaster_instance() == 0)
    return -1;

  char cmd = ((wxKeyEvent&)event).GetKeyCode();
  switch (cmd) {
  case 'J': case WXK_DOWN: case ' ':
    PatchMaster_instance()->next_patch();
    break;
  case 'K': case WXK_UP:
    PatchMaster_instance()->prev_patch();
    break;
  case 'N': case WXK_RIGHT:
    PatchMaster_instance()->next_song();
    break;
  case 'P': case WXK_LEFT:
    PatchMaster_instance()->prev_song();
    break;
  case 'G':
    {
      wxTextEntryDialog prompt(frame, "Go To Song");
      if (prompt.ShowModal() == wxID_OK) {
        wxString str = prompt.GetValue();
        if (!str.IsEmpty())
          PatchMaster_instance()->goto_song(str.ToStdString());
      }
    }
    break;
  case 't':
    {
      wxTextEntryDialog prompt(frame, "Go To Song List");
      if (prompt.ShowModal() == wxID_OK) {
        wxString str = prompt.GetValue();
        if (!str.IsEmpty())
          PatchMaster_instance()->goto_song_list(str.ToStdString());
      }
    }
    break;
  case '\e':                  /* escape */
    show_message("Sending panic...");
    PatchMaster_instance()->panic(prev_cmd == '\e');
    show_message("Panic sent");
    clear_message_after(5);
    break;
  default:
    return -1;
  }

  prev_cmd = cmd;
  frame->refresh();
  return true;
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
