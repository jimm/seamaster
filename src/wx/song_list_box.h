#ifndef SONG_LIST_BOX_H
#define SONG_LIST_BOX_H

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
 #include <wx/wx.h>
#endif

class SongList;

class SongListBox : public wxListBox {
public:
  SongListBox(wxWindow *parent, wxWindowID id, wxSize size);

  void update();
  void jump();

private:
  SongList *song_list;
};

#endif /* SONG_LIST_BOX_H */
