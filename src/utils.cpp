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

#include <exception>
#include <stdexcept>


wxString GetLibExtension()
{
  return wxDynamicLibrary::CanonicalizeName(_T(""), wxDL_MODULE);
}


//! @brief Initializes the logging functions.
///initializes logging in an hidden stream and std::cout/gui messages
void InitializeLoggingTargets()

{
	#if wxUSE_STD_IOSTREAM
    #if wxUSE_DEBUGREPORT && defined(HAVE_WX28)
      ///hidden stream logging for crash reports
      wxLog *loggercrash = new wxLogStream( &crashreport().crashlog );
      wxLogChain *logCrashChain = new wxLogChain( loggercrash );
      logCrashChain->SetLogLevel( wxLOG_Trace );
      logCrashChain->SetVerbose( true );
    #endif
    ///std::cout logging
    wxLog *loggerconsole = new wxLogStream( &std::cout );
    wxLogChain *logChain = new wxLogChain( loggerconsole );
    logChain->SetLogLevel( wxLOG_Trace );
    logChain->SetVerbose( true );
  #else
    ///gui window fallback logging if console/stream output not available
    wxLog *loggerwin = new wxLogWindow(0, _T("SpringLobby error console")  );
    wxLogChain *logChain = new wxLogChain( loggerwin );
    logChain->SetLogLevel( wxLOG_Trace );
    logChain->SetVerbose( true );
    logChain->GetOldLog()->SetLogLevel( wxLOG_Warning );
  #endif
}


wxString i2s( int arg )
{
  std::stringstream s;
  s << arg;
  return WX_STRING( s.str() );
}


wxString u2s( unsigned int arg )
{
  std::stringstream s;
  s << arg;
  return WX_STRING( s.str() );
}


wxString f2s( float arg )
{
  std::stringstream s;
  s << arg;
  return WX_STRING( s.str() );
}


long s2l( const wxString& arg )
{
    long ret;
    arg.ToLong(&ret);
    return ret;
}

double s2d( const wxString& arg )
{
    double ret;
    arg.ToDouble(&ret);
    return ret;
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



