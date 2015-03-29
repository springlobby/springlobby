/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "tasutil.h"
#include "conversion.h"

#include <wx/regex.h>

std::string GetWordParam(wxString& params)
{
	return STD_STRING(GetParamByChar(params, _T(' ')));
}


std::string GetSentenceParam(wxString& params)
{
	return STD_STRING(GetParamByChar(params, _T('\t')));
}


long GetIntParam(wxString& params)
{
	return s2l(GetParamByChar(params, _T(' ')));
}

wxString GetParamByChar(wxString& params, const wxChar& sep)
{
	int pos = params.Find(sep);
	wxString ret;
	if (pos != -1) {
		ret = params.Left(pos);
		params = params.Mid(pos + 1);
	} else {
		ret = params;
		params = wxEmptyString;
	}
	return ret;
}


bool GetBoolParam(wxString& params)
{
	return (bool)GetIntParam(params);
}

bool IsValidNickname(const wxString& _name)
{
	wxString name = _name;
	// The Regex Container
	//wxRegEx regex( wxT("[:graph:]") );
	wxRegEx regex(wxT("[ \t\r\n\v\föäüß, .:<>\\!§$%&+-]"));

	// We need to escape all regular Expression Characters, that have a special Meaning
	name.Replace(_T("["), wxEmptyString);
	name.Replace(_T("]"), wxEmptyString);

	if (name.IsEmpty())
		return false;

	return !regex.Matches(name);
}
