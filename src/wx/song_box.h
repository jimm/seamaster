#ifndef SONG_BOX_H
#define SONG_BOX_H

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
 #include <wx/wx.h>
#endif

class Song;

class SongBox : public wxListBox {
public:
  SongBox(wxWindow *parent, wxSize size);

  void update();

private:
  Song *song;
};

#endif /* SONG_BOX_H */
