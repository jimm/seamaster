#include <wx/cmdline.h>
#include <portmidi.h>
#include "app.h"
#include "frame.h"
#include "set_list_list_box.h"
#include "set_list_box.h"
#include "song_box.h"
#include "../patchmaster.h"

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
wxEND_EVENT_TABLE()

static const wxCmdLineEntryDesc g_cmdLineDesc [] = {
  { wxCMD_LINE_SWITCH, "l", "list-devices", "Display MIDI Devices" },
  { wxCMD_LINE_SWITCH, "i", "initialize", "Output initial SeamMaster file" },
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

void App::show_user_message(string msg) {
  frame->show_user_message(msg);
}

void App::show_user_message(string msg, int clear_secs) {
  frame->show_user_message(msg, clear_secs);
}

bool App::OnInit() {
  if (!wxApp::OnInit())
    return false;

  init_portmidi();
  frame = new Frame("SeaMaster");
  frame->Show(true);
  if (command_line_path.IsEmpty())
    frame->initialize();
  else
    frame->load(command_line_path);
  return true;
}

void App::OnInitCmdLine(wxCmdLineParser& parser) {
  parser.SetDesc(g_cmdLineDesc);
  parser.SetSwitchChars (wxT("-"));
}

bool App::OnCmdLineParsed(wxCmdLineParser& parser) {
  if (parser.Found("l")) {
    list_all_devices();
    return false;
  }
  if (parser.GetParamCount() > 0)
    command_line_path = parser.GetParam(0);

  return true;
}

int App::OnExit() {
  PatchMaster *pm = PatchMaster_instance();
  if (pm) pm->stop();
  close_portmidi();
  return wxApp::OnExit();
}

int App::FilterEvent(wxEvent &event) {
  if (event.GetEventType() != wxEVT_KEY_DOWN || PatchMaster_instance() == 0)
    return -1;

  wxKeyEvent &keyEvent = (wxKeyEvent &)event;
  char cmd = keyEvent.GetKeyCode();
  switch (cmd) {
  case WXK_LEFT:
    frame->prev_song();
    break;
  case WXK_RIGHT:
    frame->next_song();
    break;
  case WXK_UP:
    frame->prev_patch();
    break;
  case WXK_DOWN:
    frame->next_patch();
    break;
  case ' ':
    if (keyEvent.ShiftDown())
      frame->prev_patch();
    else
      frame->next_patch();
    break;
  default:
    return -1;
  }

  frame->refresh();
  return true;
}

void App::init_portmidi() {
  PmError err = Pm_Initialize();
  if (err != 0) {
    fprintf(stderr, "error initializing PortMidi: %s\n", Pm_GetErrorText(err));
    exit(1);
  }
}

void App::close_portmidi() {
  Pm_Terminate();
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
