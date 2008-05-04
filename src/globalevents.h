#ifndef SPRINGLOBBY_GLOBALEVENTS_H_INCLUDED
#define SPRINGLOBBY_GLOBALEVENTS_H_INCLUDED

#include <wx/event.h>
class wxCommandEvent;

DEFINE_EVENT_TYPE(httpDownloadEvtComplete)
DEFINE_EVENT_TYPE(httpDownloadEvtFailed)

BEGIN_DECLARE_EVENT_TYPES()
DECLARE_EVENT_TYPE(httpDownloadEvtComplete, 42) // second argument is required for wx 2.0.x compability only
DECLARE_EVENT_TYPE(httpDownloadEvtFailed, 42)
END_DECLARE_EVENT_TYPES()


class SL_GlobalEvtHandler : public wxEvtHandler
{
protected:
    SL_GlobalEvtHandler();

    DECLARE_EVENT_TABLE()

public:
    static SL_GlobalEvtHandler& GetSL_GlobalEvtHandler();
    void OnHttpDownLoadComplete(wxCommandEvent& event);
    void OnHttpDownLoadFailed(wxCommandEvent& event);

};

#endif // SPRINGLOBBY_GLOBALEVENTS_H_INCLUDED
