/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_VERSIONCHECKER_H
#define SPRINGLOBBY_HEADERGUARD_VERSIONCHECKER_H

class wxString;
// convert a string to IRI: https://en.wikipedia.org/wiki/Internationalized_resource_identifier
wxString ConvToIRI(const wxString& str);
wxString GetHttpFile(const wxString& httpurl);

#endif // SPRINGLOBBY_HEADERGUARD_VERSIONCHECKER_H
