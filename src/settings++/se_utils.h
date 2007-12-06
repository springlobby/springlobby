#ifndef SE_UTILS_H_
#define SE_UTILS_H_

#include <wx/string.h>
#include <string>
#include <sstream>
//! Converts a wxString to an std::string
#define STD_STRING(v) std::string((const char*)v.mb_str(wxConvUTF8))

#define _S(v) wxString(v.c_str(),wxConvUTF8)

void toString(std::string& s, int t);

int fromString(const std::string& s);

#endif /*SE_UTILS_H_*/
