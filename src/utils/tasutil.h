/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_SERVERUTIL_H
#define SPRINGLOBBY_HEADERGUARD_SERVERUTIL_H

#include <wx/string.h>
#include <wx/wxchar.h>
class wxString;

std::string GetWordParam( wxString& params );
std::string GetSentenceParam( wxString& params );
long GetIntParam( wxString& params );
bool GetBoolParam( wxString& params );
wxString GetParamByChar( wxString& params, const wxChar& sep );

//! matches against regex for printable ascii chars, excluding space
bool IsValidNickname( const wxString& name );


#endif // SPRINGLOBBY_HEADERGUARD_SERVERUTIL_H
