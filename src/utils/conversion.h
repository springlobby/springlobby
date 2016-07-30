/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_CONVERSION_H
#define SPRINGLOBBY_HEADERGUARD_CONVERSION_H

/** \name Type conversions
 **/

//! Converts a wxString to an std::string
#define STD_STRING(v) std::string((const char*)(v).mb_str(wxConvUTF8))

//! converts a wxString to a c string
#define C_STRING(v) (v).mb_str(wxConvUTF8)

#include <wx/string.h>
#include <string>


wxString TowxString(const std::string& arg);
wxString TowxString(int);
std::string strtolower(std::string str);

#if defined(__WIN32__) || defined(_MSC_VER)
std::string Utf8ToLocalEncoding(const char *str);
#endif

long FromwxString(const wxString& arg);

/** @} */

#include <vector>
class wxStringTokenizer;
//! converts a string tokneizer into a vector of string
class StringtokenizerVectorized : public std::vector<wxString>
{
public:
	explicit StringtokenizerVectorized(wxStringTokenizer tokenizer);
};

std::string stdprintf(const char* format, ...);


#endif // SPRINGLOBBY_HEADERGUARD_CONVERSION_H
