#ifndef SET_LIST_EDITOR_H
#define SET_LIST_EDITOR_H

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
 #include <wx/wx.h>
#endif

class PatchMaster;
class SetList;
class wxListCtrl;

class SetListEditor : public wxDialog {
public:
  SetListEditor(wxWindow *parent, SetList *set_list);

private:
  PatchMaster *pm;
  SetList *set_list;
  wxListBox *all_songs_wxlist;
  wxListBox *set_list_wxlist;

  wxWindow *make_all_songs_panel(wxPanel *parent);
  wxWindow *make_set_list_panel(wxPanel *parent);
  wxWindow *make_panel(wxPanel *parent, const char * const title,
                       SetList *slist, wxListBox **list_ptr);

  void done(wxCommandEvent& event);

  wxDECLARE_EVENT_TABLE();
};

#endif /* SET_LIST_EDITOR_H */
