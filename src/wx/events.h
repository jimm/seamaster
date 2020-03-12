#ifndef EVENTS_H
#define EVENTS_H

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
 #include <wx/wx.h>
#endif

wxDECLARE_EVENT(Frame_Refresh, wxCommandEvent);
wxDECLARE_EVENT(Frame_MenuUpdate, wxCommandEvent);
wxDECLARE_EVENT(Connection_Refresh, wxCommandEvent);


#endif /* EVENTS_H */