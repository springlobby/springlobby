#ifndef SPRINGLOBBY_HEADERGUARD_UTILS_H
#define SPRINGLOBBY_HEADERGUARD_UTILS_H

#include <string>
#include <wx/log.h>

#ifndef __WXDEBUG__
#define wxLogDebugFunc( params ) wxLogMessage( wxString(__FUNCTION__, wxConvUTF8 ) + _T(" ( ") + wxString(params) + _T(" )") )
#else
#define wxLogDebugFunc( params ) wxLogTrace( params )
#endif

//! Converts a wxString to an std::string
#define STD_STRING(v) std::string((const char*)(v).mb_str(wxConvUTF8))

//! Converts an std::string to a wxString
#define WX_STRING(v) wxString((v).c_str(),wxConvUTF8)

#define ASSERT_LOGIC(cond,msg) if(!(cond)){wxLogFatalError(_T("logic error: ")+ wxString(msg) + _T("StackTrace avilable in stdout") ); DumpStackTraceToLog(); throw std::logic_error(std::string(wxString(msg).mb_str()));}
#define ASSERT_RUNTIME(cond,msg) if(!(cond)){wxLogMessage(_T("runtime error: ")+ wxString(msg) );throw std::runtime_error(std::string(wxString(msg).mb_str()));}

std::string i2s( int x );


#define boundry(var,min,max) var=(var<(min))?(min):(var>(max))?(max):var

#ifdef __WXMSW__
#define CONTROL_HEIGHT 22
#else
#define CONTROL_HEIGHT 28
#endif


void DumpStackTraceToLog();
std::string GetWordParam( std::string& params );
std::string GetSentenceParam( std::string& params );
std::string GetChatLineParam( std::string& params );
int GetIntParam( std::string& params );
bool GetBoolParam( std::string& params );
std::string GetSpringLobbyVersion();

#endif // SPRINGLOBBY_HEADERGUARD_UTILS_H
