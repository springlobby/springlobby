#ifndef SPRINGLOBBY_HEADERGUARD_UPDATEHELPER_H
#define SPRINGLOBBY_HEADERGUARD_UPDATEHELPER_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif //HAVE_CONFIG_H

#ifndef VERSION
	#define VERSION "unknown"
#endif //VERSION

class wxString;

void CheckForUpdates();
wxString GetDownloadUrl( const wxString& version );
wxString GetSpringLobbyVersion();

#endif
