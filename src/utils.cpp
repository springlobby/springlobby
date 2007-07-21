//
// File: utils.h
//

#include <sstream>

#include "utils.h"

// FIXME this does not work on linux+mingw build for windows
//#ifdef __WXMSW__
//#include <windows.h>
//#include <wx/msw/winundef.h>
//#else
#include <iostream>
#include <ostream>
//#endif

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
// FIXME use wx logging thingies instead for portability
//  OutputDebugString( tmp.c_str() );
#else
  std::cout << prefix.c_str() << " " << func.c_str() << "( " << params.c_str() << " ): "<< tmpmsg.c_str() << std::endl;
#endif
}
