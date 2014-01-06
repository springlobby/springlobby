#include "slpaths.h"

#include "platform.h"
#include <wx/string.h>
#include <wx/filename.h>

bool SlPaths::m_user_defined_config = false;
wxString SlPaths::m_user_defined_config_path = wxEmptyString;


const wxChar sep = wxFileName::GetPathSeparator();
const wxString sepstring = wxString(sep);


wxString SlPaths::GetLocalConfigPath () {
	return IdentityString( GetExecutableFolder() + sepstring + _T( "%s.conf" ), true );
}

wxString SlPaths::GetDefaultConfigPath () {
	return IdentityString( GetConfigfileDir() + sepstring + _T( "%s.conf" ), true );
}

bool SlPaths::IsPortableMode () {
	if (m_user_defined_config) {
		return false;
	}
	return wxFileName::FileExists( GetLocalConfigPath() );
}

wxString SlPaths::GetConfigPath () {
	if ( m_user_defined_config) {
		return m_user_defined_config_path;
	}
	return IsPortableMode() ?
			GetLocalConfigPath() : GetDefaultConfigPath();
}
