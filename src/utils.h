
#ifndef _UTILS_H_
#define _UTILS_H_

//! Converts a wxString to an stl string
#define STL_STRING(v) std::string((const char*)v.mb_str(wxConvUTF8))

//! Converts an stl string to a wxString
#define WX_STRING(v) wxString(v.c_str(),wxConvUTF8)


#endif  //_UTILS_H_

