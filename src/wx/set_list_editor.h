#ifndef SET_LIST_EDITOR_H
#define SET_LIST_EDITOR_H

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
 #include <wx/wx.h>
#endif
#include "events.h"
#include "dialog_helper.h"

enum {
  ID_SLE_Name = 1000,
  ID_SLE_AllSongs,
  ID_SLE_SetList,
  ID_SLE_AddButton,
  ID_SLE_RemoveButton
};

class SeaMaster;
class SetList;
class wxListCtrl;

class SetListEditor : public wxDialog, DialogHelper {
public:
  SetListEditor(wxWindow *parent, SetList *set_list);

private:
  SeaMaster *pm;
  SetList *set_list;
  wxTextCtrl *name_text;
  wxListBox *all_songs_wxlist;
  wxListBox *set_list_wxlist;
  wxButton *add_button;
  wxButton *remove_button;

  wxWindow *make_name_panel(wxWindow *parent);
  wxWindow *make_all_songs_panel(wxWindow *parent);
  wxWindow *make_buttons(wxWindow *parent);
  wxWindow *make_set_list_panel(wxWindow *parent);
  wxWindow *make_panel(wxWindow *parent, wxWindowID id,
                       const char * const title,
                       SetList *slist, wxListBox **list_ptr);

  void set_name(wxCommandEvent& event);
  void all_songs_selection(wxCommandEvent& event);
  void set_list_selection(wxCommandEvent& event);
  void add_song(wxCommandEvent& event);
  void remove_song(wxCommandEvent& event);

  void update(wxListBox *list_box, SetList *set_list);
  void save(wxCommandEvent& _);

  wxDECLARE_EVENT_TABLE();
};

#endif /* SET_LIST_EDITOR_H */
