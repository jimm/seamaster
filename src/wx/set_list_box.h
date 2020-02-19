#ifndef SET_LIST_BOX_H
#define SET_LIST_BOX_H

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
 #include <wx/wx.h>
#endif

class SetList;

class SetListBox : public wxListBox {
public:
  SetListBox(wxWindow *parent, wxWindowID id, wxSize size);

  void update();
  void jump();

private:
  SetList *set_list;
};

#endif /* SET_LIST_BOX_H */
