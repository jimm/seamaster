#ifndef WX_APP_H
#define WX_APP_H

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
 #include <wx/wx.h>
#endif

class Frame;

class App: public wxApp {
public:
  App();
  ~App();

  void show_message(std::string);
  void clear_message();
  void clear_message_after(int);
  int clear_message_seconds() { return clear_msg_secs; }
  int clear_message_id() { return clear_msg_id; }

private:
  Frame *frame;
  char prev_cmd;
  int clear_msg_secs;
  int clear_msg_id;

  virtual bool OnInit();
  virtual int OnExit();
  virtual int FilterEvent(wxEvent &event);
};

App *app_instance();

#endif /* WX_APP_H */
