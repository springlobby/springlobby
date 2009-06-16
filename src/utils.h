#ifndef SPRINGLOBBY_HEADERGUARD_UTILS_H
#define SPRINGLOBBY_HEADERGUARD_UTILS_H

#include <wx/string.h>
#include <sstream>
#include <stdexcept>
#include <wx/log.h>

/** these need to stay to not break non-autotools builds */
#if ( !defined(HAVE_WX26) && !defined(HAVE_WX28) )
	#if( wxMAJOR_VERSION==2 && wxMINOR_VERSION == 6 )
		#define HAVE_WX26
	#elif( wxMAJOR_VERSION==2 && wxMINOR_VERSION == 8 )
		#define HAVE_WX28
	#endif
#endif


#ifndef VERSION
	#define VERSION "unknown"
#endif

class assert_exception : public std::runtime_error
{
  public:
   assert_exception(std::string msg) : std::runtime_error(msg) {};
};

#ifndef __WXDEBUG__
#define wxLogDebugFunc( params ) wxLogVerbose( _T("%s"), wxString(wxString(__FUNCTION__, wxConvUTF8 ) + _T(" ( ") + wxString(params) + _T(" )")).c_str() )
#else
#define wxLogDebugFunc( params ) wxLogTrace(_T("function calls"), params )
#endif


//! Converts an std::string to a wxString
#define WX_STRING(v) wxString(v.c_str(),wxConvUTF8)
#define WX_STRINGC(v) wxString(v,wxConvUTF8)


//! Converts a wxString to an std::string
#define STD_STRING(v) std::string((const char*)(v).mb_str(wxConvUTF8))

#if wxUSE_DEBUGREPORT && defined(HAVE_WX28)
#define ASSERT_LOGIC(cond,msg) if(!(cond))\
{\
  wxLogError(_T("logic error ( %s:%d ): %s"), TowxString(__FILE__).c_str(),__LINE__ , wxString(msg).c_str() );\
  throw std::logic_error(std::string(wxString(msg).mb_str()));\
}
#else
#define ASSERT_LOGIC(cond,msg) if(!(cond))\
{\
  wxLogError(_T("logic error ( %s:%d ): %s"), TowxString(__FILE__).c_str(),__LINE__ , wxString(msg).c_str() );\
  throw std::logic_error(std::string(wxString(msg).mb_str()));\
}
#endif

#define ASSERT_EXCEPTION(cond,msg) if(!(cond))\
{wxLogMessage(_T("runtime assertion ( %s:%d ): %s"), TowxString(__FILE__).c_str(),__LINE__ , wxString(msg).c_str() );throw assert_exception(std::string(wxString(msg).mb_str()));}


/** @todo convert to a templated function */
#define CLAMP(var,min,max)    \
  ( (var) =		      \
    ( (var) < (min)	      \
      ? (min)		      \
      : ( (var) > (max)	      \
	  ? (max)	      \
	  : (var)	      \
	)		      \
    )			      \
  )

#ifdef __WXMSW__
#define CONTROL_HEIGHT 22
#else
#define CONTROL_HEIGHT 28
#endif

#define IsColourOk() IsOk()

/** \name Type conversions
 * @{ */
//!@brief converts integers to wxString
wxString i2s( int arg );
//!@brief converts unsigned int to wxString
wxString u2s( unsigned int arg );
//!@brief converts floating point numbers to wxString without problem of WTF decimal separator different in every locale
wxString f2s( float arg );

long s2l( const wxString& arg );
double s2d( const wxString& arg );

template<class T>
wxString TowxString(T arg){
  std::stringstream s;
  s << arg;
  return WX_STRING( s.str() );
}

inline wxString TowxString(wxString arg){
  return arg;
}
inline wxString TowxString(const wxChar *arg){
  return wxString(arg);
}
inline wxString TowxString(std::string arg){
  return WX_STRING(arg);
}
/** @} */

template <typename T> T clamp(const T var,const T min,const T max)
{ return ( (var < min) ? min : ( var > max ) ? max : var ); }

template<typename T>
T min(T a, T b, T c)
{
    return std::min(a, std::min(b, c));
}

wxString GetLibExtension();
void InitializeLoggingTargets( wxFrame* parent, bool console, bool showgui, bool logcrash, int verbosity );
wxString GetWordParam( wxString& params );
wxString GetSentenceParam( wxString& params );
long GetIntParam( wxString& params );
bool GetBoolParam( wxString& params );
wxString GetParamByChar( wxString& params, const wxChar& sep );
wxString GetSpringLobbyVersion();
wxString GetExecutableFolder();
wxString MakeHashUnsigned( const wxString& hash );
wxString MakeHashSigned( const wxString& hash );

//! matches against regex for printable ascii chars, excluding space
bool IsValidNickname( const wxString& name );

wxString GetHostCPUSpeed();

static inline int CompareStringIgnoreCase(const wxString& first, const wxString& second)
{
    return (first.Upper() > second.Upper() );
}


/** @brief Array with runtime determined size which is not initialized on creation.

This RAII type is ment as output buffer for interfaces with e.g. C, where
initializing a temp buffer to zero is waste of time because it gets overwritten
with real data anyway.

It's ment as replacement for the error prone pattern of allocating scratch/buffer
memory using new/delete and using a std::vector "cast" to a C style array.
*/
template<typename T>
class uninitialized_array
{
  public:
    uninitialized_array(int numElem)
    : elems( reinterpret_cast<T*>( operator new[]( numElem * sizeof(T) ) ) ) {
    }
    ~uninitialized_array() {
      operator delete[]( elems );
    }

    /// this opens the door to basically any operation allowed on C style arrays
    operator T*() { return elems; }

  private:
    T* elems;

	// copying not allowed
	uninitialized_array(const uninitialized_array&);
	uninitialized_array& operator=(const uninitialized_array&);
};


#define TE(v) TooltipEnable(v)

/** @brief when setting tooltips use output of this func as input
    it outputs empty string if tooltips are disabled in SL settings
*/
const wxChar* TooltipEnable(const wxChar* input);



/**
 * @brief Computes Levenshtein distance (edit distance) between two strings.
 * @return the Levenshtein distance normalized by the longest string's length.
 * @note Source: http://en.wikipedia.org/wiki/Levenshtein_distance
 */
double LevenshteinDistance(wxString s, wxString t);

class wxArrayString;
/**
 * @brief Gets the closest match for s in a, using LevenshteinDistance.
 * @param distance If not NULL, *distance is set to the edit distance from s to the return value.
 */
wxString GetBestMatch(const wxArrayString& a, const wxString& s, double* distance = NULL);

//! convert wxArrayString into a wxString[] which must be delete[]d by caller
int ConvertWXArrayToC(const wxArrayString& aChoices, wxString **choices);

/**
    let origin be /path/to/some/dir and destination /some/other/path
    this will copy dir (and everything below that recursively to /some/other/path/dir
    \return true if successful
*/
bool CopyDir( wxString origin, wxString destination, bool overwrite = true);
#endif // SPRINGLOBBY_HEADERGUARD_UTILS_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-09

    springsettings is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

