#include "slconfig.h"

#include <wx/filename.h>
#include <wx/wfstream.h>
#include <wx/log.h>

#include "../utils/debug.h"
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

	// FIXME portable is determined at runtime and should be separate
	// from this
	GetDefaultsBool().Set(_T("/portable"), portable);

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
		wxLogError(_T("can not save config: %s"), slConfig::m_chosen_path.c_str());
	}
	Save( outstream );
	wxLogMessage(_T("config file saved: %s"), slConfig::m_chosen_path.c_str());
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

Default<wxString>& slConfig::GetDefaultsString() {
	static Default<wxString> defaultString;
	return defaultString;
}

Default<long>& slConfig::GetDefaultsLong() {
	static Default<long> defaultLong;
	return defaultLong;
}

Default<double>& slConfig::GetDefaultsDouble() {
	static Default<double> defaultDouble;
	return defaultDouble;
}

Default<bool>& slConfig::GetDefaultsBool() {
	static Default<bool> defaultBool;
	return defaultBool;
}


template <class T>
Default<T>::Default() {
	defaultMap = DefaultMap();
}

//! get default value, return true if default value found
template <class T>
void Default<T>::Get(const wxString& key, T& defValue) const {
	// TODO make find work or use other container
	for (auto it=defaultMap.begin(); it!=defaultMap.end(); ++it) {
		if (it->first.Cmp(key)) {
			defValue = it->second;
			return;
		}
	}
	ASSERT_LOGIC(
		false,
		wxString::Format(_T("no default set for: %s"), key.c_str())
	);
};

//! return true if default value set, won't overwrite if already exists
template <class T>
void Default<T>::Set(const wxString& key, const T& defValue) {
	auto ret = defaultMap.insert(std::pair<const wxString,const T>(key,defValue));
	ASSERT_LOGIC(
		ret.second,
		wxString::Format(_T("default already exists: %s"), key.c_str())
	);
}


wxString& slConfig::ReadString(const wxString& key) const
{
	// read a value with preset default
	// this will return a valid value or fail loudly
	// 1. read the default value to make sure it is set
	// 2. read current value, if set
	wxString* value = NULL;
	GetDefaultsString().Get(key, *value);

	// Read() without default (third parameter) will change value if
	// set, or leave it alone
	slConfigBaseType::Read( key, value );

	// we can assert that value will be valid or the program terminated
	if (IsRecordingDefaults()) {
		((slConfig*)this)->Write(key, *value);
	}
	return *value;
}

long slConfig::ReadLong(const wxString& key) const
{
	long value;
	GetDefaultsLong().Get(key, value);
	slConfigBaseType::Read( key, &value );
	if (IsRecordingDefaults()) {
		((slConfig*)this)->Write(key, value);
	}
	return value;
}

double slConfig::ReadDouble(const wxString& key) const
{
	double value;
	GetDefaultsDouble().Get(key, value);
	slConfigBaseType::Read( key, &value );
	if (IsRecordingDefaults()) {
		((slConfig*)this)->Write(key, value);
	}
	return value;
}

bool slConfig::ReadBool(const wxString& key) const
{
	bool value;
	GetDefaultsBool().Get(key, value);
	slConfigBaseType::Read( key, &value );
	if (IsRecordingDefaults()) {
		((slConfig*)this)->Write(key, value);
	}
	return value;
}

wxArrayString slConfig::GetGroupList( const wxString& base_key )
{
	slConfig::PathGuard pathGuard ( this, base_key );
	wxString groupname;
	long dummy;
	wxArrayString ret;
	bool groupexist = GetFirstGroup( groupname, dummy );
	while ( groupexist )
	{
		ret.Add( groupname );
		groupexist = GetNextGroup( groupname, dummy );
	}
	return ret;
}

wxArrayString slConfig::GetEntryList( const wxString& base_key )
{
	slConfig::PathGuard pathGuard ( this, base_key );
	wxString entryname;
	long dummy;
	wxArrayString ret;
	bool entryexist = GetFirstEntry( entryname, dummy );
	while ( entryexist )
	{
		ret.Add( entryname );
		entryexist = GetNextEntry( entryname, dummy );
	}
	return ret;
}

unsigned int slConfig::GetGroupCount( const wxString& base_key )
{
	slConfig::PathGuard pathGuard ( this, base_key );
	return GetNumberOfGroups( false );
}


slConfigDefault::slConfigDefault(const wxString& key, const wxString& defVal) {
	slConfig::GetDefaultsString().Set(key, defVal);
}

slConfigDefault::slConfigDefault(const wxString& key, const long& defVal) {
	slConfig::GetDefaultsLong().Set(key, defVal);
}

slConfigDefault::slConfigDefault(const wxString& key, const double& defVal) {
	slConfig::GetDefaultsDouble().Set(key, defVal);
}

slConfigDefault::slConfigDefault(const wxString& key, const bool& defVal) {
	slConfig::GetDefaultsBool().Set(key, defVal);
}

slConfig& cfg() {
	return *slConfig::Get();
}
