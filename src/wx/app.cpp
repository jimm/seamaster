#include "portmidi.h"
#include "app.h"
#include "frame.h"

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

bool App::OnInit()
{
  Pm_Initialize();
  pm = PatchMaster_instance();
  frame = new Frame(pm, "SeaMaster");
  frame->Show(true);
  return true;
}

int App::FilterEvent(wxEvent &event) {
  if (event.GetEventType() != wxEVT_KEY_DOWN)
    return -1;

  switch (((wxKeyEvent&)event).GetKeyCode()) {
  case 27:
    frame->SetStatusText("PANIC!");
    return true;
  default:
    frame->SetStatusText(wxString::Format("saw key %c", ((wxKeyEvent&)event).GetKeyCode()));
    return true;
  }
  return -1;
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
