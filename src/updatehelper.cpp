/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "updatehelper.h"

#include "httpfile.h"
#include "utils/conversion.h"
#include "utils/slconfig.h"
#include "utils/version.h"
#include <time.h>

SLCONFIG("/General/LastUpdateCheck", 0L, "Last time springlobby checked for an update");
SLCONFIG("/General/UpdateChannel", "release", "update channel to use (release or develop)");
#define VERSION_CHECK_INTERVAL 1*60*60

static bool isReleaseChannel()
{
	const wxString channel = cfg().ReadString(_T("/General/UpdateChannel"));
	return channel != _T("develop"); //typos will make use release channel
}

std::string GetDownloadUrl(const std::string& version)
{
	if (isReleaseChannel()) {
		return stdprintf("https://springlobby.info/windows/springlobby-%s-win32.zip", version.c_str());
	}
	return "https://springlobby.info/temp/builds/develop/sl_master.zip";
}

std::string GetLatestVersionUrl()
{
	if (isReleaseChannel()) {
		return std::string("https://springlobby.info/version/current.txt");
	}
	return std::string("https://springlobby.info/temp/builds/develop/current.txt");
}

static time_t GetTime()
{
	time_t now = time(nullptr);
	if (static_cast<time_t>(-1) == now) {
		std::string msg = "time() broke: ";
		msg += strerror (errno);
		throw std::runtime_error (msg.c_str());
	}
	return now;
}

std::string GetLatestVersion(bool use_cached)
{
	static std::string latest_version = GetSpringlobbyVersion();
	const time_t now = GetTime();
	const time_t last_check_time = cfg().ReadLong(_T("/General/LastUpdateCheck"));

	if (latest_version.empty() || !use_cached || now > (last_check_time + VERSION_CHECK_INTERVAL)) {
		latest_version = GetHttpFile(GetLatestVersionUrl());
		latest_version = STD_STRING(TowxString(latest_version).Trim().Trim(false));
		cfg().Write(_T("/General/LastUpdateCheck"), (long)now);
	}
	return latest_version;
}
