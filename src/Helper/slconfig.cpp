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
	SetRecordDefaults( true );
	#ifdef __WXMSW__
		wxString p (wxFileName::GetPathSeparator());
		wxString global_config_path = wxFormat( _T("%s%s%s.global.conf") )
													   % GetExecutableFolder()
													   % p
													   % ::GetAppName( true );
	#else
		wxString global_config_path = IdentityString( _T("/etc/default/%s.conf"), true );
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
			((slConfig*)this)->Write( key, ret );
		return ret;
	}
	if ( IsRecordingDefaults() )
		((slConfig*)this)->Write( key, defaultVal );
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
			((slConfig*)this)->Write( key, ret );
		return ret;
	}
	if ( IsRecordingDefaults() )
		((slConfig*)this)->Write( key, defaultVal );
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
			((slConfig*)this)->Write( key, *str );
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
		((slConfig*)this)->Write( key, defaultVal );
	return false;
}

bool slConfig::Read(const wxString& key, long* l) const
{
	if ( slConfigBaseType::Read( key, l ) )
		return true;
	else if ( m_global_config && m_global_config->Read( key, l ) )
	{
		if ( l && IsRecordingDefaults() )
			((slConfig*)this)->Write( key, *l );
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
		((slConfig*)this)->Write( key, defaultVal );
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
		((slConfig*)this)->Write( key, defaultVal );
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
		((slConfig*)this)->Write( key, defaultVal );
	return false;
}

bool slConfig::HasSection( const wxString& strName) const
{
	return Exists( strName ) || ( m_global_config && m_global_config->Exists( strName ) );
}

void slConfig::SetPath(const wxString& strPath)
{
	slConfigBaseType::SetPath(strPath);
	if ( m_global_config )
		m_global_config->SetPath(strPath);
}

bool slConfig::GetFirstEntry(wxString& str, long& index) const
{
	//the common case, no need to query the global conf
	if ( slConfigBaseType::GetFirstEntry( str, index) )
		return true;
	long index_global;
	if ( !( m_global_config && m_global_config->GetFirstEntry( str, index_global) ) )
		return false;
	//the local has no entry, but the global does
	//setup an entry in the forwards map, so we can redirect subsequent call to GetNextEntry to the global
	m_enumerationId_forwards_entries[index] = index_global;
	return true;
}

bool slConfig::GetNextEntry(wxString& str, long& index) const
{
	ForwardsType::iterator forward_it =
			m_enumerationId_forwards_entries.find( index );
	//an entry in the forwards map must take precendence over local
	if ( forward_it != m_enumerationId_forwards_entries.end() )
	{
		bool ret = m_global_config && m_global_config->GetNextEntry( str, forward_it->second );
		if ( !ret )
			//returning false here will (in proper usage) stop the outside loop from calling GetNextEntry again
			//so this is the time to remove the forward
			m_enumerationId_forwards_entries.erase( forward_it );
		return ret;
	}
	return slConfigBaseType::GetNextEntry( str, index );
}

bool slConfig::GetFirstGroup(wxString& str, long& index) const
{
	//the common case, no need to query the global conf
	if ( slConfigBaseType::GetFirstGroup( str, index) )
		return true;
	long index_global;
	if ( !( m_global_config && m_global_config->GetFirstGroup( str, index_global) ) )
		return false;
	//the local has no entry, but the global does
	//setup an entry in the forwards map, so we can redirect subsequent call to GetNextEntry to the global
	m_enumerationId_forwards_groups[index] = index_global;
	return true;
}

bool slConfig::GetNextGroup(wxString& str, long& index) const
{
	ForwardsType::iterator forward_it =
			m_enumerationId_forwards_groups.find( index );
	//an entry in the forwards map must take precendence over local
	if ( forward_it != m_enumerationId_forwards_groups.end() )
	{
		bool ret = m_global_config && m_global_config->GetNextGroup( str, forward_it->second );
		if ( !ret )
			//returning false here will (in proper usage) stop the outside loop from calling GetNextEntry again
			//so this is the time to remove the forward
			m_enumerationId_forwards_groups.erase( forward_it );
		return ret;
	}
	return slConfigBaseType::GetNextGroup( str, index );
}

//! only return gobal number if local is zero
//size_t slConfig::GetNumberOfGroups(bool bRecursive ) const
//{
//	return slConfigBaseType::GetNumberOfGroups( bRecursive  );
////	size_t this_count =

//	if ( this_count != 0 )
//		return this_count;
//	if ( m_global_config )
//		return m_global_config->GetNumberOfGroups( bRecursive  );
//	return 0;
//}

////! only return gobal number if local is zero
//size_t slConfig::GetNumberOfEntries(bool bRecursive ) const
//{
//	size_t this_count = slConfigBaseType::GetNumberOfEntries( bRecursive  );
//	if ( this_count != 0 )
//		return this_count;
//	if ( m_global_config )
//		return m_global_config->GetNumberOfEntries( bRecursive  );
//	return 0;
//}
