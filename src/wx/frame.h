#ifndef WX_FRAME_H
#define WX_FRAME_H

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
 #include <wx/wx.h>
 #include <wx/listctrl.h>
#endif
#include "events.h"
#include "../patchmaster.h"

enum {
  ID_GoNextSong = 1,
  ID_GoPrevSong,
  ID_GoNextPatch,
  ID_GoPrevPatch,
  ID_FindSong,
  ID_FindSetList,
  ID_SetListList,
  ID_SetListSongs,
  ID_SongPatches,
  ID_PatchConnections,
  ID_CreateMessage,
  ID_CreateTrigger,
  ID_CreateSong,
  ID_CreatePatch,
  ID_CreateConnection,
  ID_CreateSetList,
  ID_DestroyMessage,
  ID_DestroyTrigger,
  ID_DestroySong,
  ID_DestroyPatch,
  ID_DestroyConnection,
  ID_DestroySetList,
  ID_ListInstruments,
  ID_Monitor,
  ID_RegularPanic,
  ID_SuperPanic,
  ID_MessageList,
  ID_TriggerList
};

class wxListCtrl;
class wxTextCtrl;
class SetListBox;
class SetListListBox;
class SongBox;
class PatchConnections;
class MessageList;
class TriggerList;
class SetListEditor;

class Frame: public wxFrame {
public:
  Frame(const wxString& title);
  virtual ~Frame() {}

  void initialize();
  void load(wxString path);

  void update(wxCommandEvent& event) { update(); }
  void update();

  void show_user_message(std::string);
  void show_user_message(std::string, int);
  void clear_user_message();
  void clear_user_message_after(int);
  int clear_user_message_seconds() { return clear_msg_secs; }
  int clear_user_message_id() { return clear_msg_id; }

  void next_song();
  void prev_song();
  void next_patch();
  void prev_patch();
  void find_set_list();
  void find_song();

  void jump_to_set_list();
  void jump_to_song();
  void jump_to_patch();

private:
  wxMenuBar *menu_bar;
  SetListListBox *lc_set_lists;
  SetListBox *lc_set_list;
  SongBox *lc_song_patches;
  PatchConnections *lc_patch_conns;
  MessageList *lc_messages;
  TriggerList *lc_triggers;
  wxTextCtrl *lc_notes;
  SetListEditor *set_list_editor;
  int clear_msg_secs;
  int clear_msg_id;

  void OnOpen(wxCommandEvent& event);
  void OnListInstruments(wxCommandEvent& event);
  void OnMonitor(wxCommandEvent& event);
  void OnExit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);

  void regular_panic(wxCommandEvent &_event);
  void super_panic(wxCommandEvent &_event);

  void next_song(wxCommandEvent &_event) { next_song(); }
  void prev_song(wxCommandEvent &_event) { prev_song(); }
  void next_patch(wxCommandEvent &_event) { next_patch(); }
  void prev_patch(wxCommandEvent &_event) { prev_patch(); }
  void find_set_list(wxCommandEvent &_event) { find_set_list(); }
  void find_song(wxCommandEvent &_event) { find_song(); }

  void jump_to_set_list(wxCommandEvent& event);
  void jump_to_song(wxCommandEvent& event);
  void jump_to_patch(wxCommandEvent& event);

  void create_message(wxCommandEvent& event);
  void create_trigger(wxCommandEvent& event);
  void create_song(wxCommandEvent& event);
  void create_patch(wxCommandEvent& event);
  void create_connection(wxCommandEvent& event);
  void create_set_list(wxCommandEvent& event);

  void send_message(wxCommandEvent& event);

  void edit_message(wxCommandEvent& event);
  void edit_message(Message *);
  void edit_trigger(wxListEvent& event);
  void edit_trigger(Trigger *);
  void edit_set_list(wxCommandEvent& event);
  void edit_set_list(SetList *set_list);
  void set_list_editor_done(wxCommandEvent& event);
  void edit_song(wxCommandEvent& event);
  void edit_song(Song *);
  void edit_patch(wxCommandEvent& event);
  void edit_patch(Patch *);
  void edit_connection(wxListEvent& event);
  void edit_connection(Connection *);

  void destroy_message(wxCommandEvent& event);
  void destroy_trigger(wxCommandEvent& event);
  void destroy_song(wxCommandEvent& event);
  void destroy_patch(wxCommandEvent& event);
  void destroy_connection(wxCommandEvent& event);
  void destroy_set_list(wxCommandEvent& event);

  void make_frame_panels();
  void make_menu_bar();
  wxWindow * make_set_list_panel(wxPanel *);
  wxWindow * make_set_list_list_panel(wxPanel *);
  wxWindow * make_song_panel(wxPanel *);
  wxWindow * make_message_panel(wxPanel *);
  wxWindow * make_trigger_panel(wxPanel *);
  wxWindow * make_notes_panel(wxPanel *);
  wxWindow * make_patch_panel(wxPanel *);

  long selected_trigger_index();
  long selected_connection_index();

  void update_lists();
  void update_song_notes();
  void update_menu_items();

  wxDECLARE_EVENT_TABLE();
};

#endif /* WX_FRAME_H */
