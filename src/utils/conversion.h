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

static inline wxString MakeHashUnsigned( const wxString& hash )
{
	return TowxString( FromwxString<unsigned int>( hash ) );
}

static inline wxString MakeHashSigned( const wxString& hash )
{
	return TowxString( FromwxString<int>( hash ) );
}

#include <vector>
class wxStringTokenizer;
//! converts a string tokneizer into a vector of string
class StringtokenizerVectorized : public std::vector<wxString> {
    public:
        StringtokenizerVectorized( wxStringTokenizer tokenizer );
};

#include <boost/format.hpp>

//! a wxString extension class to boost::format
struct wxFormat : public boost::format
{
	explicit wxFormat( const wxString& format )
		: boost::format( STD_STRING( format ) )
	{}

	explicit wxFormat( const std::string& format )
		: boost::format( format )
	{}

	explicit wxFormat(const char* format)
		: boost::format(format)
	{}

	wxString str() const
	{
		return TowxString( boost::format::str() );
	}

	wxString c_str() const
	{
		return TowxString( boost::format::str() ).c_str();
	}

	//! conversion operator to wxString
	operator wxString () const
	{
		return str();
	}

    //! conversion operator to wxString
    operator std::string () const
    {
        return boost::format::str();
    }

	//! overload the base class % operator to accept wxString input (and return our own type again)
	template <class T>
	wxFormat&  operator%(const T& x)
	{
		return static_cast<wxFormat&>( boost::format::operator % ( x ) );
	}
	//! this signature is needed to be able to specialize on wxChar and apply conversion
	template <class T>
	wxFormat&  operator%(const T* x)
	{
		return static_cast<wxFormat&>( boost::format::operator % ( x ) );
	}

};

template <>
inline wxFormat&  wxFormat::operator%(const wxString& x)
{
	return static_cast<wxFormat&>( boost::format::operator % ( STD_STRING(x) ) );
}

template <>
inline wxFormat&  wxFormat::operator%(const wxChar* x)
{
	return operator % ( wxString(x) );
}

std::string stdprintf(const char* format, ...);


#endif // SPRINGLOBBY_HEADERGUARD_CONVERSION_H
