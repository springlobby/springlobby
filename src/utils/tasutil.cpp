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
	return FromwxString(GetParamByChar(params, _T(' ')));
}

wxString GetParamByChar(wxString& params, const wxChar& sep)
{
	const int pos = params.Find(sep);
	if (pos == wxNOT_FOUND) { // no separator found, return params
		const wxString ret = params;
		params.clear();
		return ret;
	}
	const wxString ret = params.Left(pos); //separator found, return before sep
	params = params.Mid(pos + 1); //return everything after first seperator
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
