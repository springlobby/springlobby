#include "plasmaresourceinfo.h"
#include "conversion.h"

#include <wx/tokenzr.h>
#include <wx/log.h>

wxString PlasmaResourceInfo::ToString() const
{
	//we save the number of elements in both arraytypes so we can use the same seperator for the whole line
	wxString num_deps = TowxString( m_dependencies.Count() );
	wxString num_seed = TowxString( m_webseeds.Count() );
	wxString sep(_T("\t") );
	//this takes care of the non array members
	wxString ret( num_seed + sep + num_deps + sep + m_name + sep
				+ m_torrent_filename + sep + m_local_torrent_filepath
				+ sep + TowxString( m_size_KB ) + sep + TowxString(  m_type )  );
	for ( size_t i = 0; i < m_dependencies.Count(); ++i )
		ret += sep + m_dependencies[i];
	for ( size_t i = 0; i < m_webseeds.Count(); ++i )
		ret += sep + m_webseeds[i];

	return ret;
}

void PlasmaResourceInfo::FromString( const wxString& str )
{
	wxString sep(_T("\t") );
	wxStringTokenizer tok( str, sep, wxTOKEN_RET_EMPTY_ALL );
	StringtokenizerVectorized vec( tok );
	size_t num_tokens = vec.size();
	if ( num_tokens < 6 )
	{
		wxLogError( _T("Loading PLasmaResourceInfo from string failed") ) ;
		return;
	}
	size_t num_deps = FromwxString<size_t>( vec[0] );
//    size_t num_seed = FromwxString<size_t>( vec[1] ); //not actually needed
	m_name = vec[2];
	m_torrent_filename = vec[3];
	m_local_torrent_filepath = vec[4];
	m_size_KB = FromwxString<unsigned int>( vec[5] );
	int rt = FromwxString<int>( vec[6] );
	switch ( rt ) {
		default: m_type = unknown; break;
		case 1: m_type = map; break;
		case 0: m_type = mod; break;
	}
	m_dependencies.Clear();
	m_webseeds.Clear();
	size_t i = 7;
	for ( ; i < 7 + num_deps; ++i )
		m_dependencies.Add( vec[i] );
	for ( ; i < num_tokens; ++i )
		m_dependencies.Add( vec[i] );
}

PlasmaResourceInfo::PlasmaResourceInfo() :
	m_size_KB(0), m_type(unknown)
{
}
