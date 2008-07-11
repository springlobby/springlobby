#include "globalevents.h"
#include "settings++/custom_dialogs.h"
#include "ui.h"
#include <wx/intl.h>

const wxEventType httpDownloadEvtComplete = wxNewEventType();
const wxEventType httpDownloadEvtFailed = wxNewEventType();
const wxEventType UnitSyncReloadRequest = wxNewEventType();

BEGIN_EVENT_TABLE(SL_GlobalEvtHandler, wxEvtHandler)
    EVT_COMMAND(wxID_ANY, httpDownloadEvtComplete,  SL_GlobalEvtHandler::OnHttpDownLoadComplete)
    EVT_COMMAND(wxID_ANY, httpDownloadEvtFailed,    SL_GlobalEvtHandler::OnHttpDownLoadFailed)
    EVT_COMMAND(wxID_ANY, UnitSyncReloadRequest,    SL_GlobalEvtHandler::OnUnitSyncReload)
END_EVENT_TABLE()

SL_GlobalEvtHandler& SL_GlobalEvtHandler::GetSL_GlobalEvtHandler()
{
    static SL_GlobalEvtHandler slGlobalEvtHandler;
    return slGlobalEvtHandler;
}

SL_GlobalEvtHandler::SL_GlobalEvtHandler()
{
}

void SL_GlobalEvtHandler::OnHttpDownLoadComplete(wxCommandEvent& event)
{
    customMessageBoxNoModal(SL_MAIN_ICON,event.GetString(),_("Download complete") );
}

void SL_GlobalEvtHandler::OnHttpDownLoadFailed(wxCommandEvent& event)
{
    customMessageBoxNoModal(SL_MAIN_ICON,event.GetString(),_("Download failed") );
}


void SL_GlobalEvtHandler::OnUnitSyncReload(wxCommandEvent& event)
{
    ui().ReloadUnitSync();
}
