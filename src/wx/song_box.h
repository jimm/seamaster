#ifndef SONG_BOX_H
#define SONG_BOX_H

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
 #include <wx/wx.h>
#endif

class Song;

class SongBox : public wxListBox {
public:
  SongBox(wxWindow *parent, wxWindowID id, wxSize size);

  void update();
  void jump();

private:
  Song *song;
};

#endif /* SONG_BOX_H */
