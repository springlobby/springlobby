/// \file uiutils.cpp

/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */

#include <wx/colour.h>
#include <wx/regex.h>

#include "uiutils.h"
#include "utils.h"

#ifdef __WXMSW__
#include <wx/msw/registry.h>
#endif


wxString RefineMapname( wxString mapname )
{
  mapname = mapname.SubString(0, mapname.Find( '.', true ) - 1 );
  mapname.Replace(_T("_"), _T(" ") );
  mapname.Replace(_T("-"), _T(" ") );
  return mapname;
}


wxString RefineModname( wxString modname )
{
  modname.Replace(_T("Absolute Annihilation"), _T("AA") );
  modname.Replace(_T("Complete Annihilation"), _T("CA") );
  modname.Replace(_T("Balanced Annihilation"), _T("BA") );
  modname.Replace(_T("Expand and Exterminate"), _T("EE") );
  modname.Replace(_T("War Evolution"), _T("WarEv") );
  modname.Replace(_T("TinyComm"), _T("TC") );
  modname.Replace(_T("BETA"), _T("b") );
  modname.Replace(_T("Public Alpha"), _T("pa") );
  modname.Replace(_T("Public Beta"), _T("pb") );
  modname.Replace(_T("Public"), _T("p") );
  modname.Replace(_T("Alpha"), _T("a") );
  modname.Replace(_T("Beta"), _T("b") );
  return modname;
}


wxString RTFtoText( wxString rtfinput )
{
  rtfinput = rtfinput.AfterFirst( '{' ).BeforeLast( '}' );

  rtfinput.Replace( _T("\\pard"), _T("") ); // remove a ambiguus char

  rtfinput.Replace( _T("\\par"), _T(" \n") ); // convert the end of lines

  wxString BeforeBrack = rtfinput.BeforeFirst( '{' );
  wxString AfterBrack = rtfinput.AfterLast( '}' );
  rtfinput = BeforeBrack + AfterBrack; // remove everyhting that matches { text }

  wxString out;
  while ( rtfinput.Find('\\') >= 0 ) //remove anything in the form \please\ignore\this
  {
    out += rtfinput.BeforeFirst( '\\' );
    rtfinput = rtfinput.AfterFirst ( '\\' );
    rtfinput = rtfinput.AfterFirst ( ' ' );
  } ;

  return out;
}

bool AreColoursSimilar( int r1, int g1, int b1, int r2, int g2, int b2, int mindiff )
{
  int r,g,b;
  r = r1 - r2;
  g = g1 - g2;
  b = b1 - b2;
  r = r>0?r:-r;
  g = g>0?g:-g;
  b = b>0?b:-b;
  if ( (r <= mindiff) && (g <= mindiff) && (b <= mindiff) ) return true;
  return false;
}


void ColourDelta( int& r, int& g, int& b, const int& delta )
{
  int tmpdelta;
  if ( delta > 0 ) {
    r += delta;
    tmpdelta = delta;
    if ( r > 255 ) {
      tmpdelta += r - 255;
      r = 255;
    }
    g += tmpdelta;
    tmpdelta = delta;
    if ( g > 255 ) {
      tmpdelta += g - 255;
      g = 255;
    }
    b += tmpdelta;
    if ( b > 255 ) b = 255;
  } else {
    r += delta;
    tmpdelta = -delta;
    if ( r < 0 ) {
      tmpdelta -= r;
      r = 0;
    }
    g -= tmpdelta;
    tmpdelta = -delta;
    if ( g < 0 ) {
      tmpdelta -= g;
      g = 0;
    }
    b -= tmpdelta;
    if ( b < 0 ) b = 0;
  }
}


wxColour ColourDelta( const wxColour& colour, const int& delta )
{
  int r = colour.Red();
  int g = colour.Green();
  int b = colour.Blue();
  ColourDelta( r, g, b, delta );
  return wxColour( r, g, b );
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
long
GetHostCPUSpeed() {

	long totalcpuspeed = 0;

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
	  }
	}

#else

	// Create an Inputstream from /proc/cpuinfo
	std::ifstream fin( "/proc/cpuinfo" );
	std::string line;
	std::string content_str( "" );

	// Read from Inputstream

	if ( fin ) {
		while ( std::getline( fin, line ) ) {
			// std::cout << "Read from file: " << line << std::endl;
			content_str.append( line );
			content_str.append( "\n" );
		}

		wxString content = wxString::FromAscii( content_str.c_str() );

		// Building a RegEx to match one Block of CPU Info
		wxRegEx regex_CPUSection( wxT( "processor.*?(cpu MHz.*?:.*?(\\d+\\.\\d+)).*?\n\n" ), wxRE_ADVANCED );
		// Replace each Block of CPU Info with only the CPU Speed in MHz
		regex_CPUSection.Replace( &content, _T( "\\2\n" ) );

		// Tokenize the String containing all CPU Speed of the Host: e.g. 3000.0\n3000.0\n
		wxStringTokenizer tokenlist( content, wxT( "\n" ) );

		// Sum up all CPU Speeds

		while ( tokenlist.HasMoreTokens() ) {
			wxString token = tokenlist.GetNextToken();
			long cpuspeed = 0;
			token.ToLong( &cpuspeed, 10 );
			totalcpuspeed += cpuspeed;
		}
	}

#endif

	return totalcpuspeed;
}
