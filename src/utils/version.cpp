/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_CONFIGH
#error SPRINGLOBBY_CONFIGH is not defined
#endif

#include SPRINGLOBBY_CONFIGH
#include "version.h"
#include <lsl/lslutils/platform.h>

const std::string GetSpringlobbyName(bool lowercase)
{
	if (lowercase)
		return "springlobby";
	return "SpringLobby";
}

const std::string GetSpringlobbyVersion()
{
	const static std::string version(VERSION);
	return version;
}

const std::string GetSpringlobbyAgent()
{
	std::string agent = GetSpringlobbyName() + " ";
	agent += GetSpringlobbyVersion();
	agent += " (";
	agent += LSL::Util::GetPlatformString(LSL::Util::GetPlatform());
	agent += ")";

	return agent;
}
