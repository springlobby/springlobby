#include "versionchecker.h"
#include "../settings++/custom_dialogs.h"
#include "../utils.h"
#include "exedownloader.h"
#include "updater.h"
#include "../settings.h"

#include <wx/stdpaths.h>
#include <wx/filefn.h>
#include <wx/filename.h>

UpdaterClass& Updater()
{
    static UpdaterClass m_upd;
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
  wxString myVersion = GetSpringLobbyVersion();

  wxString msg = _("Your Version: ") + myVersion + _T("\n") + _("Latest Version: ") + latestVersion;

  if ( !latestVersion.IsSameAs(myVersion, false) )
  {
    int answer = customMessageBox(SL_MAIN_ICON, _("Your SpringLobby version is not up to date.\n\n") + msg + _("\n\nWould you like for me to autodownload the new version? It will be automatically used next time you launch the lobby again."), _("Not up to Date"), wxYES_NO);
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
  }
}

void UpdaterClass::OnDownloadEvent( int code )
{
  if ( code != 0) customMessageBox(SL_MAIN_ICON, _("There was an error downloading for the latest version.\nPlease try again later.\nIf the problem persists, please use Help->Report Bug to report this bug."), _("Error"));
  else
  {
    if ( !UpdateExe( m_newexe , false ) )  customMessageBoxNoModal(SL_MAIN_ICON, _("There was an error while trying to replace the current executable version\n manual copy is necessary from\n") + m_newexe + _("to\n") + wxStandardPaths::Get().GetExecutablePath() +  _("\nPlease use Help->Report Bug to report this bug."), _("Error"));
    else
    {
        wxRmdir( m_newexe );
        customMessageBoxNoModal(SL_MAIN_ICON, _("Update complete. Please restart SpringLobby for it to take effect."), _("Success"));
    }
  }
}


bool UpdaterClass::UpdateExe( const wxString& newexe, bool WaitForReboot )
{
//  if ( !wxFileName::IsFileExecutable( newexe + _T("springlobby.exe") ) )
//  {
//      customMessageBoxNoModal(SL_MAIN_ICON, _("not exe."), _("Error"));
//      return false;
//  }
  wxString currentexe = wxStandardPaths::Get().GetExecutablePath();
  customMessageBox(SL_MAIN_ICON, currentexe , _T("currentexe ") );
  wxString backupfile =  currentexe + _T(".bak");
  if ( !wxRenameFile( currentexe, backupfile ) )
    return false;

  if ( !wxCopyFile( newexe + _T("springlobby.exe"), currentexe ) )
  {
    wxRenameFile( currentexe.BeforeFirst( wxFileName::GetPathSeparator() ) + wxFileName::GetPathSeparator()+ backupfile, _T("springlobby.exe") );
    return false;
  }
  //wxRemoveFile( backupfile );
  return true;
}
