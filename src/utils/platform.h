#ifndef SPRINGLOBBY_HEADERGUARD_PLATFORM_H
#define SPRINGLOBBY_HEADERGUARD_PLATFORM_H

class wxLogWindow;
class wxFrame;
class wxString;

/** these need to stay to not break non-autotools builds */
#if ( !defined(HAVE_WX26) && !defined(HAVE_WX28) )
	#if( wxMAJOR_VERSION==2 && wxMINOR_VERSION == 6 )
		#define HAVE_WX26
	#elif( wxMAJOR_VERSION==2 && wxMINOR_VERSION == 8 )
		#define HAVE_WX28
	#endif
#endif


/**
    let origin be /path/to/some/dir and destination /some/other/path
    this will copy dir (and everything below that recursively to /some/other/path/dir
    \return true if successful
*/
bool CopyDir( wxString origin, wxString destination, bool overwrite = true);

/** \brief execute command with admin temp eleveation **/
int WinExecuteAdmin( const wxString& command, const wxString& params );

//! returns false on !win, checks for regkey on win
bool IsUACenabled();

/** \brief initialize logchain

    \return Logwindow pointer (may be 0), useful if parent frame should be created _after_ logging is set up
**/
wxLogWindow* InitializeLoggingTargets( wxFrame* parent, bool console, bool showgui, bool logcrash, int verbosity );

wxString GetSpringLobbyVersion();
wxString GetExecutableFolder();
wxString GetLibExtension();
wxString GetHostCPUSpeed();

#endif // SPRINGLOBBY_HEADERGUARD_PLATFORM_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-09

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    SpringLobby is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/
