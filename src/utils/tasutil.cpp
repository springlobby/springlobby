/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "tasutil.h"
#include <lslutils/conversion.h>
#include <lslutils/misc.h>

#include <wx/regex.h>

std::string GetWordParam(std::string& params)
{
	return GetParamByChar(params, ' ');
}


std::string GetSentenceParam(std::string& params)
{
	return GetParamByChar(params, '\t');
}


long GetIntParam(std::string& params)
{
	return LSL::Util::FromLongString(GetParamByChar(params, ' '));
}

std::string GetParamByChar(std::string& params, const char sep)
{
	const size_t pos = params.find(sep);
	if (pos == std::string::npos) { // no separator found, return params
		const std::string ret = params;
		params.clear();
		return ret;
	}
	const std::string ret = params.substr(0, pos); //separator found, return before sep
	params = params.substr(pos + 1);	  //return everything after first seperator
	return ret;
}


bool GetBoolParam(std::string& params)
{
	return (bool)GetIntParam(params);
}

bool IsValidNickname(const std::string& _name)
{
	std::string name = _name;
	// The Regex Container
	//wxRegEx regex( wxT("[:graph:]") );
	wxRegEx regex(wxT("[ \t\r\n\v\föäüß, .:<>\\!§$%&+-]"));

	// We need to escape all regular Expression Characters, that have a special Meaning
	name = LSL::Util::Replace(name, "[", "");
	name = LSL::Util::Replace(name, "]", "");

	if (name.empty())
		return false;

	return !regex.Matches(name);
}
