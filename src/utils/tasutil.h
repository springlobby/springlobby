#ifndef SPRINGLOBBY_HEADERGUARD_SERVERUTIL_H
#define SPRINGLOBBY_HEADERGUARD_SERVERUTIL_H

#include <wx/string.h>
#include <wx/wxchar.h>
class wxString;

wxString GetWordParam( wxString& params );
wxString GetSentenceParam( wxString& params );
long GetIntParam( wxString& params );
bool GetBoolParam( wxString& params );
wxString GetParamByChar( wxString& params, const wxChar& sep );

//! matches against regex for printable ascii chars, excluding space
bool IsValidNickname( const wxString& name );


#endif // SPRINGLOBBY_HEADERGUARD_SERVERUTIL_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2010

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
