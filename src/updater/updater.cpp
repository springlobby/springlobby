#include "updater.h"
#include "updatehelper.h"
#include "versionchecker.h"
#include "../utils/customdialogs.h"
#include "../utils/platform.h"
#include "../utils/globalevents.h"
#include "../settings.h"
#include <lslutils/globalsmanager.h>
#include "../uiutils.h"

#include <wx/stdpaths.h>
#include <wx/filefn.h>
#include <wx/filename.h>
#include <limits>

BEGIN_EVENT_TABLE(UpdaterClass, wxEvtHandler)
    EVT_COMMAND(wxID_ANY, httpDownloadEvtComplete,  UpdaterClass::OnDownloadEvent)
    EVT_COMMAND(wxID_ANY, httpDownloadEvtFailed,    UpdaterClass::OnDownloadEvent)
END_EVENT_TABLE()

UpdaterClass& Updater()
{
    static LSL::Util::LineInfo<UpdaterClass> m( AT );
    static LSL::Util::GlobalObjectHolder<UpdaterClass, LSL::Util::LineInfo<UpdaterClass> > m_upd( m );
    return m_upd;
}

UpdaterClass::UpdaterClass():
	wxEvtHandler(),
	m_http_thread( 0 )
{
}

UpdaterClass::~UpdaterClass()
{
    delete m_http_thread;
}

bool UpdaterClass::StartUpdate( const wxString& latestVersion, const wxString& exe_to_update )
{
    wxString sep = wxFileName::GetPathSeparator();
    m_latest_version = latestVersion;
    m_currentexe = exe_to_update;
    if ( !wxFileName::IsDirWritable( wxPathOnly( m_currentexe ) ) ) {
        wxLogError( _T("dir not writable: ") + m_currentexe );
        customMessageBox(SL_MAIN_ICON, _("Unable to write to the lobby installation directory.\nPlease update manually or enable write permissions for the current user."), _("Error"));
        return false;
    }
    m_newexe = sett().GetLobbyWriteDir() + _T("update") + sep;
    wxLogError( m_newexe  );
    if ( !wxDirExists( m_newexe ) ) {
        if ( !wxMkdir( m_newexe ) ){
            wxLogError( _T("couldn't create update directory") );
            return false;
        }
    }

    m_http_thread = new HttpDownloaderThread<UpdaterClass>( GetDownloadUrl( m_latest_version ), m_newexe + _T("temp.zip"), *this, wxNewId(), true, true );

    //could prolly use some test on the thread here instead
    return true;
}


//all messageboxes need to be modal, updater closes immeadiately when receiving the UpdateFinished event
void UpdaterClass::OnDownloadEvent( wxCommandEvent& event )
{
    int code = event.GetInt();
    if ( code != 0)
        customMessageBox(SL_MAIN_ICON, _("There was an error downloading for the latest version.\nPlease try again later.\nIf the problem persists, please use Help->Report Bug to report this bug."), _("Error"));
    else {
        if (!PostMinGW44( m_newexe ) ) {
            customMessageBox(SL_MAIN_ICON, _("Automatic update failed\n\nyou will be redirected to a web page with instructions and the download link will be opened in your browser."), _("Updater error.") );
			OpenWebBrowser( _T("http://projects.springlobby.info/wiki/springlobby/Install#Windows-Binary") );
            OpenWebBrowser( GetDownloadUrl( m_latest_version ) );
        }
        if ( !UpdateExe( m_newexe , false ) ) {
			customMessageBox(SL_MAIN_ICON,
							 wxFormat( _("There was an error while trying to replace the current executable version.\n Please manually copy springlobby.exe from: %s\n to: %s\n") )
											   % m_newexe
											   % m_currentexe,
							 _("Error") );
			GlobalEvent::Send( GlobalEvent::OnUpdateFinished );
        }
        else {
            bool locale_ok = UpdateLocale( m_newexe, false );
            if ( locale_ok ) {
				customMessageBox(SL_MAIN_ICON, IdentityString( _("Update complete. \nPlease restart %s now.") ), _("Success"));
            }
            else {
				customMessageBox(SL_MAIN_ICON,
								 IdentityString( _("Binary updated successfully. \nSome translation files could not be updated.\nPlease report this in #springlobby. \nPlease restart %s now.") ),
								 _("Partial success") );
            }
            wxRmdir( m_newexe );
        }
    }
    GlobalEvent::Send( GlobalEvent::OnUpdateFinished );
}

//! DO NOT use mw() global unless fromCli is false !
bool UpdaterClass::UpdateLocale( const wxString& tempdir, bool /*unused*/ ) {
    wxString target = wxPathOnly( wxStandardPaths::Get().GetExecutablePath() ) + wxFileName::GetPathSeparator() + _T("locale");
    wxString origin = tempdir + _T("locale") + wxFileName::GetPathSeparator() ;
    return CopyDir( origin, target );
}

//! DO NOT use mw() global unless fromCli is false !
bool UpdaterClass::UpdateExe( const wxString& newexe, bool /*unused*/ ) {
    //this always returns false on msw
//  if ( !wxFileName::IsFileExecutable( newexe + _T("springlobby.exe") ) )
//  {
//      customMessageBoxNoModal(SL_MAIN_ICON, _("not exe."), _("Error"));
//      return false;
//  }

    wxString backupfile =  m_currentexe + _T(".bak");
    wxRemoveFile( backupfile );
    if ( !wxRenameFile( m_currentexe, backupfile ) )
        return false;

    if ( !wxCopyFile( newexe + _T("springlobby.exe"), m_currentexe ) ) {
        wxRenameFile(  backupfile, m_currentexe.AfterLast( wxFileName::GetPathSeparator() )  ); //restore original file from backup on update failure
        return false;
    }

    return true;
}

bool UpdaterClass::PostMinGW44( const wxString& newdir )
{
	wxString current = GetSpringLobbyVersion(false);
    long minor = std::numeric_limits<long>::max();
    bool convert_ok = current.AfterLast( '.' ).ToLong( &minor );
	wxLogMessage( (wxFormat( _T("Got minor rev %d") ) % minor ).c_str() );
    if ( minor > 43 || !convert_ok   ) //0.43 was the last build on old mingw
        return true; //all is well, we're already on a mingw4.4 build
    wxString base = wxPathOnly( wxStandardPaths::Get().GetExecutablePath() ) + wxFileName::GetPathSeparator() ;

    //use special copydir to overwrite locked files
    bool success = CopyDirWithFilebackupRename( newdir + wxFileName::GetPathSeparator(), base  );
    if ( !success ) {
        wxLogError( _T("Full dir copy failed") );
    }
    return success;
}
