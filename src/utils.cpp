/* Copyright (C) 2007, 2008 The SpringLobby Team. All rights reserved. */
//
// File: utils.h
//

#include <wx/dynlib.h>
#include <iostream>
#include <wx/log.h>

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
#include <algorithm>
#include <fstream>
#include <wx/regex.h>
#ifdef __WXMSW__
#include <wx/msw/registry.h>
#endif
#include <wx/intl.h>
#include "settings.h"
#include <vector>

wxString GetLibExtension()
{
    return wxDynamicLibrary::CanonicalizeName(_T(""), wxDL_MODULE);
}


//! @brief Initializes the logging functions.
///initializes logging in an hidden stream and std::cout/gui messages
void InitializeLoggingTargets()

{
#if defined ( USE_LOG_WINDOW )
    ///gui window fallback logging if console/stream output not available
    wxLog *loggerwin = new wxLogWindow(0, _T("SpringLobby error console")  );
    loggerwin->SetLogLevel( wxLOG_Trace );
    loggerwin->SetVerbose( true );
#elif wxUSE_STD_IOSTREAM
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
#else
    /// if all fails, no log is better than msg box spam :P
    new wxLogNull();
#endif
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
#ifndef AUX_VERSION
    return (WX_STRINGC(VERSION)).BeforeFirst( _T(' ') );
#else
    return (WX_STRINGC(VERSION)).BeforeFirst( _T(' ') ) + _T(AUX_VERSION);
#endif

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
    return i2s( clamp( max_cpu_speed,0,max_cpu_speed ) );
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

template<typename T>
T min(T a, T b, T c)
{
    return std::min(a, std::min(b, c));
}

double LevenshteinDistance(wxString s, wxString t)
{
    s.MakeLower(); // case insensitive edit distance
    t.MakeLower();

    const int m = s.length(), n = t.length(), _w = m + 1;
    std::vector<unsigned char> _d((m + 1) * (n + 1));
#define D(x, y) _d[(y) * _w + (x)]

    for (int i = 0; i <= m; ++i) D(i,0) = i;
    for (int j = 0; j <= n; ++j) D(0,j) = j;

    for (int i = 1; i <= m; ++i)
    {
        for (int j = 1; j <= n; ++j)
        {
            const int cost = (s[i - 1] != t[j - 1]);
            D(i,j) = min(D(i-1,j) + 1, // deletion
                         D(i,j-1) + 1, // insertion
                         D(i-1,j-1) + cost); // substitution
        }
    }
    double d = (double) D(m,n) / std::max(m, n);
    wxLogMessage( _T("LevenshteinDistance('%s', '%s') = %g"), s.c_str(), t.c_str(), d );
    return d;
#undef D
}

wxString GetBestMatch(const wxArrayString& a, const wxString& s, double* distance )
{
    const unsigned int count = a.GetCount();
    double minDistance = 1.0;
    int minDistanceIndex = -1;
    for (unsigned int i = 0; i < count; ++i)
    {
        const double distance = LevenshteinDistance(a[i], s);
        if (distance < minDistance)
        {
            minDistance = distance;
            minDistanceIndex = i;
        }
    }
    if (distance != NULL) *distance = minDistance;
    if (minDistanceIndex == -1) return _T("");
    return a[minDistanceIndex];
}
