#ifndef SPRINGLOBBY_GLOBALEVENTS_H_INCLUDED
#define SPRINGLOBBY_GLOBALEVENTS_H_INCLUDED

#include <wx/event.h>
class wxCommandEvent;


extern const wxEventType httpDownloadEvtComplete;
extern const wxEventType httpDownloadEvtFailed;
extern const wxEventType UnitSyncReloadRequest;


class SL_GlobalEvtHandler : public wxEvtHandler
{
protected:
    SL_GlobalEvtHandler();

    DECLARE_EVENT_TABLE()

public:
    static SL_GlobalEvtHandler& GetSL_GlobalEvtHandler();
    void OnHttpDownLoadComplete(wxCommandEvent& event);
    void OnHttpDownLoadFailed(wxCommandEvent& event);
    void OnUnitSyncReload(wxCommandEvent& event);

};

#endif // SPRINGLOBBY_GLOBALEVENTS_H_INCLUDED
