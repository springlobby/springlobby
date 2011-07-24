/**
    This file is part of springsettings,
    Copyright (C) 2007-2011

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    SpringLobby is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with springsettings.  If not, see <http://www.gnu.org/licenses/>.
**/

#ifndef SE_UTILS_H_
#define SE_UTILS_H_

#include <wx/string.h>
#include <sstream>
/** \name SPringSettings Utils
 * these are necessary duplications for standalone mode atm
 * \todo can prolly be avoided if properly use code from SL
 * @{ */

int fromString(const wxString& s);

void openUrl(const wxString& url);

template<class T>
wxString towxString(T arg){
  std::stringstream s;
  s << arg;
  return wxString(s.str().c_str(),wxConvUTF8);
}
/** @} */

bool IsSettingsStandAlone();
void SetSettingsStandAlone( bool value );

#endif /*SE_UTILS_H_*/

