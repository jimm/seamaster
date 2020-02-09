#ifndef INSTRUMENT_DIALOG_H
#define INSTRUMENT_DIALOG_H

#include <vector>
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
 #include <wx/wx.h>
#endif

using namespace std;

class PatchMaster;
class Instrument;

class InstrumentDialog : public wxDialog {
public:
  InstrumentDialog(wxWindow *parent, PatchMaster *pm);
  void run();

private:
  PatchMaster *pm;

  void add_instrument(wxListCtrl *list_box, Instrument *inst, int i);
};

#endif /* INSTRUMENT_DIALOG_H */