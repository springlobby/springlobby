/* Copyright (C) 2007, 2008 The SpringLobby Team. All rights reserved. */
//
// File: utils.h
//

#include <wx/dynlib.h>
#include <iostream>

#include "utils.h"
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

//for cpu detection
#include <wx/tokenzr.h>
#include <string>
#include <fstream>
#include <wx/regex.h>
#ifdef __WXMSW__
#include <wx/msw/registry.h>
#endif
#include <wx/intl.h>
#include "settings.h"

wxString GetLibExtension()
{
  return wxDynamicLibrary::CanonicalizeName(_T(""), wxDL_MODULE);
}


//! @brief Initializes the logging functions.
///initializes logging in an hidden stream and std::cout/gui messages
void InitializeLoggingTargets( bool console, bool showgui, bool logcrash, int verbosity )
{
  wxLogChain *lastlog;
  if ( showgui && verbosity != 0 )
  {
    ///gui window logging
    wxLog *loggerwin = new wxLogWindow(0, _T("SpringLobby error console"), showgui );
    wxLogChain *logGuiChain = new wxLogChain( loggerwin );
    lastlog = logGuiChain;
  }
	#if wxUSE_STD_IOSTREAM

    if (  console && verbosity != 0 )
    {
      ///std::cout logging
      wxLog *loggerconsole = new wxLogStream( &std::cout );
      wxLogChain *logConsoleChain = new wxLogChain( loggerconsole );
      lastlog = logConsoleChain;
    }
        #if 0 //TODO reenable wxUSE_DEBUGREPORT
          if ( logcrash )
          {
            ///hidden stream logging for crash reports, verbosity ignores command line params
            wxLog *loggercrash = new wxLogStream( &crashreport().crashlog );
            wxLogChain *logCrashChain = new wxLogChain( loggercrash );
            lastlog = logCrashChain;
          }

            #if wxUSE_DEBUGREPORT && defined(HAVE_WX28) && defined(ENABLE_DEBUG_REPORT)
              ///hidden stream logging for crash reports
              wxLog *loggercrash = new wxLogStream( &crashreport().crashlog );
              wxLogChain *logCrashChain = new wxLogChain( loggercrash );
              logCrashChain->SetLogLevel( wxLOG_Trace );
              logCrashChain->SetVerbose( true );

            #endif

        #endif
    #endif

  if ( lastlog != 0 )
  {
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


wxString i2s( int arg )
{
    return TowxString(arg);
}


wxString u2s( unsigned int arg )
{
  return TowxString(arg);
}


wxString f2s( float arg )
{
  return TowxString(arg);
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
  return (WX_STRINGC(VERSION)).BeforeFirst( *wxT(" ") );
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

    int totalcpuspeed = 0;
    int cpu_count = 0;

#ifdef __WXMSW__

    //afaik there is no way to determine the number of sub keys for a given key
    //so i'll hardcode some value here and hope bd doesn't hit me with a stick :P
    for (int i = 0; i< 16; ++i)
    {
        wxRegKey programreg( _T("HKEY_LOCAL_MACHINE\\HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\")+	wxString::Format(_T("%d"), i));
        long* tmp = new long;
        if ( programreg.QueryValue( _T("~MHz"), tmp ) )
        {
            totalcpuspeed += (*tmp);
            cpu_count++;
        }

    }

#else


    // Create an Inputstream from /proc/cpuinfo
    std::ifstream fin( "/proc/cpuinfo" );
    std::string line;
    std::string content_str( "" );

    // Read from Inputstream

    if ( fin )
    {
        while ( std::getline( fin, line ) )
        {
            // std::cout << "Read from file: " << line << std::endl;
            content_str.append( line );
            content_str.append( "\n" );
        }

        wxString content = wxString::FromAscii( content_str.c_str() );

        // Building a RegEx to match one Block of CPU Info
        #ifdef wxHAS_REGEX_ADVANCED
        wxRegEx regex_CPUSection( wxT( "processor.*?(cpu MHz.*?:.*?(\\d+\\.\\d+)).*?\n\n" ), wxRE_ADVANCED );
        #else
        wxRegEx regex_CPUSection( wxT( "processor.*?(cpu MHz.*?:.*?(\\d+\\.\\d+)).*?\n\n" ), wxRE_EXTENDED );
        #endif
        // Replace each Block of CPU Info with only the CPU Speed in MHz
        regex_CPUSection.Replace( &content, _T( "\\2\n" ) );

        // Tokenize the String containing all CPU Speed of the Host: e.g. 3000.0\n3000.0\n
        wxStringTokenizer tokenlist( content, wxT( "\n" ) );

        // Sum up all CPU Speeds

        while ( tokenlist.HasMoreTokens() )
        {
            wxString token = tokenlist.GetNextToken();
            long cpuspeed = 0;
            token.ToLong( &cpuspeed, 10 );
            totalcpuspeed += cpuspeed;
            cpu_count++;
        }
    }
#endif
    totalcpuspeed = cpu_count > 0 ? totalcpuspeed / cpu_count : 2100;
    return i2s(totalcpuspeed);
}


//int CompareStringIgnoreCase(const wxString& first, const wxString& second)
//{
//    return (first.Upper() > second.Upper() );
//}

bool IsValidNickname( const wxString& _name )
{
    wxString name = _name;
    // The Regex Container
	//wxRegEx regex( wxT("[:graph:]") );
	wxRegEx regex( wxT("[ \t\r\n\v\föäüß, .:<>\\!§$%&+-]" ));

	// We need to escape all regular Expression Characters, that have a special Meaning
    name.Replace( _T("["), _T("") );
	name.Replace( _T("]"), _T("") );

    return !regex.Matches( name );
}

const wxChar* TooltipEnable(const wxChar* input)
{
    return sett().GetShowTooltips() ? input : _("");
}

