
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
