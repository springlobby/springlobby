/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_UPDATEHELPER_H
#define SPRINGLOBBY_HEADERGUARD_UPDATEHELPER_H

#include <string>

std::string GetDownloadUrl(const std::string& version);
std::string GetLatestVersionUrl();
// returns current version; if use_cached and check timer has not expired
//         empty string   ; if download failed
//         latest version ; otherwise
std::string GetLatestVersion(bool use_cached = true);

#endif
