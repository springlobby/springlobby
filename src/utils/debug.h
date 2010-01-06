#ifndef SPRINGLOBBY_HEADERGUARD_DEBUG_H
#define SPRINGLOBBY_HEADERGUARD_DEBUG_H

#include <stdexcept>

class assert_exception : public std::runtime_error
{
  public:
   assert_exception(std::string msg) : std::runtime_error(msg) {};
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


#endif // SPRINGLOBBY_HEADERGUARD_DEBUG_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-09

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    SpringLobby is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/
