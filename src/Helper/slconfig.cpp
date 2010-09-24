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
					 const wxMBConv& conv )
	: slConfigBaseType( appName, vendorName, strLocal, strGlobal, style, conv ),
	m_global_config( 0 )
{
	SetupGlobalconfig();
}

#if wxUSE_STREAMS
slConfig::slConfig( wxInputStream& in, const wxMBConv& conv )
	: slConfigBaseType( in, conv ),
	m_global_config( 0 )
{
	SetupGlobalconfig();
}
#endif // wxUSE_STREAMS

void slConfig::SetupGlobalconfig()
{
	#ifdef __WXMSW__
		wxString p (wxFileName::GetPathSeparator());
		wxString global_config_path = wxString::Format( _T("%s%s%s.global.conf"),
													   GetExecutableFolder().c_str(),
													   p.c_str(),
													   ::GetAppName( true ).c_str()
													   );
	#else
		wxString global_config_path = IdentityString( _T("/etc/default/%s.conf") );
	#endif //__WXMSW__

	if (  wxFileName::FileExists( global_config_path ) )
	{
		wxFileInputStream instream( global_config_path );
		if ( instream.IsOk() )
		{
			m_global_config = new wxFileConfig ( instream );
			m_global_config->SetRecordDefaults( false );
		}
	}
}

#ifdef __WXMSW__
bool slConfig::DoWriteLong( const wxString& key, long lValue )
{
	return slConfigBaseType::DoWriteString( key, TowxString<long>( lValue ) );
}
#endif

wxString slConfig::Read(const wxString& key, const wxString& defaultVal ) const
{
	//1. value from self, 2. value from global, 3. default value
	wxString ret;
	if ( slConfigBaseType::Read( key, &ret ) )
		return ret;
	else if ( m_global_config && m_global_config->Read( key, &ret ) )
	{
		//basically this means we're treating the global value as a default to our self
		//and write it according to policy
		if ( IsRecordingDefaults() )
			((slConfigBaseType*)this)->Write( key, ret );
		return ret;
	}
	if ( IsRecordingDefaults() )
		((slConfigBaseType*)this)->Write( key, defaultVal );
	return defaultVal;
}

long slConfig::Read(const wxString& key, long defaultVal) const
{
	long ret;
	if ( slConfigBaseType::Read( key, &ret ) )
		return ret;
	else if ( m_global_config && m_global_config->Read( key, &ret ) )
	{
		if ( IsRecordingDefaults() )
			((slConfigBaseType*)this)->Write( key, ret );
		return ret;
	}
	if ( IsRecordingDefaults() )
		((slConfigBaseType*)this)->Write( key, defaultVal );
	return defaultVal;
}

bool slConfig::Read(const wxString& key, wxString* str) const
{
	//value was found in self -> return true
	if ( slConfigBaseType::Read( key, str ) )
		return true;
	//value was found in global. while technically this constitutes using a default value
	//we still return true since no default value was passed as argument to the original call
	else if ( m_global_config && m_global_config->Read( key, str ) )
	{
		if ( str && IsRecordingDefaults() )
			((slConfigBaseType*)this)->Write( key, *str );
		return true;
	}
	return false;
}

bool slConfig::Read(const wxString& key, wxString* str, const wxString& defaultVal) const
{
	// do not pass the default to this first call since it might write it back immeadiately
	if ( slConfigBaseType::Read( key, str ) )
		return true;
	else if ( m_global_config && m_global_config->Read( key, str, defaultVal ) )
		return true;//means the default was not used

	//at this point the last call will have modified *str with defaultVal
	if ( IsRecordingDefaults() )
		((slConfigBaseType*)this)->Write( key, defaultVal );
	return false;
}

bool slConfig::Read(const wxString& key, long* l) const
{
	if ( slConfigBaseType::Read( key, l ) )
		return true;
	else if ( m_global_config && m_global_config->Read( key, l ) )
	{
		if ( l && IsRecordingDefaults() )
			((slConfigBaseType*)this)->Write( key, *l );
		return true;
	}
	return false;
}

bool slConfig::Read(const wxString& key, long* l, long defaultVal) const
{
	if ( slConfigBaseType::Read( key, l, defaultVal ) )
		return true;
	else if ( m_global_config && m_global_config->Read( key, l, defaultVal ) )
		return true;
	if ( IsRecordingDefaults() )
		((slConfigBaseType*)this)->Write( key, defaultVal );
	return false;
}

bool slConfig::Read(const wxString& key, double* d) const
{
	if ( slConfigBaseType::Read( key, d ) )
		return true;
	else if ( m_global_config && m_global_config->Read( key, d ) )
		return true;
	return false;
}

bool slConfig::Read(const wxString& key, double* d, double defaultVal) const
{
	if ( slConfigBaseType::Read( key, d, defaultVal ) )
		return true;
	else if ( m_global_config && m_global_config->Read( key, d, defaultVal ) )
		return true;
	if ( IsRecordingDefaults() )
		((slConfigBaseType*)this)->Write( key, defaultVal );
	return false;
}

bool slConfig::Read(const wxString& key, bool* b) const
{
	if ( slConfigBaseType::Read( key, b ) )
		return true;
	else if ( m_global_config && m_global_config->Read( key, b ) )
		return true;
	return false;
}

bool slConfig::Read(const wxString& key, bool* b, bool defaultVal) const
{
	if ( slConfigBaseType::Read( key, b, defaultVal ) )
		return true;
	else if ( m_global_config && m_global_config->Read( key, b, defaultVal ) )
		return true;
	if ( IsRecordingDefaults() )
		((slConfigBaseType*)this)->Write( key, defaultVal );
	return false;
}
