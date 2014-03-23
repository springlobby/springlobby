/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_DEBUG_H
#define SPRINGLOBBY_HEADERGUARD_DEBUG_H

#include <stdexcept>
#include <wx/log.h>

class assert_exception : public std::runtime_error
{
  public:
   assert_exception(std::string msg) : std::runtime_error(msg) {}
};

#ifndef __WXDEBUG__
#define wxLogDebugFunc( params ) wxLogVerbose( _T("%s"), wxString(wxString(__FUNCTION__, wxConvUTF8 ) + _T(" ( ") + wxString(params) + _T(" )")).c_str() )
#else
#define wxLogDebugFunc( params ) wxLogTrace(_T("function calls"), params )
#endif

#if wxUSE_DEBUGREPORT
#define ASSERT_LOGIC(cond,msg) if(!(cond))\
{\
  wxLogError(_T("logic error ( %s:%d ): %s"), TowxString(__FILE__).c_str(),__LINE__ , wxString(msg).c_str() );\
  throw std::logic_error(std::string(wxString(msg).mb_str()));\
}
#else
#define ASSERT_LOGIC(cond,msg) if(!(cond))\
{\
  wxLogError(_T("logic error ( %s:%d ): %s"), TowxString(__FILE__).c_str(),__LINE__ , wxString(msg).c_str() );\
  throw std::logic_error(std::string(wxString(msg).mb_str()));\
}
#endif

#define ASSERT_EXCEPTION(cond,msg) if(!(cond))\
{wxLogMessage(_T("runtime assertion ( %s:%d ): %s"), TowxString(__FILE__).c_str(),__LINE__ , wxString(msg).c_str() );throw assert_exception(std::string(wxString(msg).mb_str()));}

#define CATCH_ANY catch(...)\
{wxLogMessage(_T("%s-%d: undiff exception"),TowxString(__FILE__).c_str(),__LINE__ );}


#endif // SPRINGLOBBY_HEADERGUARD_DEBUG_H
