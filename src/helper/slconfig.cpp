#include "slconfig.h"

#include <wx/filename.h>
#include <wx/wfstream.h>
#include <wx/log.h>

#include "../utils/platform.h"
#include "../utils/conversion.h"


bool slConfig::m_user_defined_config = false;
wxString slConfig::m_user_defined_config_path = wxEmptyString;
wxString slConfig::m_chosen_path = wxEmptyString;
const wxChar sep = wxFileName::GetPathSeparator();
const wxString sepstring = wxString(sep);


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

//! overwrite default create of wxConfigBase
slConfig* slConfig::Create()
{
	bool portable = false;

	wxString userfilepath = IdentityString( GetConfigfileDir() + sepstring + _T( "%s.conf" ), true );
	wxString localfilepath =  IdentityString( GetExecutableFolder() + sepstring + _T( "%s.conf" ), true );

	if ( m_user_defined_config && wxFileName::IsFileWritable( m_user_defined_config_path ) ) {
		slConfig::m_chosen_path = m_user_defined_config_path;
		portable = false;
	} else {
		if ( !wxFileName::FileExists( localfilepath ) || !wxFileName::IsFileWritable( localfilepath ) ) {
			//either local conf file does not exist, or it exists but is not writable
			slConfig::m_chosen_path = userfilepath;
			portable = false;
		} else {
			slConfig::m_chosen_path = localfilepath; // portable mode, use only current app paths
			portable = true;
		}
	}

	// if it doesn't exist, try to create it
	if ( !wxFileName::FileExists( slConfig::m_chosen_path ) ) {
		// if directory doesn't exist, try to create it
		if ( !portable && !wxFileName::DirExists( GetUserDataDir() ) )
			wxFileName::Mkdir( GetUserDataDir(), 0755 );

		wxFileOutputStream outstream( slConfig::m_chosen_path );

		if ( !outstream.IsOk() ) {
			if ( m_user_defined_config ) {
				wxLogError( _T( "unable to use specified config file" ) );
				exit( -1 );
			}
		}
	}

	wxFileInputStream instream( slConfig::m_chosen_path );

	if ( !instream.IsOk() ) {
		if ( m_user_defined_config ) {
			wxLogError( _T( "unable to use specified config file" ) );
			exit( -1 );
		}
	}

	// abuse the defaults system to set portable mode
	setDefault(_T("/portable"), portable);

	slConfig* config = new slConfig( instream );
	config->SetRecordDefaults( true );
	return config;
}

//! create slConfig on first access
slConfig* slConfig::Get() {
	static slConfig* cfg = NULL;
	if (cfg  == NULL) {
		cfg = Create();
	}
	return cfg;
}

void slConfig::SaveFile()
{
	Flush();
	wxFileOutputStream outstream( slConfig::m_chosen_path );

	if ( !outstream.IsOk() ) {
		// TODO: error handling
	}
	Save( outstream );
}

wxString slConfig::GetFilePath() const {
	return slConfig::m_chosen_path;
}

#ifdef __WXMSW__
bool slConfig::DoWriteLong( const wxString& key, long lValue )
{
	return slConfigBaseType::DoWriteString( key, TowxString<long>( lValue ) );
}
#endif

Default<wxString>& slConfig::getDefaultsString() {
	static Default<wxString> defaultString;
	return defaultString;
}

Default<long>& slConfig::getDefaultsLong() {
	static Default<long> defaultLong;
	return defaultLong;
}

Default<double>& slConfig::getDefaultsDouble() {
	static Default<double> defaultDouble;
	return defaultDouble;
}

Default<bool>& slConfig::getDefaultsBool() {
	static Default<bool> defaultBool;
	return defaultBool;
}


template <class T>
Default<T>::Default() {
	defaultMap = DefaultMap();
}

//! get default value, return true if default value found
template <class T>
bool Default<T>::Get(const wxString& key, T& defValue) const {
	// map[key] is not const, use find
	auto it = defaultMap.find(key);
	if (it == defaultMap.end()) {
		wxLogWarning(wxString::Format(_T("no default set for: %s"), key.c_str()));
		return false;
	}
	defValue = it->second;
	return true;
};

//! return true if default value set, won't overwrite if already exists
template <class T>
bool Default<T>::Set(const wxString& key, const T& defValue) {
	auto ret = defaultMap.insert(std::pair<const wxString,const T>(key,defValue));
	if (ret.second) {
		return true;
	}
	wxLogWarning(wxString::Format(_T("default already exists: %s"), key.c_str()));
	return false;
};


bool slConfig::setDefault(const wxString& key, const wxString& defVal) {
	return getDefaultsString().Set(key, defVal);
}
bool slConfig::setDefault(const wxString& key, const long& defVal) {
	return getDefaultsLong().Set(key, defVal);
}

bool slConfig::setDefault(const wxString& key, const double& defVal) {
	return getDefaultsDouble().Set(key, defVal);
}

bool slConfig::setDefault(const wxString& key, const bool& defVal) {
	return getDefaultsBool().Set(key, defVal);
}

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
	return getDefaultsString().Get(key, *str);
}


bool slConfig::Read(const wxString& key, long* l) const
{
	if ( slConfigBaseType::Read( key, l ) )
		return true;
	getDefaultsLong().Get(key, *l);
	if ( l && IsRecordingDefaults() )
		((slConfig*)this)->Write( key, *l );
	return false;
}


bool slConfig::Read(const wxString& key, double* d) const
{
	if ( slConfigBaseType::Read( key, d ) )
		return true;
	return getDefaultsDouble().Get(key, *d);
}

bool slConfig::Read(const wxString& key, bool* b) const
{
	if ( slConfigBaseType::Read( key, b ) )
		return true;
	return getDefaultsBool().Get(key, *b);
}


slConfigDefault::slConfigDefault(const wxString& key, const wxString& defVal) {
	slConfig::setDefault(key, defVal);
}

slConfigDefault::slConfigDefault(const wxString& key, const long& defVal) {
	slConfig::setDefault(key, defVal);
}

slConfigDefault::slConfigDefault(const wxString& key, const double& defVal) {
	slConfig::setDefault(key, defVal);
}

slConfigDefault::slConfigDefault(const wxString& key, const bool& defVal) {
	slConfig::setDefault(key, defVal);
}

