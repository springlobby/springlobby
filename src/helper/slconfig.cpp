#include "slconfig.h"

#include <wx/filename.h>
#include <wx/wfstream.h>

#include "../utils/platform.h"
#include "../utils/conversion.h"

slConfig::slConfig ( const wxString& appName,
					 const wxString& vendorName,
					 const wxString& strLocal,
					 const wxString& strGlobal,
					 long style,
					 const wxMBConv& conv ):
	slConfigBaseType( appName, vendorName, strLocal, strGlobal, style, conv )
{
	// nop
}

#if wxUSE_STREAMS
slConfig::slConfig( wxInputStream& in, const wxMBConv& conv ):
	slConfigBaseType( in, conv )
{
	// nop
}
#endif // wxUSE_STREAMS

#ifdef __WXMSW__
bool slConfig::DoWriteLong( const wxString& key, long lValue )
{
	return slConfigBaseType::DoWriteString( key, TowxString<long>( lValue ) );
}
#endif

wxString slConfig::Read(const wxString& key, const wxString& defaultVal ) const
{
	//1. value from self, 2. default value
	wxString ret;
	if ( slConfigBaseType::Read( key, &ret ) )
		return ret;
	if ( IsRecordingDefaults() )
		((slConfig*)this)->Write( key, defaultVal );
	return defaultVal;
}

long slConfig::Read(const wxString& key, long defaultVal) const
{
	long ret;
	if ( slConfigBaseType::Read( key, &ret ) )
		return ret;
	if ( IsRecordingDefaults() )
		((slConfig*)this)->Write( key, defaultVal );
	return defaultVal;
}

bool slConfig::Read(const wxString& key, wxString* str) const
{
	//value was found in self -> return true
	if ( slConfigBaseType::Read( key, str ) )
		return true;
	return false;
}


bool slConfig::Read(const wxString& key, long* l) const
{
	if ( slConfigBaseType::Read( key, l ) )
		return true;
	if ( l && IsRecordingDefaults() )
		((slConfig*)this)->Write( key, *l );
	return false;
}


bool slConfig::Read(const wxString& key, double* d) const
{
	if ( slConfigBaseType::Read( key, d ) )
		return true;
	return false;
}

bool slConfig::Read(const wxString& key, bool* b) const
{
	if ( slConfigBaseType::Read( key, b ) )
		return true;
	return false;
}
