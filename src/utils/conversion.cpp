/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "conversion.h"

#include <wx/arrstr.h>
#include <wx/tokenzr.h>
#include <sstream>
#include <algorithm>

#if defined(__WIN32__) || defined(_MSC_VER)
#include <windows.h>
#endif

StringtokenizerVectorized::StringtokenizerVectorized(wxStringTokenizer tokenizer)
{
	reserve(tokenizer.CountTokens());
	while (tokenizer.HasMoreTokens())
		push_back(tokenizer.GetNextToken());
}

std::string stdprintf(const char* format, ...)
{
	char buf[1024];
	va_list args;
	va_start(args, format);
	const int count = vsnprintf(buf, 1024, format, args);
	va_end(args);
	if (count <= 0) {
		return "";
	}
	return std::string(buf, std::min(count, 1024));
}

//IMPORTANT NOTE: wxString( blah, wxConvUTF8 ) since wx2.9 returns empty string
//on Windows if "blah" contains non-english characters!
//wxString(std::string) works normally
wxString TowxString(const std::string& arg)
{
	wxString temproraryString = wxString(arg.c_str(), wxConvUTF8);
	return temproraryString.IsEmpty() ? wxString(arg) : temproraryString;
}

wxString TowxString(int arg)
{
	std::stringstream s;
	s << arg;
	return wxString(s.str().c_str(), wxConvUTF8);
}

long FromwxString(const wxString& arg)
{
	std::stringstream s;
	s << STD_STRING(arg);
	int64_t ret = 0l;
	s >> ret;
	return ret;
}


std::string strtolower(std::string str)
{
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
	return str;
}

#if defined(__WIN32__) || defined(_MSC_VER)
std::string Utf8ToLocalEncoding(const char* multyByteString)
{
	std::string res;
	int result_u, result_c;

	result_u = MultiByteToWideChar(CP_UTF8, 0, multyByteString, -1, 0, 0);

	if (!result_u) {
		return 0;
	}

	wchar_t* ures = new wchar_t[result_u];

	if (!MultiByteToWideChar(CP_UTF8, 0, multyByteString, -1, ures, result_u)) {
		delete[] ures;
		return 0;
	}


	result_c = WideCharToMultiByte(GetACP(), 0, ures, -1, 0, 0, 0, 0);

	if (!result_c) {
		delete[] ures;
		return 0;
	}

	char* cres = new char[result_c];

	if (!WideCharToMultiByte(GetACP(), 0, ures, -1, cres, result_c, 0, 0)) {
		delete[] ures;
		delete[] cres;
		return 0;
	}

	delete[] ures;
	res.append(cres);
	delete[] cres;
	return res;
}
#endif
