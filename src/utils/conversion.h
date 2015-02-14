/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_CONVERSION_H
#define SPRINGLOBBY_HEADERGUARD_CONVERSION_H

/** \name Type conversions
 **/

#ifdef _MSC_VER
typedef __int64 int64_t;
#endif

//! Converts a wxString to an std::string
#define STD_STRING(v) std::string((const char*)(v).mb_str(wxConvUTF8))

//! converts a wxString to a c string
#define C_STRING(v) (v).mb_str(wxConvUTF8)

#include <wx/string.h>
#include <sstream>

template<class T>
static inline wxString TowxString(T arg){
  std::stringstream s;
  s << arg;
  return wxString(s.str().c_str(),wxConvUTF8);
}

template<>
inline wxString TowxString(wxString arg){
  return arg;
}

template<>
inline wxString TowxString(const wxChar *arg){
  return wxString(arg);
}

template<class T>
inline T FromwxString(const wxString& arg){
  std::stringstream s;
  s << STD_STRING(arg);
  int64_t ret;
  s >> ret;
  return (T)ret;
}

#define WX_STRINGC(v) wxString(v,wxConvUTF8)

static inline long s2l( const wxString& arg )
{
    long ret;
    arg.ToLong(&ret);
    return ret;
}

/** @} */

#include <vector>
class wxStringTokenizer;
//! converts a string tokneizer into a vector of string
class StringtokenizerVectorized : public std::vector<wxString> {
    public:
        StringtokenizerVectorized( wxStringTokenizer tokenizer );
};

std::string stdprintf(const char* format, ...);


#endif // SPRINGLOBBY_HEADERGUARD_CONVERSION_H
