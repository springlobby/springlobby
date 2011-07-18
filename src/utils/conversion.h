#ifndef SPRINGLOBBY_HEADERGUARD_CONVERSION_H
#define SPRINGLOBBY_HEADERGUARD_CONVERSION_H

/** \name Type conversions
 **/
 //! Converts an std::string to a wxString
//static inline wxString WX_STRING( const std::string& v ) {
//    return wxString(v.c_str(),wxConvUTF8);
//}

#ifdef _MSC_VER
typedef __int64 int64_t;
#endif

//! Converts a wxString to an std::string
#define STD_STRING(v) std::string((const char*)(v).mb_str(wxConvUTF8))

//! converts a wxString to a c string
#define C_STRING(v) (v).mb_str(wxConvUTF8)

#include <wx/string.h>
#include <sstream>

#include <wx/arrstr.h>

template<class T>
static inline wxString TowxString(T arg){
  std::stringstream s;
  s << arg;
  return wxString(s.str().c_str(),wxConvUTF8);
}

//template<>
//inline wxString TowxString(const std::string& arg){
//  return wxString( arg.c_str(), wxConvUTF8 );
//}

template<>
inline wxString TowxString(wxString arg){
  return arg;
}

template<>
inline wxString TowxString(const wxChar *arg){
  return wxString(arg);
}

//static inline wxString TowxString(){
//  return wxString();
//}

template<class T>
inline T FromwxString(const wxString& arg){
  std::stringstream s;
  s << STD_STRING(arg);
  int64_t ret;
  s >> ret;
  return (T)ret;
}

#ifdef SL_QT_MODE
#include <QString>
#include <QVariant>
template<>
inline QVariant FromwxString(const wxString& arg) {
	return QVariant::fromValue( QString(arg.mb_str()) );
}

template<>
inline wxString TowxString(QString arg){
  return wxString(arg.toStdString().c_str(),wxConvUTF8);
}


//template<>
static inline QString ToQString(const wxString& arg){
  return QString( arg.mbc_str() );
}
#endif

#define WX_STRINGC(v) wxString(v,wxConvUTF8)

static inline long s2l( const wxString& arg )
{
    long ret;
    arg.ToLong(&ret);
    return ret;
}

static inline double s2d( const wxString& arg )
{
    double ret;
    arg.ToDouble(&ret);
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

//! convert wxArrayString into a wxString[] which must be delete[]d by caller
int ConvertWXArrayToC(const wxArrayString& aChoices, wxString **choices);

//! is a copy of given wxArrayString with a function applied on each element
class TransformedArrayString : public wxArrayString {
    public:
        TransformedArrayString( const wxArrayString& original, wxString trans_op (const wxString& ) );
};

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


#endif // SPRINGLOBBY_HEADERGUARD_CONVERSION_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2011

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    SpringLobby is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/
