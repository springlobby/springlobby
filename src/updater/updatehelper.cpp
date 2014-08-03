/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "updatehelper.h"

#include "utils/conversion.h"
#include "utils/slconfig.h"

SLCONFIG("/General/DownloadUrl", "http://springlobby.info/windows/springlobby-%s-win32.zip", "Url for downloading a springlobby update, %s is replaced with the version");

std::string GetDownloadUrl( const std::string& version )
{
	wxString url = cfg().ReadString(_T("/General/DownloadUrl"));
	url.Replace(wxT("%s"), TowxString(version));
	return STD_STRING(url);
}
