/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// File: utils.h
//

#include <sstream>

#include "utils.h"

// FIXME this does not work on linux+mingw build for windows
#ifdef _MSC_VER
#include <windows.h>
#include <wx/msw/winundef.h>
#endif
//#else
#include <iostream>
#include <ostream>
//#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

std::string i2s( int x )
{
  std::ostringstream o;
  o << x;
  return o.str();
}

void debug_output( const std::string& prefix, const std::string& func, const std::string& params, const std::string& msg )
{
  std::string tmpmsg = msg;
  if ( msg != "" ) tmpmsg = std::string(": ") + msg;
#ifdef __WXMSW__
  std::string tmp = prefix;
  tmp += " " + func + "( " + params + " )" + tmpmsg + "\n";
#ifdef _MSC_VER
  OutputDebugString( tmp.c_str() );
#endif

#else
  std::cout << prefix.c_str() << " " << func.c_str() << "( " << params.c_str() << " ): "<< tmpmsg.c_str() << std::endl;
#endif
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
  return VERSION;
#else
  return "Unknown";
#endif
}