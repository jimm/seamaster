#include <wx/cmdline.h>
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

static const wxCmdLineEntryDesc g_cmdLineDesc [] = {
  { wxCMD_LINE_SWITCH, "l", "list-devices", "Display MIDI Devices" },
  { wxCMD_LINE_PARAM, nullptr, nullptr, "SeaMaster file", wxCMD_LINE_VAL_STRING,
    wxCMD_LINE_PARAM_OPTIONAL },
  { wxCMD_LINE_NONE }
};

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
  if (!wxApp::OnInit())
    return false;

  init_portmidi();
  prev_cmd = '\0';
  frame = new Frame("SeaMaster");
  frame->Show(true);
  if (!command_line_path.IsEmpty())
    frame->load(command_line_path);
  return true;
}

void App::OnInitCmdLine(wxCmdLineParser& parser) {
  parser.SetDesc (g_cmdLineDesc);
  parser.SetSwitchChars (wxT("-"));
}

bool App::OnCmdLineParsed(wxCmdLineParser& parser) {
  if (parser.Found(wxT("l"))) {
    list_all_devices();
    return false;
  }
  if (parser.GetParamCount() > 0)
    command_line_path = parser.GetParam(0);

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

void App::init_portmidi() {
  PmError err = Pm_Initialize();
  if (err != 0) {
    fprintf(stderr, "error initializing PortMidi: %s\n", Pm_GetErrorText(err));
    exit(1);
  }

}

void App::list_devices(const char *title, const PmDeviceInfo *infos[], int num_devices) {
  printf("%s:\n", title);
  for (int i = 0; i < num_devices; ++i)
    if (infos[i] != nullptr) {
      const char *name = infos[i]->name;
      const char *q = (name[0] == ' ' || name[strlen(name)-1] == ' ') ? "\"" : "";
      printf("  %2d: %s%s%s%s\n", i, q, name, q, infos[i]->opened ? " (open)" : "");
    }
}

void App::list_all_devices() {
  init_portmidi();
  int num_devices = Pm_CountDevices();
  const PmDeviceInfo *inputs[num_devices], *outputs[num_devices];

  for (int i = 0; i < num_devices; ++i) {
    const PmDeviceInfo *info = Pm_GetDeviceInfo(i);
    inputs[i] = info->input ? info : 0;
    outputs[i] = info->output ? info : 0;
  }

  list_devices("Inputs", inputs, num_devices);
  list_devices("Outputs", outputs, num_devices);
}
