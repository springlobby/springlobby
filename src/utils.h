#ifndef SPRINGLOBBY_HEADERGUARD_UTILS_H
#define SPRINGLOBBY_HEADERGUARD_UTILS_H

#include <wx/string.h>
#include <wx/log.h>
#include <sstream>

#ifndef __WXDEBUG__
#define wxLogDebugFunc( params ) wxLogVerbose( _T("%s"), wxString(wxString(__FUNCTION__, wxConvUTF8 ) + _T(" ( ") + wxString(params) + _T(" )")).c_str() )
#else
#define wxLogDebugFunc( params ) wxLogTrace(_T("function calls"), params )
#endif

#if ( !defined(HAVE_WX26) && !defined(HAVE_WX28) )
#if( wxMAJOR_VERSION==2 && wxMINOR_VERSION == 6 )
#define HAVE_WX26
#elif( wxMAJOR_VERSION==2 && wxMINOR_VERSION == 8 )
#define HAVE_WX28
#endif
#endif

#ifndef VERSION
#define VERSION "Unknown"
#endif

//! Converts an std::string to a wxString
#define WX_STRING(v) wxString(v.c_str(),wxConvUTF8)
#define WX_STRINGC(v) wxString(v,wxConvUTF8)


//! Converts a wxString to an std::string
#define STD_STRING(v) std::string((const char*)(v).mb_str(wxConvUTF8))

#if wxUSE_DEBUGREPORT && defined(HAVE_WX28)
#define ASSERT_LOGIC(cond,msg) if(!(cond))\
{\
  wxLogError(_T("logic error: %s"), wxString(msg).c_str() );\
  throw std::logic_error(std::string(wxString(msg).mb_str()));\
}
#else
#define ASSERT_LOGIC(cond,msg) if(!(cond))\
{\
  wxLogError(_T("logic error: %s"), wxString(msg).c_str() );\
  throw std::logic_error(std::string(wxString(msg).mb_str()));\
}
#endif

#define ASSERT_RUNTIME(cond,msg) if(!(cond)){wxLogMessage(_T("runtime error: %s"), wxString(msg).c_str() );throw std::runtime_error(std::string(wxString(msg).mb_str()));}


#define CLAMP(var,min,max) ((var)=((var)<(min))?(min):((var)>(max))?(max):(var))

#ifdef __WXMSW__
#define CONTROL_HEIGHT 22
#else
#define CONTROL_HEIGHT 28
#endif

#ifdef HAVE_WX26
#define IsColourOk() Ok()
#else
#define IsColourOk() IsOk()
#endif

//!@brief converts integers to wxString
wxString i2s( int arg );
//!@brief converts unsigned int to wxString
wxString u2s( unsigned int arg );
//!@brief converts floating point numbers to wxString without problem of WTF decimal separator different in every locale
wxString f2s( float arg );
/// new, much improved way to convert stuff to wxString.
template<class T>
wxString TowxString(T arg){
  std::stringstream s;
  s << arg;
  return WX_STRING( s.str() );
}
inline wxString TowxString(wxString arg){
  return arg;
}
inline wxString TowxString(const wxChar *arg){
  return wxString(arg);
}
inline wxString TowxString(std::string arg){
  return WX_STRING(arg);
}



wxString GetLibExtension();
void InitializeLoggingTargets();
wxString GetWordParam( wxString& params );
wxString GetSentenceParam( wxString& params );
long GetIntParam( wxString& params );
bool GetBoolParam( wxString& params );
wxString GetSpringLobbyVersion();

wxString GetHostCPUSpeed();

#endif // SPRINGLOBBY_HEADERGUARD_UTILS_H
