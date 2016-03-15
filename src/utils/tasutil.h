/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_SERVERUTIL_H
#define SPRINGLOBBY_HEADERGUARD_SERVERUTIL_H

#include <string>

std::string GetWordParam(std::string& params);
std::string GetSentenceParam(std::string& params);
long GetIntParam(std::string& params);
bool GetBoolParam(std::string& params);
std::string GetParamByChar(std::string& params, const char sep);

//! matches against regex for printable ascii chars, excluding space
bool IsValidNickname(const std::string& name);


#endif // SPRINGLOBBY_HEADERGUARD_SERVERUTIL_H
