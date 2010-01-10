#include "updatehelper.h"

#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif //HAVE_CONFIG_H

#ifndef VERSION
	#define VERSION "unknown"
#endif //VERSION

#include "../utils/conversion.h"

wxString GetDownloadUrl( const wxString& version )
{
    return _T("springlobby.info/windows/springlobby-") + version + _T("-win32.zip");
}

wxString GetSpringLobbyVersion( bool consider_aux )
{
	if ( consider_aux )
	{
		return (TowxString(VERSION)).BeforeFirst( _T(' ') ) + _T(" ") + GetSpringLobbyAuxVersion();
	}
	else
	{
		return (TowxString(VERSION)).BeforeFirst( _T(' ') );
	}

}

wxString GetSpringLobbyAuxVersion()
{
#ifndef AUX_VERSION
	return wxEmptyString;
#else
	return TowxString(AUX_VERSION);
#endif
}
