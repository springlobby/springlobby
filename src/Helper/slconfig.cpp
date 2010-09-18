#include "slconfig.h"

#include <wx/filename.h>
#include <wx/wfstream.h>

#include "../utils/platform.h"

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
		wxString global_config_path = wxString::Format( _T("%s.global.conf"),
													   GetExecutableFolder().c_str(),
													   wxFileName::GetPathSeparator(),
													   GetAppName( true ).c_str()
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
	wxString ret;
	if ( slConfigBaseType::Read( key, &ret ) )
		return ret;
	else if ( m_global_config && m_global_config->Read( key, &ret ) )
		return ret;
	else
		return defaultVal;
}

long slConfig::Read(const wxString& key, long defaultVal) const
{
	long ret;
	if ( slConfigBaseType::Read( key, &ret ) )
		return ret;
	else if ( m_global_config && m_global_config->Read( key, &ret ) )
		return ret;
	else
		return defaultVal;
}

bool slConfig::Read(const wxString& key, wxString* str) const
{
	if ( slConfigBaseType::Read( key, str ) )
		return true;
	else if ( m_global_config && m_global_config->Read( key, str ) )
		return true;
	return false;
}

bool slConfig::Read(const wxString& key, wxString* str, const wxString& defaultVal) const
{
	if ( slConfigBaseType::Read( key, str, defaultVal ) )
		return true;
	else if ( m_global_config && m_global_config->Read( key, str, defaultVal ) )
		return true;
	return false;
}

bool slConfig::Read(const wxString& key, long* l) const
{
	if ( slConfigBaseType::Read( key, l ) )
		return true;
	else if ( m_global_config && m_global_config->Read( key, l ) )
		return true;
	return false;
}

bool slConfig::Read(const wxString& key, long* l, long defaultVal) const
{
	if ( slConfigBaseType::Read( key, l, defaultVal ) )
		return true;
	else if ( m_global_config && m_global_config->Read( key, l, defaultVal ) )
		return true;
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
	return false;
}
