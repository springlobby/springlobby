/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_VERSIONCHECKER_H
#define SPRINGLOBBY_HEADERGUARD_VERSIONCHECKER_H

#include <string>
// convert a string to IRI: https://en.wikipedia.org/wiki/Internationalized_resource_identifier
std::string ConvToIRI(const std::string& str);
std::string GetHttpFile(const std::string& httpurl);

#endif // SPRINGLOBBY_HEADERGUARD_VERSIONCHECKER_H
