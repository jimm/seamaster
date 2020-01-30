#ifndef WX_FRAME_H
#define WX_FRAME_H

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
 #include <wx/wx.h>
#endif

enum {
  ID_ListDevices = 1,
  ID_Monitor
};

class PatchMaster;
class wxListCtrl;
class wxTextCtrl;
class PatchList;

class Frame: public wxFrame {
public:
  Frame(const wxString& title);
  virtual ~Frame() {}

  void load(wxString path);
  void refresh() { load_data_into_windows(); }

private:
  wxListCtrl *lc_song_lists;
  wxListCtrl *lc_song_list;
  wxListCtrl *lc_song;
  PatchList *lc_patch;
  wxListCtrl *lc_triggers;
  wxTextCtrl *lc_notes;

  void OnOpen(wxCommandEvent& event);
  void OnListDevices(wxCommandEvent& event);
  void OnMonitor(wxCommandEvent& event);
  void OnExit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);

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
