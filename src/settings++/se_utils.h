#ifndef SE_UTILS_H_
#define SE_UTILS_H_
#include <wx/string.h>
#include <sstream>
#include <string>
//! Converts a wxString to an std::string
#define STD_STRING(v) std::string((const char*)v.mb_str(wxConvUTF8))

#define _S(v) wxString(v.c_str(),wxConvUTF8)

//inline wxString _S (const std::string str);

template<typename Type> void toString(std::string& s, Type t) {
	//using namespace std;
			std::stringstream ss;
			ss << t;
			ss >> s;
}

template<typename Type> Type fromString(const std::string& s) {
        std::stringstream ss;
        ss << s;
        Type r;
        ss >> r;
        return r;
}

#endif /*SE_UTILS_H_*/
