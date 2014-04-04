/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "updatehelper.h"

#include "utils/version.h"
#include "utils/conversion.h"
#include "helper/slconfig.h"

SLCONFIG("/General/DownloadUrl", "springlobby.info/windows/springlobby-%s-win32.zip", "Url for downloading a springlobby update, %s is replaced with the version");

std::string GetDownloadUrl( const std::string& version )
{
	wxString url = cfg().ReadString(_T("/General/DownloadUrl"));
	url.Replace(wxT("%s"), TowxString(version));
	return STD_STRING(url);
}

std::string GetSpringLobbyVersion( bool consider_aux )
{
	if ( consider_aux ) {
		return STD_STRING((TowxString(getSpringlobbyVersion())).BeforeFirst( _T(' ') )) + " " + GetSpringLobbyAuxVersion();
	} else {
		return STD_STRING((TowxString(getSpringlobbyVersion())).BeforeFirst( _T(' ') ));
	}

}

std::string GetSpringLobbyAuxVersion()
{
#ifndef AUX_VERSION
	return "";
#else
	return AUX_VERSION;
#endif
}
