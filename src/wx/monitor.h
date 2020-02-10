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
  int input_list_len;
  int output_list_len;

  wxWindow *make_input_panel(wxPanel *parent);
  wxWindow *make_output_panel(wxPanel *parent);
  void add_message(Instrument *, wxListCtrl *list, int row, PmMessage msg);
};

#endif /* MONITOR_H */
