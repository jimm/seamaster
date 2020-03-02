#ifndef SET_LIST_LIST_BOX_H
#define SET_LIST_LIST_BOX_H

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
 #include <wx/wx.h>
#endif

class SetListListBox : public wxListBox {
public:
  SetListListBox(wxWindow *parent, wxWindowID id, wxSize size);

  void update();
  void jump();
};

#endif /* SET_LIST_LIST_BOX_H */
