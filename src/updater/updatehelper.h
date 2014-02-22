/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_UPDATEHELPER_H
#define SPRINGLOBBY_HEADERGUARD_UPDATEHELPER_H

class wxString;

wxString GetDownloadUrl( const wxString& version );
wxString GetSpringLobbyVersion( bool consider_aux = true );
wxString GetSpringLobbyAuxVersion();

#endif
