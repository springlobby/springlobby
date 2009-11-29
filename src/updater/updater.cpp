
#include "updater.h"
#include "versionchecker.h"
#include "../utils/customdialogs.h"
#include "../utils/platform.h"
#include "../utils/globalevents.h"
#include "../settings.h"
#include "../globalsmanager.h"
#include "../ui.h"
#include "../mainwindow.h"
#include "../httpdownloader.h"
#include "../uiutils.h"

#include <wx/stdpaths.h>
#include <wx/filefn.h>
#include <wx/filename.h>


BEGIN_EVENT_TABLE(UpdaterClass, wxEvtHandler)
    EVT_COMMAND(wxID_ANY, httpDownloadEvtComplete,  UpdaterClass::OnDownloadEvent)
    EVT_COMMAND(wxID_ANY, httpDownloadEvtFailed,    UpdaterClass::OnDownloadEvent)
END_EVENT_TABLE()

UpdaterClass& Updater()
{
    static GlobalObjectHolder<UpdaterClass> m_upd;
    return m_upd;
}


UpdaterClass::UpdaterClass()
{
}


UpdaterClass::~UpdaterClass()
{
}

#ifdef __WXMSW__
void UpdaterClass::StartUpdate( const wxString& latestVersion )
{
    wxString sep = wxFileName::GetPathSeparator();
    m_latest_version = latestVersion;
    wxString currentexe = wxStandardPaths::Get().GetExecutablePath();
    if ( !wxFileName::IsDirWritable( currentexe.BeforeLast( wxFileName::GetPathSeparator() ) + wxFileName::GetPathSeparator() ) )
    {
      customMessageBoxNoModal(SL_MAIN_ICON, _("Unable to write to the lobby installation directory.\nPlease update manually or enable write permissions for the current user."), _("Error"));
      return;
    }
    m_newexe = sett().GetLobbyWriteDir() + _T("update") + sep;
    if ( !wxDirExists( m_newexe ) )
        wxMkdir( m_newexe );
    new HttpDownloaderThread<UpdaterClass>( GetDownloadUrl(), m_newexe + _T("temp.zip"), *this, wxID_HIGHEST + 10000, true, true );
}
#endif

wxString UpdaterClass::GetDownloadUrl()
{
	return _T("springlobby.info/windows/springlobby-") + m_latest_version + _T("-win32.zip");
}

//all messageboxes need to be modal, updater closes immeadiately when receiving the UpdateFinished event
void UpdaterClass::OnDownloadEvent( wxCommandEvent& event )
{
	int code = event.GetInt();
  if ( code != 0)
    customMessageBox(SL_MAIN_ICON, _("There was an error downloading for the latest version.\nPlease try again later.\nIf the problem persists, please use Help->Report Bug to report this bug."), _("Error"));
  else
  {
    if ( !UpdateExe( m_newexe , false ) ) {
            customMessageBox(SL_MAIN_ICON, wxString::Format( _("There was an error while trying to replace the current executable version.\n Please manually copy springlobby.exe from: %s\n to: %s\nPlease use Help->Report Bug to report this bug."), m_newexe.c_str(), wxStandardPaths::Get().GetExecutablePath().c_str() ), _("Error"));
    }
    else
    {
        bool locale_ok = UpdateLocale( m_newexe, false );
        if ( locale_ok ) {
                customMessageBox(SL_MAIN_ICON, _("Update complete. The changes will be available next lobby start."), _("Success"));
        }
        else {
                customMessageBox(SL_MAIN_ICON, _("Binary updated successfully. \nSome translation files could not be updated.\nPlease report this in #springlobby after restarting."), _("Partial success"));
        }
        wxRmdir( m_newexe );
        GetGlobalEventSender( GlobalEvents::UpdateFinished ).SendEvent( 0 );
    }
  }
}

//! DO NOT use mw() global unless fromCli is false !
bool UpdaterClass::UpdateLocale( const wxString& tempdir, bool /*unused*/ )
{
    wxString target = wxPathOnly( wxStandardPaths::Get().GetExecutablePath() ) + wxFileName::GetPathSeparator() + _T("locale");
    wxString origin = tempdir + _T("locale") + wxFileName::GetPathSeparator() ;
    return CopyDir( origin, target );
}

//! DO NOT use mw() global unless fromCli is false !
bool UpdaterClass::UpdateExe( const wxString& newexe, bool /*unused*/ )
{
    //this always returns false on msw
//  if ( !wxFileName::IsFileExecutable( newexe + _T("springlobby.exe") ) )
//  {
//      customMessageBoxNoModal(SL_MAIN_ICON, _("not exe."), _("Error"));
//      return false;
//  }
  wxString currentexe = wxStandardPaths::Get().GetExecutablePath();

  wxString backupfile =  currentexe + _T(".bak");
  wxRemoveFile( backupfile );
  if ( !wxRenameFile( currentexe, backupfile ) )
    return false;

  if ( !wxCopyFile( newexe + _T("springlobby.exe"), currentexe ) )
  {
    wxRenameFile(  backupfile, currentexe.AfterLast( wxFileName::GetPathSeparator() )  ); //restore original file from backup on update failure
    return false;
  }

  return true;
}
