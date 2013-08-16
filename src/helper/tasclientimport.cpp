#include "tasclientimport.h"
#include <wx/platform.h>

#ifdef __WXMSW__

#include <wx/filefn.h>
#include <wx/filename.h>
#include <wx/tokenzr.h>
#include <wx/log.h>

#include "../settings.h"
#include "../utils/platform.h"

#include <wx/msw/registry.h>

wxString GetChan( const wxString& token )
{
    wxString ret = token.AfterFirst('#');
    ret = ret.Left( ret.Find( ' ' ) );
    return ret;
}

bool ImportAutojoins()
{
    wxString sep = wxFileName::GetPathSeparator();
    wxString fname = sett().GetLobbyWriteDir();
	fname.Replace( GetAppName() , wxEmptyString );

    fname = fname + sep + _T("var") + sep + _T("perform.dat");
    if ( wxFileName::FileExists( fname ) ) {
        wxTextFile file( fname );
        wxLogWarning( _T("parsing file: ") + fname );
        file.Open();
        for ( wxString line = file.GetFirstLine(); !file.Eof(); line = file.GetNextLine() ) {
            wxString chan = GetChan( line );
            sett().AddChannelJoin( chan, _T("") );
        }
        return true;
    }
    else {
        wxLogError( _T("Not found: ") + fname );
        return false;
    }
}

bool ImportTASClientSettings()
{
    wxRegKey base( _T("HKEY_CURRENT_USER\\Software\\TASClient\\Preferences") );

	sett().SetAutoConnect( GetRegkeyVal( base, _T("ConnectOnStartup"), (long) sett().GetAutoConnect() ) );
	sett().SetChatPMSoundNotificationEnabled( GetRegkeyVal( base, _T("DisableAllSounds"), (long) sett().GetChatPMSoundNotificationEnabled() ) );
	sett().SetServerAccountPass( sett().GetDefaultServer(),  GetRegkeyVal( base, _T("Password"), sett().GetServerAccountPass( sett().GetDefaultServer() ) ) );
	sett().SetServerAccountSavePass( sett().GetDefaultServer() ,  GetRegkeyVal( base, _T("RememberPasswords"), (long) sett().GetServerAccountSavePass( sett().GetDefaultServer() ) ) );
	sett().SetServerAccountNick( sett().GetDefaultServer()  , GetRegkeyVal( base, _T("Username"), sett().GetServerAccountNick( sett().GetDefaultServer()  ) ) );

    bool ret = ImportAutojoins();

    sett().SaveSettings();

    return ret;

}

bool TASClientPresent()
{
	return wxRegKey( _T("HKEY_CURRENT_USER\\Software\\TASClient\\Preferences") ).Exists();
}
#endif
