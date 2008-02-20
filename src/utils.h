#ifndef SPRINGLOBBY_HEADERGUARD_UTILS_H
#define SPRINGLOBBY_HEADERGUARD_UTILS_H

#include <wx/string.h>
#include <wx/log.h>

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


#define ASSERT_LOGIC(cond,msg) if(!(cond)){wxLogError(_T("logic error: %s"), wxString(msg).c_str() ); throw std::logic_error(std::string(wxString(msg).mb_str()));}
#define ASSERT_RUNTIME(cond,msg) if(!(cond)){wxLogMessage(_T("runtime error: %s"), wxString(msg).c_str() );throw std::runtime_error(std::string(wxString(msg).mb_str()));}


#define boundry(var,min,max) var=(var<(min))?(min):(var>(max))?(max):var

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

wxString GetLibExtension();
void InitializeLoggingTargets( bool console, bool showgui, bool logcrash, int verbosity );
wxString GetWordParam( wxString& params );
wxString GetSentenceParam( wxString& params );
long GetIntParam( wxString& params );
bool GetBoolParam( wxString& params );
wxString GetSpringLobbyVersion();

#endif // SPRINGLOBBY_HEADERGUARD_UTILS_H
