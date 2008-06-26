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


wxString GetLibExtension()
{
  return wxDynamicLibrary::CanonicalizeName(_T(""), wxDL_MODULE);
}


//! @brief Initializes the logging functions.
///initializes logging in an hidden stream and std::cout/gui messages
void InitializeLoggingTargets()

{
	#if wxUSE_STD_IOSTREAM
    #if wxUSE_DEBUGREPORT && defined(HAVE_WX28) && defined(ENABLE_DEBUG_REPORT)
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
  #elif defined ( USE_LOG_WINDOW )
    ///gui window fallback logging if console/stream output not available
    wxLog *loggerwin = new wxLogWindow(0, _T("SpringLobby error console")  );
    wxLogChain *logChain = new wxLogChain( loggerwin );
    logChain->SetLogLevel( wxLOG_Trace );
    logChain->SetVerbose( true );
    logChain->GetOldLog()->SetLogLevel( wxLOG_Warning );
  #else
    /// if all fails, no log is better than msg box spam :P
    new wxLogNull();
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


// ------------------------------------------------------------------------------------------------------------------------
///
/// Read out Host's CPU Speed
///
/// \return Sum of each CPU's Speed of this Computer
///
/// \TODO Porting to Windows
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
