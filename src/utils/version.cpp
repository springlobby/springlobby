
#ifdef HAVE_VERSION
#include "springlobby_config.h"
#else
#define VERSION "unknown"
#endif

#include "version.h"

const std::string getSpringlobbyVersion()
{
	const static std::string version(VERSION);
	return version;
}

const std::string getSpringlobbyAgent()
{
	std::string agent = "SpringLobby ";
	agent += getSpringlobbyVersion();
	agent += " (";
#if WIN32
	agent+="win";
#elif __APPLE__
	agent+="osx";
#else
	agent+="unix";
#endif

	agent+=" ";

#if __WORDSIZE == 64
	agent+="x64";
#else
	agent+="x32";
#endif
	agent+=")";

	return agent;
}
