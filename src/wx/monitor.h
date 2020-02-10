#ifndef MONITOR_H
#define MONITOR_H

#include <vector>
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
 #include <wx/wx.h>
#endif
#include "../midi_monitor.h"

using namespace std;

class PatchMaster;
class Instrument;

class Monitor : public wxFrame, public MIDIMonitor {
public:
  Monitor();
  virtual ~Monitor();
  void run();

  virtual void monitor_input(Input *input, PmMessage msg);
  virtual void monitor_output(Output *output, PmMessage msg);

private:
  wxListCtrl *input_list;
  wxListCtrl *output_list;
  vector<PmMessage> input_messages;
  vector<PmMessage> output_messages;

  wxWindow *make_input_panel(wxPanel *parent);
  wxWindow *make_output_panel(wxPanel *parent);
  wxWindow *make_panel(wxPanel *parent, const char * const title, wxListCtrl **list_ptr);
  void add_message(Instrument *inst, wxListCtrl *list, PmMessage msg, vector<PmMessage> &message_list);
};

#endif /* MONITOR_H */
