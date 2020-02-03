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

  void show_message(std::string msg);
  void show_message(std::string msg, int clear_secs);

private:
  Frame *frame;
  wxString command_line_path;

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
