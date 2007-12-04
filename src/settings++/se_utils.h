#ifndef SE_UTILS_H_
#define SE_UTILS_H_

//! Converts a wxString to an std::string
#define STD_STRING(v) std::string((const char*)v.mb_str(wxConvUTF8))

inline wxString _S (const std::string str);

template<typename Type> void toString(std::string& s, Type t);

template<typename Type> Type fromString(const std::string& s);

#endif /*SE_UTILS_H_*/
