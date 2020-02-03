#ifndef WX_APP_H
#define WX_APP_H

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
 #include <wx/wx.h>
#endif
#include <portmidi.h>

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
  wxString command_line_path;
  char prev_cmd;
  int clear_msg_secs;
  int clear_msg_id;

  virtual bool OnInit();
  virtual void OnInitCmdLine(wxCmdLineParser &);
  virtual bool OnCmdLineParsed(wxCmdLineParser &);
  virtual int OnExit();
  virtual int FilterEvent(wxEvent &event);

  void init_portmidi();
  void list_all_devices();
  void list_devices(const char *title, const PmDeviceInfo *infos[], int num_devices);
};

App *app_instance();

#endif /* WX_APP_H */
