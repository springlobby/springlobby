#ifndef SPRINGLOBBY_UPDATER_H_INCLUDED
#define SPRINGLOBBY_UPDATER_H_INCLUDED

class ExeDownloader;
#include <wx/string.h>

class UpdaterClass
{
  public:
    UpdaterClass();
    ~UpdaterClass();
    void CheckForUpdates();
    void OnDownloadEvent( int code );

protected:

    bool UpdateExe( const wxString& newexe, bool WaitForReboot );
    bool UpdateLocale( const wxString& newdir, bool WaitForReboot );

    wxString m_newexe;
    ExeDownloader* m_exedownloader;

};

UpdaterClass& Updater();

#endif // SPRINGLOBBY_GLOBALEVENTS_H_INCLUDED
