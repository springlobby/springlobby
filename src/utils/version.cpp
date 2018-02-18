/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_CONFIGH
#error SPRINGLOBBY_CONFIGH is not defined
#endif

#include SPRINGLOBBY_CONFIGH
#include "version.h"

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
#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__) || defined(__MINGW32__)
	agent += "win";
#elif __APPLE__
	agent += "osx";
#else
	agent += "unix";
#endif

	agent += " ";

#if __WORDSIZE == 64
	agent += "x64";
#else
	agent += "x32";
#endif
	agent += ")";

	return agent;
}
