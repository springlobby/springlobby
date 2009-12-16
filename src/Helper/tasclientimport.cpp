#include "tasclientimport.h"
#include <wx/platform.h>

#ifdef __WXMSW__

#include <wx/filefn.h>
#include <wx/filename.h>
#include <wx/tokenzr.h>
#include <wx/log.h>

#include "../settings.h"

#include <wx/msw/registry.h>

template < typename T >
T GetVal( const wxRegKey& reg, const wxString& name, const T def )
{
    T val = def;
    if ( reg.QueryValue( name, &val ) )
        return val;
    else
        return def;
}

template < >
wxString GetVal( const wxRegKey& reg, const wxString& name, const wxString def )
{
    wxString val = def;
    if ( reg.QueryValue( name, val ) )
        return val;
    else
        return def;
}

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
    fname.Replace( _T("SpringLobby") , wxEmptyString );

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

    sett().SetAutoConnect( GetVal( base, _T("ConnectOnStartup"), (long) sett().GetAutoConnect() ) );
    sett().SetChatPMSoundNotificationEnabled( GetVal( base, _T("DisableAllSounds"), (long) sett().GetChatPMSoundNotificationEnabled() ) );
    sett().SetServerAccountPass( sett().GetDefaultServer(),  GetVal( base, _T("Password"), sett().GetServerAccountPass( sett().GetDefaultServer() ) ) );
    sett().SetServerAccountSavePass( sett().GetDefaultServer() ,  GetVal( base, _T("RememberPasswords"), (long) sett().GetServerAccountSavePass( sett().GetDefaultServer() ) ) );
    sett().SetServerAccountNick( sett().GetDefaultServer()  , GetVal( base, _T("Username"), sett().GetServerAccountNick( sett().GetDefaultServer()  ) ) );

    bool ret = ImportAutojoins();

    sett().SaveSettings();

    return ret;

}

bool TASClientPresent()
{
    wxRegKey base( _T("HKEY_CURRENT_USER\\Software\\TASClient\\Preferences") );
    return base.Exists();
}
#endif
