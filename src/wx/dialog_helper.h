#ifndef DIALOG_HELPER_H
#define DIALOG_HELPER_H

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
 #include <wx/wx.h>
#endif

class DialogHelper {
protected:
  wxSizer *make_ok_cancel_buttons(wxDialog *parent);
};

#endif /* DIALOG_HELPER_H */
