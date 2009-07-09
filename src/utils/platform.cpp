/* Copyright (C) 2007-2009 The SpringLobby Team. All rights reserved. */

#include "platform.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef VERSION
	#define VERSION "unknown"
#endif

#include <wx/string.h>
#include <wx/log.h>
#include <wx/dynlib.h>
#include <wx/stdpaths.h>
#include <wx/textfile.h>
#include <wx/filename.h>
#include <wx/dir.h>

#ifdef __WXMSW__
 #include <wx/msw/registry.h>
#endif

#include <iostream>

#include "conversion.h"
#include "math.h"

wxString GetLibExtension()
{
    return wxDynamicLibrary::CanonicalizeName(_T(""), wxDL_MODULE);
}


//! @brief Initializes the logging functions.
///initializes logging in an hidden stream and std::cout/gui messages
wxLogWindow* InitializeLoggingTargets( wxFrame* parent, bool console, bool showgui, bool logcrash, int verbosity, wxLogChain* logChain )
{
    wxLogWindow* loggerwin = 0;

#if wxUSE_STD_IOSTREAM
    if (  console && verbosity != 0 )
    {
        ///std::cout logging
        logChain = new wxLogChain( new wxLogStream( &std::cout ) );
    }
#endif

    if ( showgui && verbosity != 0 )
    {
        ///gui window logging
        loggerwin = new wxLogWindow( (wxWindow*) parent, _T("SpringLobby error console"), showgui );
        logChain = new wxLogChain( loggerwin );
    }

    #if 0 //TODO reenable wxUSE_DEBUGREPORT
        if ( logcrash )
        {
            ///hidden stream logging for crash reports, verbosity ignores command line params
            wxLog *loggercrash = new wxLogStream( &crashreport().crashlog );
            wxLogChain *logCrashChain = new wxLogChain( loggercrash );
            lastlog = logCrashChain;
        }

        #if wxUSE_DEBUGREPORT && defined(ENABLE_DEBUG_REPORT)
            ///hidden stream logging for crash reports
            wxLog *loggercrash = new wxLogStream( &crashreport().crashlog );
            wxLogChain *logCrashChain = new wxLogChain( loggercrash );
            logCrashChain->SetLogLevel( wxLOG_Trace );
            logCrashChain->SetVerbose( true );
        #endif

    #endif


    if ( !(  console || showgui ) || verbosity == 0 ){
        new wxLogNull;
        return loggerwin;
    }

    if ( logChain )
    {
        switch (verbosity)
        {
            case 1:
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



wxString GetSpringLobbyVersion()
{
#ifndef AUX_VERSION
    return (TowxString(VERSION)).BeforeFirst( _T(' ') );
#else
    return (TowxString(VERSION)).BeforeFirst( _T(' ') ) + TowxString(AUX_VERSION);
#endif

}

wxString GetExecutableFolder()
{
	return wxStandardPathsBase::Get().GetExecutablePath().BeforeLast( wxFileName::GetPathSeparator() );
}


// ------------------------------------------------------------------------------------------------------------------------
///
/// Read out Host's CPU Speed
///
/// \return Sum of each CPU's Speed of this Computer
///
///
// ------------------------------------------------------------------------------------------------------------------------
wxString GetHostCPUSpeed()
{

    int cpu_count = 0;
    int max_cpu_speed=0;

#ifdef __WXMSW__

    //afaik there is no way to determine the number of sub keys for a given key
    //so i'll hardcode some value here and hope bd doesn't hit me with a stick :P
    for (int i = 0; i< 16; ++i)
    {
        wxRegKey programreg( _T("HKEY_LOCAL_MACHINE\\HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\")+	wxString::Format(_T("%d"), i));
        long tmp;
        if ( programreg.QueryValue( _T("~MHz"), &tmp ) )
        {
            if ( max_cpu_speed < tmp ) max_cpu_speed = tmp;
            cpu_count++;
        }

    }

#else

    wxTextFile file( _T("/proc/cpuinfo") );
    if ( file.Exists() )
    {
      file.Open();
      for ( wxString line = file.GetFirstLine(); !file.Eof(); line = file.GetNextLine() )
      {
        if ( line.Left(7) == _T("cpu MHz") )
        {
          line = line.AfterLast( _T(' ') ).BeforeLast( _T('.') );
          cpu_count++;
          int tmp = s2l( line );
          if ( max_cpu_speed < tmp ) max_cpu_speed = tmp;
        }
      }
    }
#endif
    return TowxString( clamp( max_cpu_speed,0,max_cpu_speed ) );
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

int WinExecuteAdmin( const wxString& command, const wxString& params )
{
      SHELLEXECUTEINFO shExecInfo;

      shExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);

      shExecInfo.fMask = NULL;
      shExecInfo.hwnd = NULL;

      //on XP this would open the real runas dialog, which apparently is its own wonder
      //by default it has a checkbox enabled which makes sl unable to write to the working dir...
      if ( IsUACenabled() )
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

      ShellExecuteEx(&shExecInfo);

      return 0;
}
#endif
