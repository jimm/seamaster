#ifndef WX_APP_H
#define WX_APP_H

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
 #include <wx/wx.h>
#endif
#include "../patchmaster.h"

class App: public wxApp {
public:
  App();
  ~App();

  virtual bool OnInit();

  void show_message(string);
  void clear_message();
  void clear_message_after(int);
  int clear_message_seconds() { return clear_msg_secs; }
  int clear_message_id() { return clear_msg_id; }

private:
  wxFrame *frame;
  PatchMaster *pm;
  int clear_msg_secs;
  int clear_msg_id;
};

App *app_instance();

#endif /* WX_APP_H */