//
// File: utils.h
//

#include "utils.h"

#ifdef __WXMSW__
#  include <windows.h> 
#endif

std::string i2s( int x )
{
  std::ostringstream o;
  o << x;
  return o.str();
}

void debug_msg( std::string msg )
{
#ifdef __WXMSW__
  std::string tmp = " -- ";
  tmp += msg;
  OutputDebugString( tmp.c_str() );
#else
  std::cout << " -- " << msg.c_str() << std::endl;
#endif
}

void debug_func( const std::string& func, const std::string& params = "" )
{
#ifdef __WXMSW__
  std::string tmp = " -- ";
  tmp += func;
  OutputDebugString( tmp.c_str() );
#else
  std::cout << " -- " << msg.c_str() << std::endl;
#endif
}

void debug_warning( std::string msg )
{
#ifdef __WXMSW__
  std::string tmp = " ww ";
  tmp += msg;
  OutputDebugString( tmp.c_str() );
#else
  std::cout << " ww " << msg.c_str() << std::endl;
#endif
}

void debug_error( std::string msg )
{
#ifdef __WXMSW__
  std::string tmp = " !! ";
  tmp += msg;
  OutputDebugString( tmp.c_str() );
#else
  std::cout << " !! " << msg.c_str() << std::endl;
#endif
}
