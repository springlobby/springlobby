/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// File: utils.h
//

#include <wx/dynlib.h>
#include <iostream>

#include "utils.h"
#include "revision.h"
#include "crashreport.h"

#include "settings++/custom_dialogs.h"

// FIXME this does not work on linux+mingw build for windows
#ifdef _MSC_VER
#include <windows.h>
#include <wx/msw/winundef.h>
#endif
//#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


wxString GetLibExtension()
{
  return wxDynamicLibrary::CanonicalizeName(_T(""), wxDL_MODULE);
}


//! @brief Initializes the logging functions.
///initializes logging in an hidden stream and std::cout/gui messages
void InitializeLoggingTargets( bool console, bool showgui, bool logcrash, int verbosity )
{
  wxLog::SetLogLevel( wxLOG_Warning );
  wxLogChain *lastlog;
	#if wxUSE_STD_IOSTREAM
    #if wxUSE_DEBUGREPORT
      if ( logcrash )
      {
        ///hidden stream logging for crash reports, verbosity ignores command line params
        wxLog *loggercrash = new wxLogStream( &crashreport().crashlog );
        wxLogChain *logCrashChain = new wxLogChain( loggercrash );
        logCrashChain->SetLogLevel( wxLOG_Warning );
      }
    #endif
    if (  console && verbosity != 0 )
    {
      ///std::cout logging
      wxLog *loggerconsole = new wxLogStream( &std::cout );
      wxLogChain *logConsoleChain = new wxLogChain( loggerconsole );
      lastlog = logConsoleChain;
    }
  #endif
  if ( showgui && verbosity != 0 )
    {
      ///gui window logging
      wxLog *loggerwin = new wxLogWindow(0, _T("SpringLobby error console"), showgui );
      wxLogChain *logGuiChain = new wxLogChain( loggerwin );
      lastlog = logGuiChain;
    }
  if ( lastlog != 0 )
  {
    wxMessageBox( wxString::Format(_T("%d"), verbosity));
    switch (verbosity)
    {
      case 1:
        lastlog->SetLogLevel( wxLOG_FatalError );
      case 2:
        lastlog->SetLogLevel( wxLOG_Error );
      case 3:
        lastlog->SetLogLevel( wxLOG_Warning );
      case 4:
        lastlog->SetLogLevel( wxLOG_Message );
      case 5:
        lastlog->SetLogLevel( wxLOG_Trace );
        lastlog->SetVerbose( true );
      default:
        lastlog->SetLogLevel( wxLOG_Warning );
    }
  }
  else if ( verbosity == 0 ) wxLogNull NoLog;
}


wxString GetWordParam( wxString& params )
{
  wxString param;

  param = params.BeforeFirst( ' ' );
  if ( param.IsEmpty() )
  {
    param = params;
    params = _T("");
    return param;
  }
  else
  {
    params = params.AfterFirst( ' ' );
    return param;
  }
}


wxString GetSentenceParam( wxString& params )
{
  wxString param;

  param = params.BeforeFirst( '\t' );
  if ( param.IsEmpty() )
  {
    param = params;
    params = _T("");
    return param;
  }
  else
  {
    params = params.AfterFirst( '\t' );
    return param;
  }
}


long GetIntParam( wxString& params )
{
  wxString param;
  long ret;

  param = params.BeforeFirst( ' ' );
  if ( param.IsEmpty() )
  {
    params.ToLong( &ret );
    params = _T("");
  }
  else
  {
    params = params.AfterFirst( ' ' );
    param.ToLong( &ret );
  }
  return ret;
}


bool GetBoolParam( wxString& params )
{
  return (bool)GetIntParam( params );
}


wxString GetSpringLobbyVersion()
{
  return WX_STRINGC(VERSION);
}


