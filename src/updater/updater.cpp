#include "versionchecker.h"
#include "../settings++/custom_dialogs.h"
#include "../utils.h"
#include "exedownloader.h"
#include "updater.h"
#include "../settings.h"
#include "../globalsmanager.h"

#include <wx/stdpaths.h>
#include <wx/filefn.h>
#include <wx/filename.h>

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
  delete m_exedownloader;
}

void UpdaterClass::CheckForUpdates()
{
  wxString latestVersion = GetLatestVersion();
  // Need to replace crap chars or versions will always be inequal
  latestVersion.Replace(_T(" "), _T(""), true);
  latestVersion.Replace(_T("\n"), _T(""), true);
  latestVersion.Replace(_T("\t"), _T(""), true);
  if (latestVersion == _T("-1"))
  {
    customMessageBoxNoModal(SL_MAIN_ICON, _("There was an error checking for the latest version.\nPlease try again later.\nIf the problem persists, please use Help->Report Bug to report this bug."), _("Error"));
    return;
  }
  wxString myVersion = GetSpringLobbyVersion() ;

  wxString msg = _("Your Version: ") + myVersion + _T("\n") + _("Latest Version: ") + latestVersion;
  if ( !latestVersion.IsSameAs(myVersion, false) )
  {
      #ifdef __WXMSW__
      int answer = customMessageBox(SL_MAIN_ICON, _("Your SpringLobby version is not up to date.\n\n") + msg + _("\n\nWould you like for me to autodownload the new version? Changes will take effect next you launch the lobby again."), _("Not up to date"), wxYES_NO);
      if (answer == wxYES)
      {
        wxString sep = wxFileName::GetPathSeparator();
        wxString currentexe = wxStandardPaths::Get().GetExecutablePath();
        if ( !wxFileName::IsDirWritable( currentexe.BeforeLast( wxFileName::GetPathSeparator() ) + wxFileName::GetPathSeparator() ) )
        {
          customMessageBoxNoModal(SL_MAIN_ICON, _("Unable to write to the lobby installation directory.\nPlease update manually or enable write permissions for the current user."), _("Error"));
          return;
        }
        m_newexe = sett().GetLobbyWriteDir() + _T("update") + sep;
        wxMkdir( m_newexe );
        wxString url = _T("springlobby.info/windows/springlobby-") + latestVersion + _T("-win32.zip");
        m_exedownloader = new ExeDownloader( url, m_newexe + _T("temp.zip") );
      }
    #else
    customMessageBox(SL_MAIN_ICON, _("Your SpringLobby version is not up to date.\n\n") + msg, _("Not up to Date") );
    #endif
  }
}

void UpdaterClass::OnDownloadEvent( int code )
{
  if ( code != 0) customMessageBox(SL_MAIN_ICON, _("There was an error downloading for the latest version.\nPlease try again later.\nIf the problem persists, please use Help->Report Bug to report this bug."), _("Error"));
  else
  {
    if ( !UpdateExe( m_newexe , false ) )
        customMessageBoxNoModal(SL_MAIN_ICON, wxString::Format( _("There was an error while trying to replace the current executable version\n manual copy is necessary from: %s\n to: %s\nPlease use Help->Report Bug to report this bug."), m_newexe.c_str(), wxStandardPaths::Get().GetExecutablePath().c_str() ), _("Error"));
    else
    {
        bool locale_ok = UpdateLocale( m_newexe, false );
        if ( locale_ok ) {
            customMessageBoxNoModal(SL_MAIN_ICON, _("Update complete. The changes will be available next lobby start."), _("Success"));
        }
        else {
            customMessageBoxNoModal(SL_MAIN_ICON, _("Binary updated successfully. \nSome translation files could not be updated.\nPlease report this in #springlobby after restarting."), _("Partial success"));
        }
        wxRmdir( m_newexe );
    }
  }
}

bool UpdaterClass::UpdateLocale( const wxString& tempdir, bool WaitForReboot )
{
    wxString target = wxPathOnly( wxStandardPaths::Get().GetExecutablePath() ) + wxFileName::GetPathSeparator() + _T("locale");
    wxString origin = tempdir + _T("locale") + wxFileName::GetPathSeparator() ;
    return CopyDir( origin, target );
}

bool UpdaterClass::UpdateExe( const wxString& newexe, bool WaitForReboot )
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
