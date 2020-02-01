#ifndef SONG_LIST_LIST_BOX_H
#define SONG_LIST_LIST_BOX_H

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
 #include <wx/wx.h>
#endif

class SongListListBox : public wxListBox {
public:
  SongListListBox(wxWindow *parent, wxWindowID id, wxSize size);

  void update();
  void jump();

private:
  bool initialized;
};

#endif /* SONG_LIST_LIST_BOX_H */
