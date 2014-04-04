/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "platform.h"

#include <wx/string.h>
#include <wx/log.h>
#include <wx/dynlib.h>
#include <wx/stdpaths.h>
#include <wx/textfile.h>
#include <wx/filename.h>
#include <wx/dir.h>
#include <wx/app.h>

#include <iostream>

#include <stdio.h>

#include "conversion.h"
#include "../updater/versionchecker.h"
#include "customdialogs.h"
#include <lslutils/misc.h>
#include "../crashreport.h"
#include "utils/version.h"

//! @brief Initializes the logging functions.
///initializes logging in an hidden stream and std::cout/gui messages
wxLogWindow* InitializeLoggingTargets( wxWindow* parent, bool console, const wxString&  logfilepath, bool showgui, int verbosity, wxLogChain* logChain )
{
	wxLogWindow* loggerwin = NULL;
	if ( console ) {
#if wxUSE_STD_IOSTREAM
		///std::cout logging
		logChain = new wxLogChain( new wxLogStream( &std::cout ) );
#else
		///std::cerr logging
		logChain = new wxLogChain( new  wxLogStderr( 0 ) );
#endif
	}

	if (showgui) {
		///gui window logging
		loggerwin = new wxLogWindow(parent, IdentityString( _("%s error console") ), showgui );
		logChain = new wxLogChain( loggerwin );
	}

	if (!console && !showgui) {
		new wxLogNull(); //memleak but disables logging as no log target exists
		//FIXME: there should be a cleaner way (like just not showing message boxes)
	}

	if (!logfilepath.empty()) {
		FILE* logfile = fopen(C_STRING(logfilepath), "w"); // even if it returns null, wxLogStderr will switch to stderr logging, so it's fine
		logChain = new wxLogChain( new  wxLogStderr( logfile ) );
	}

#if wxUSE_DEBUGREPORT && defined(ENABLE_DEBUG_REPORT) && wxUSE_STD_IOSTREAM
	///hidden stream logging for crash reports
	wxLog *loggercrash = new wxLogStream( &CrashReport::instance().crashlog );
	logChain = new wxLogChain( loggercrash );

//	logCrashChain->SetLogLevel( wxLOG_Trace );
//	logCrashChain->SetVerbose( true );
#endif

	if (logChain!=NULL) {
	switch (verbosity) {
		case 0: case 1:
			logChain->SetLogLevel( wxLOG_FatalError ); break;
		case 2:
			logChain->SetLogLevel( wxLOG_Error ); break;
		case 3:
			logChain->SetLogLevel( wxLOG_Warning ); break;
		case 4:
			logChain->SetLogLevel( wxLOG_Message ); break;
		case 5:
			logChain->SetLogLevel( wxLOG_Trace );
			logChain->SetVerbose( true ); break;
		default://meaning loglevel < 0 or > 5 , == 0 is handled seperately
			logChain->SetLogLevel( wxLOG_Warning ); break;
		}
	}

	return loggerwin;
}

bool SafeMkdir(const wxString& dir)
{
    if (!wxDirExists(dir))
        return wxMkdir(dir);
	return true;
}

bool CopyDirWithFilebackupRename( wxString from, wxString to, bool overwrite, bool backup )
{
    // first make sure that the source dir exists
    if(!wxDir::Exists(from)) {
            wxLogError(from + _T(" does not exist.  Can not copy directory.") );
            return false;
    }

    SafeMkdir(to);

    wxString sep = wxFileName::GetPathSeparator();

    // append a slash if there is not one (for easier parsing)
    // because who knows what people will pass to the function.
    if ( !to.EndsWith( sep ) ) {
            to += sep;
    }
    // for both dirs
    if ( !from.EndsWith( sep ) ) {
            from += sep;
    }


    wxDir dir(from);
    wxString filename;
    if (!dir.GetFirst(&filename)) {
		return false;
	}

	do {
		if (wxDirExists(from + filename) )
		{
			CopyDirWithFilebackupRename(from + filename, to + filename, overwrite, false); //no backup in subdirs
		} else{
			//if files exists move it to backup, this way we can use this func on windows to replace 'active' files
			if ( backup && wxFileExists( to + filename ) ) {
				//delete prev backup
				if ( wxFileExists( to + filename + _T(".old") ) ) {
					wxRemoveFile( to + filename + _T(".old")  );
				}
				//make backup
				if ( !wxRenameFile( to + filename, to + filename + _T(".old") ) ) {
					wxLogError( _T("could not rename %s, copydir aborted"), (to + filename).c_str() );
					return false;
				}
			}
			//do the actual copy
			if ( !wxCopyFile(from + filename, to + filename, overwrite) ) {
				wxLogError( _T("could not copy %s to %s, copydir aborted"), (from + filename).c_str(), (to + filename).c_str() );
				return false;
			}
		}
	} while (dir.GetNext(&filename) );
    return true;
}

