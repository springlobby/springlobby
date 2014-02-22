/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_UPDATER_H_INCLUDED
#define SPRINGLOBBY_UPDATER_H_INCLUDED

#include <wx/event.h>
#include <downloader/httpdownloader.h>

class UpdaterClass : public wxEvtHandler
{
  public:
    UpdaterClass();
    ~UpdaterClass();
    void CheckForUpdates();

    bool StartUpdate( const wxString& latestVersion, const wxString& exe_to_update );
    void OnDownloadEvent( wxCommandEvent& event );

protected:

    bool UpdateExe( const wxString& newexe, bool WaitForReboot );
    bool UpdateLocale( const wxString& newdir, bool WaitForReboot );

    bool PostMinGW44( const wxString& newdir );

    wxString m_newexe;
    wxString m_currentexe;
    wxString m_latest_version;

    HttpDownloaderThread<UpdaterClass>* m_http_thread;

    DECLARE_EVENT_TABLE()
};

UpdaterClass& Updater();

#endif // SPRINGLOBBY_GLOBALEVENTS_H_INCLUDED
