/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "updatehelper.h"

#include "utils/version.h"
#include "utils/conversion.h"

std::string GetDownloadUrl( const std::string& version )
{
    return "springlobby.info/windows/springlobby-" + version + "-win32.zip";
}

std::string GetSpringLobbyVersion( bool consider_aux )
{
	if ( consider_aux )
	{
		return STD_STRING((TowxString(getSpringlobbyVersion())).BeforeFirst( _T(' ') )) + " " + GetSpringLobbyAuxVersion();
	}
	else
	{
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