#ifdef __WXMSW__
bool IsPreVistaWindows()
{
    return wxPlatformInfo().GetOSMajorVersion() < 6;
}
#endif

bool IsUACenabled()
{
#ifdef __WXMSW__
    wxRegKey UACpath( _T("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System") ); // check if UAC is on, skip dialog if not
    if( UACpath.Exists() )
    {
        long value;
        if( UACpath.QueryValue( _T("EnableLUA"), &value ) ) // reg key not present -> not vista
        {
            if( value != 0 )
            {
                return true;
            }
        }
    }
#endif
    return false;
}

#ifdef __WXMSW__
#include <windows.h>
#include <wx/msw/winundef.h>
#include <shellapi.h>
#endif


CwdGuard::CwdGuard( const wxString& new_cwd )
    : m_old_cwd( wxGetCwd() )
{
    wxSetWorkingDirectory( new_cwd );
}
CwdGuard::~CwdGuard()
{
    wxSetWorkingDirectory( m_old_cwd );
}

PwdGuard::PwdGuard(  )
	: m_old_pwd( wxGetCwd() )
{}

PwdGuard::~PwdGuard()
{
	wxSetWorkingDirectory( m_old_pwd );
}

wxString IdentityString(const wxString& format, bool lowerCase )
{
	return wxFormat( format ) % TowxString(getSpringlobbyName(lowerCase));
}

static wxString escapeStr(const wxString& str)
{
	if (str.Find(_T(" ")) == wxNOT_FOUND)
		return str;
	return _T("\"") + str + _T("\"");
}

int RunProcess(const wxString& cmd, const wxArrayString& params, const bool async, const bool root)
{
	wxString paramstring;
	for (wxString param: params) {
		if (!paramstring.empty()) {
			paramstring += _T(" ");
		}
		paramstring += escapeStr(param);
	}
	wxLogDebug(_T("going to run %s %s"), cmd.c_str(), paramstring.c_str());
#ifdef __WXMSW__
	SHELLEXECUTEINFO ShExecInfo;
	DWORD exitCode = 0;
	ShExecInfo.lpVerb = _T("open");
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpFile = cmd.t_str();
	ShExecInfo.lpParameters = paramstring.t_str();
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_SHOW;
	ShExecInfo.hInstApp = NULL;

	if (root && IsUACenabled()) {
		if (!IsPreVistaWindows() )
			ShExecInfo.lpVerb = _T("runas");
	}

	int res = ShellExecuteEx(&ShExecInfo);
	if (async) return (res > 32);
	WaitForSingleObject(ShExecInfo.hProcess,INFINITE);
	GetExitCodeProcess(ShExecInfo.hProcess, &exitCode);
	return exitCode;
#else
	wxString realcmd = escapeStr(cmd);
	if (!paramstring.empty()) {
		realcmd += _T(" ") + paramstring;
	}
	return system( realcmd.mb_str( wxConvUTF8 ) );
#endif
}

int BrowseFolder(const wxString& path)
{
	wxArrayString param;
#ifdef __WXMSW__
	return RunProcess(path, param);
#elif defined(__APPLE__)
	param.push_back(path);
	return RunProcess(_T("open"), param);
#else
	param.push_back(path);
	return RunProcess(_T("xdg-open"), param);
#endif
}

int WaitForExit(int pid)
{
#ifdef WIN32
	HANDLE h = OpenProcess(0, false, pid);
	if (h == NULL) {
        return 0;
	}
	WaitForSingleObject(h, INFINITE);
	DWORD exitCode = 0;
	GetExitCodeProcess(h, &exitCode);
	CloseHandle(h);
	return exitCode;
#endif
	return 0;
}
