/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */

#include <wx/colour.h>
#include <wx/regex.h>

#include "uiutils.h"


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
  wxRegEx StripParenthesis = wxRegEx ( _T("{ * "), wxRE_ADVANCED ); // remove everything in {}, excluding the first level
  if ( StripParenthesis.Matches( rtfinput ) ) StripParenthesis.ReplaceAll( rtfinput, _T("") );

  wxReg StripAfterSlash = wxRegEx ( _T("'\'"), wxRE_ADVANCED ); //ignore anything in the form \please\ignore\this
  if ( StripAfterSlash.Matches( rtfinput ) StripAfterSlash.ReplaceAll( rtfinput, _T("") );

  rtfinput.Replace( _T("{"), _T("") ); //strip inital & final parenthesis
  rtfinput.Replace( _T("}"), _T("") );

  return rtfinput;
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

