/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "updatehelper.h"

#include "utils/conversion.h"
#include "utils/slconfig.h"

SLCONFIG("/General/UpdateChannel", "release", "update channel to use (release or develop)");

static bool isReleaseChannel()
{
	const wxString channel = cfg().ReadString(_T("/General/UpdateChannel"));
	return channel != _T("develop"); //typos will make use release channel
}

std::string GetDownloadUrl(const std::string& version)
{
	if (isReleaseChannel()) {
		return stdprintf("http://springlobby.info/windows/springlobby-%s-win32.zip", version.c_str());
	}
	return "http://springlobby.info/temp/builds/abma/sl_master.zip";
}

std::string GetLatestVersionUrl()
{
	if (isReleaseChannel()) {
		return std::string("http://version.springlobby.info/current.txt");
	}
	return std::string("http://springlobby.info/temp/builds/abma/current.txt");
}
