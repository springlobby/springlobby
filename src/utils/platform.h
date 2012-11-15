#ifndef SPRINGLOBBY_HEADERGUARD_PLATFORM_H
#define SPRINGLOBBY_HEADERGUARD_PLATFORM_H

class wxLogWindow;
class wxFrame;
class wxString;
class wxLogChain;

#include <wx/string.h>

/**
    let origin be /path/to/some/dir and destination /some/other/path
    this will copy dir (and everything below that recursively to /some/other/path/dir
    \return true if successful
*/
bool CopyDir( wxString origin, wxString destination, bool overwrite = true);
bool CopyDirWithFilebackupRename( wxString origin, wxString destination, bool overwrite = true);

/** \brief on vista execute command with admin temp eleveation, on earlier win it acts as "normal" shell execute **/
bool WinExecuteAdmin( const wxString& command, const wxString& params );
/** \brief "normal" shell execute **/
bool WinExecute( const wxString& command, const wxString& params );

//! returns false on !win, checks for regkey on win
bool IsUACenabled();

/** \brief initialize logchain

    \return Logwindow pointer (may be 0), useful if parent frame should be created _after_ logging is set up
**/
wxLogWindow* InitializeLoggingTargets( wxFrame* parent, bool console, const wxString& logfilepath, bool showgui, bool logcrash, int verbosity, wxLogChain* logChain );

wxString GetExecutableFolder();
wxString GetLibExtension();
wxString GetHostCPUSpeed();

//! set new cwd in ctor, reset to old in dtor
class CwdGuard {
    wxString m_old_cwd;
    public:
        CwdGuard( const wxString& new_cwd );
        ~CwdGuard();
};

//! remember pwd in ctor and reset in dtor
class PwdGuard {
	wxString m_old_pwd;
	public:
		PwdGuard( );
		~PwdGuard();
};


#ifdef __WXMSW__
bool IsPreVistaWindows();
#endif

//! simply return wxApp::GetAppName with letter lowercased on demand
wxString GetAppName( const bool lowerCase = false );
wxString GetConfigfileDir();
wxString GetUserDataDir();

/**
  \in Format string with a single %s
  \out wxString with %s replaced with GetAppName()
  **/
wxString IdentityString(const wxString format, bool lowerCase = false );

wxString GetCustomizedEngineConfigFilePath();

#ifdef __WXMSW__
#include <wx/msw/registry.h>
template < typename T >
static T GetRegkeyVal( const wxRegKey& reg, const wxString& name, const T def )
{
	T val = def;
	if ( reg.QueryValue( name, &val ) )
		return val;
	else
		return def;
}

template < >
wxString GetRegkeyVal( const wxRegKey& reg, const wxString& name, const wxString& def )
{
	wxString val = def;
	if ( reg.QueryValue( name, val ) )
		return val;
	else
		return def;
}
#endif

//!
void CopyUikeys(wxString currentDatadir);

#endif // SPRINGLOBBY_HEADERGUARD_PLATFORM_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2011

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
