#ifndef MESSAGE_BOX_H
#define MESSAGE_BOX_H

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
 #include <wx/wx.h>
#endif

class Message;

class MessageList : public wxListBox {
public:
  MessageList(wxWindow *parent, wxWindowID id, wxSize size);

  void update();

private:
  Message *message;
};

#endif /* MESSAGE_BOX_H */
