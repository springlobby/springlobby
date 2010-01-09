#include "updatehelper.h"

#include "../utils/conversion.h"

wxString GetDownloadUrl( const wxString& version )
{
    return _T("springlobby.info/windows/springlobby-") + version + _T("-win32.zip");
}

wxString GetSpringLobbyVersion()
{
#ifndef AUX_VERSION
    return (TowxString(VERSION)).BeforeFirst( _T(' ') );
#else
	return (TowxString(VERSION)).BeforeFirst( _T(' ') ) + _T(" ") + TowxString(AUX_VERSION);
#endif

}
