#ifndef SPRINGLOBBY_HEADERGUARD_UTILS_H
#define SPRINGLOBBY_HEADERGUARD_UTILS_H

#include <string>
#include <wx/log.h>

//! Converts a wxString to an std::string
#define STD_STRING(v) std::string((const char*)v.mb_str(wxConvUTF8))

//! Converts an std::string to a wxString
#define WX_STRING(v) wxString(v.c_str(),wxConvUTF8)

#define ASSERT_LOGIC(cond,msg) if(!(cond)){wxLogFatalError(_T("logic ")+ wxString(msg) );throw std::logic_error(msg);}
#define ASSERT_RUNTIME(cond,msg) if(!(cond)){wxLogDebug(_T("runtime ")+ wxString(msg) );throw std::runtime_error(msg);}

std::string i2s( int x );

void debug_output( const std::string& prefix, const std::string& func, const std::string& params, const std::string& msg );


#define debug_func( params ) wxLogTrace( __FUNCTION__ + _T(" ( ") + params + _T(" )"),,)


#define boundry(var,min,max) var=(var<(min))?(min):(var>(max))?(max):var

#ifdef __WXMSW__
#define CONTROL_HEIGHT 22
#else
#define CONTROL_HEIGHT 28
#endif


std::string GetWordParam( std::string& params );
std::string GetSentenceParam( std::string& params );
std::string GetChatLineParam( std::string& params );
int GetIntParam( std::string& params );
bool GetBoolParam( std::string& params );
std::string GetSpringLobbyVersion();

#endif // SPRINGLOBBY_HEADERGUARD_UTILS_H
