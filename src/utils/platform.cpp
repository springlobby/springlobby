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

// copied from http://wxforum.shadonet.com/viewtopic.php?t=2080
//slightly modified
bool CopyDir( wxString from, wxString to, bool overwrite )
{
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

    // first make sure that the source dir exists
    if(!wxDir::Exists(from)) {
            wxLogError(from + _T(" does not exist.  Can not copy directory.") );
            return false;
    }

    if (!wxDirExists(to))
        wxMkdir(to);

    wxDir dir(from);
    wxString filename;
    bool bla = dir.GetFirst(&filename);

    if (bla){
        do {

            if (wxDirExists(from + filename) )
            {
                wxMkdir(to + filename);
                CopyDir(from + filename, to + filename, overwrite);
            }
            else{
                wxCopyFile(from + filename, to + filename, overwrite);
            }
        }
        while (dir.GetNext(&filename) );
    }
    return true;
}

bool CopyDirWithFilebackupRename( wxString from, wxString to, bool overwrite )
{
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

    // first make sure that the source dir exists
    if(!wxDir::Exists(from)) {
            wxLogError(from + _T(" does not exist.  Can not copy directory.") );
            return false;
    }

    if (!wxDirExists(to))
        wxMkdir(to);

    wxDir dir(from);
    wxString filename;
    bool bla = dir.GetFirst(&filename);

    if (bla){
        do {

            if (wxDirExists(from + filename) )
            {
                wxMkdir(to + filename);
                CopyDir(from + filename, to + filename, overwrite);
            }
            else{
                //if files exists move it to backup, this way we can use this func on windows to replace 'active' files
                if ( wxFileExists( to + filename ) ) {
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
        }
        while (dir.GetNext(&filename) );
    }
    return true;
}

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

bool WinExecuteAdmin( const wxString& command, const wxString& params )
{
#ifdef __WXMSW__
      SHELLEXECUTEINFO shExecInfo;

      shExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);

      shExecInfo.fMask = 0;
      shExecInfo.hwnd = NULL;

      //on XP this would open the real runas dialog, which apparently is its own wonder
      //by default it has a checkbox enabled which makes sl unable to write to the working dir...
      if ( IsUACenabled() ) {
        if ( IsPreVistaWindows() )
            shExecInfo.lpVerb = _T("open");
        else
            shExecInfo.lpVerb = _T("runas");
#ifdef _MSC_VER //some strange compiler stupidity going on here
      shExecInfo.lpFile = command.c_str();
      shExecInfo.lpParameters = params.c_str();
#else
	  shExecInfo.lpFile = command.wc_str();
      shExecInfo.lpParameters = params.wc_str();
#endif
      shExecInfo.lpDirectory = NULL;
      shExecInfo.hInstApp = NULL;
      }
      return ShellExecuteEx(&shExecInfo);
#endif // __WXMSW__
	return false;
}

#ifdef __WXMSW__
bool WinExecute( const wxString& command, const wxString& params )
{
      SHELLEXECUTEINFO shExecInfo;

      shExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    shExecInfo.lpVerb = _T("open");
      shExecInfo.fMask = 0;
      shExecInfo.hwnd = NULL;

#ifdef _MSC_VER //some strange compiler stupidity going on here
      shExecInfo.lpFile = command.c_str();
      shExecInfo.lpParameters = params.c_str();
#else
	  shExecInfo.lpFile = command.wc_str();
      shExecInfo.lpParameters = params.wc_str();
#endif
      shExecInfo.lpDirectory = NULL;
      shExecInfo.hInstApp = NULL;

      return ShellExecuteEx(&shExecInfo);
}

bool IsPreVistaWindows()
{
    return wxPlatformInfo().GetOSMajorVersion() < 6;
}
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

wxString GetAppName( const bool lowerCase )
{
	return TowxString(getSpringlobbyName(lowerCase));
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

int RunProcess(const wxString& cmd, const wxArrayString& params)
{
	wxString paramstring;
	for (wxString param: params) {
		if (!paramstring.empty()) {
			paramstring += _T(" ");
		}
		paramstring += escapeStr(param);
	}
#ifdef __WXMSW__
	SHELLEXECUTEINFO ShExecInfo;
	DWORD exitCode = 0;

	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = cmd.t_str();
	ShExecInfo.lpParameters = paramstring.t_str();
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_SHOW;
	ShExecInfo.hInstApp = NULL;

	ShellExecuteEx(&ShExecInfo);
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
