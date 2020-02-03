#ifndef WX_FRAME_H
#define WX_FRAME_H

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
 #include <wx/wx.h>
#endif

enum {
  ID_ListDevices = 1,
  ID_GoNextSong,
  ID_GoPrevSong,
  ID_GoNextPatch,
  ID_GoPrevPatch,
  ID_FindSong,
  ID_FindSetList,
  ID_JumpToSetList,
  ID_JumpToSong,
  ID_JumpToPatch,
  ID_Monitor,
  ID_RegularPanic,
  ID_SuperPanic
};

class PatchMaster;
class wxListCtrl;
class wxTextCtrl;
class SongListBox;
class SongListListBox;
class SongBox;
class PatchList;

class Frame: public wxFrame {
public:
  Frame(const wxString& title);
  virtual ~Frame() {}

  void load(wxString path);
  void refresh() { load_data_into_windows(); }

  void show_message(std::string);
  void show_message(std::string, int);
  void clear_message();
  void clear_message_after(int);
  int clear_message_seconds() { return clear_msg_secs; }
  int clear_message_id() { return clear_msg_id; }

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
  SongListListBox *lc_song_lists;
  SongListBox *lc_song_list;
  SongBox *lc_song;
  PatchList *lc_patch;
  wxListCtrl *lc_triggers;
  wxTextCtrl *lc_notes;
  int clear_msg_secs;
  int clear_msg_id;

  void OnOpen(wxCommandEvent& event);
  void OnListDevices(wxCommandEvent& event);
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

  void make_frame_panels();
  void make_menu_bar();
  wxWindow * make_song_list_panel(wxPanel *);
  wxWindow * make_song_list_list_panel(wxPanel *);
  wxWindow * make_song_panel(wxPanel *);
  wxWindow * make_trigger_panel(wxPanel *);
  wxWindow * make_notes_panel(wxPanel *);
  wxWindow * make_patch_panel(wxPanel *);
  void load_data_into_windows();

  wxDECLARE_EVENT_TABLE();
};

#endif /* WX_FRAME_H */
