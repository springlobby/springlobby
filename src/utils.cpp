/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// File: utils.h
//

#include <sstream>
#include <wx/intl.h>
#include <wx/msgdlg.h>
#include <iostream>

#include "utils.h"
#include "revision.h"
#include "stacktrace.h"

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


void DumpStackTraceToLog()
{
  #if wxUSE_STACKWALKER

  wxString DebugInfo = _T("\n-------- Begin StackTrace --------\n");

  DebugInfo += _T("StackTraceID: ") + stacktrace().GetStackTraceHash() + _T("\n");

  stacktrace().Walk();
  DebugInfo += stacktrace().GetStackTrace();

  DebugInfo += _T("-------- End StackTrace --------");

  wxLogMessage( DebugInfo );

  customMessageBox(SL_MAIN_ICON, _("SpringLobby has generated a fatal error and will be terminated\nA stacktrace will be dumped to the application's console output"),_("Critical error"), wxICON_ERROR );

  #else
  customMessageBox(SL_MAIN_ICON, _("SpringLobby has generated a fatal error and will be terminated\nGenerating a stacktrace is not possible\n\nplease enable wxStackWalker"),_("Critical error"), wxICON_ERROR );
  #endif
}

//! @brief Initializes the logging functions.
///initializes logging in both std::cout and gui messages
void InitializeLoggingTargets()

{
	#if wxUSE_STD_IOSTREAM
  wxLog *loggerconsole = new wxLogStream( &std::cout );
  wxLogChain *logChain = new wxLogChain( loggerconsole );
  #else
  wxLog *loggerwin = new wxLogWindow(0, _T("SpringLobby error console")  );
  wxLogChain *logChain = new wxLogChain( loggerwin );
  #endif
  logChain->GetOldLog()->SetLogLevel( wxLOG_Warning );
  logChain->SetLogLevel( wxLOG_Trace );
  logChain->SetVerbose( true );
}

std::string i2s( int x )
{
  std::ostringstream o;
  o << x;
  return o.str();
}


std::string GetWordParam( std::string& params )
{
  std::string::size_type pos;
  std::string param;

  pos = params.find( " ", 0 );
  if ( pos == std::string::npos ) {
    param = params;
    params = "";
    return param;
  } else {
    param = params.substr( 0, pos );
    params = params.substr( pos + 1 );
    return param;
  }
}


std::string GetSentenceParam( std::string& params )
{
  std::string::size_type pos;
  std::string param;

  pos = params.find( "\t", 0 );
  if ( pos == std::string::npos ) {
    param = params;
    params = "";
    return param;
  } else {
    param = params.substr( 0, pos );
    params = params.substr( pos + 1 );
    return param;
  }
}


std::string GetChatLineParam( std::string& params )
{
  std::string::size_type pos;
  std::string param;

  pos = params.find( "\n", 0 );
  if ( pos == std::string::npos ) {
    param = params;
    params = "";
    return param;
  } else {
    param = params.substr( 0, pos );
    params = params.substr( pos + 1 );
    return param;
  }
}


int GetIntParam( std::string& params )
{
  std::string::size_type pos;
  std::string param;

  pos = params.find( " ", 0 );
  if ( pos == std::string::npos ) {
    param = params;
    params = "";
    return atoi( param.c_str() );
  } else {
    param = params.substr( 0, pos );
    params = params.substr( pos + 1 );
    return atoi( param.c_str() );
  }
}


bool GetBoolParam( std::string& params )
{
  return (bool)GetIntParam( params );
}


std::string GetSpringLobbyVersion()
{
#ifdef VERSION
  return std::string(VERSION) + " built from " + revision();
#else
  return std::string("Unknown built from ") + revision();
#endif
}
