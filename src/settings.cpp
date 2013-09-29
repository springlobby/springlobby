/* Copyright (C) 2007, 2008 The SpringLobby Team. All rights reserved. */
//
// Class: Settings
//

#include "settings.h"

#ifdef __WXMSW__
	#include <wx/msw/registry.h>
#endif
#include "helper/slconfig.h"
#include "defines.h" //to get HAVEWX??

#include <wx/filename.h>
#include <wx/filefn.h>
#include <wx/intl.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <wx/colour.h>
#include <wx/cmndata.h>
#include <wx/font.h>
#include <wx/log.h>
#include <wx/wfstream.h>
#include <wx/settings.h>
#include <wx/tokenzr.h>
#ifdef HAVE_WX29
	#include <wx/colourdata.h>
#endif
#include <set>

#include "nonportable.h"
#include "utils/conversion.h"
#include "utils/debug.h"
#include <lslutils/misc.h>
#include "utils/platform.h"
#include "uiutils.h"
#include "battlelist/battlelistfiltervalues.h"
#include "playback/playbackfiltervalues.h"
#include <lslutils/globalsmanager.h>
#include <lslunitsync/c_api.h>
#include "customlistctrl.h"
#include "springsettings/presets.h"
#include "helper/sortutil.h"
#include "mainwindow.h"
#ifdef SL_DUMMY_COL
    #include "utils/customdialogs.h"
#endif
#include "utils/pathlistfactory.h"

#ifdef __WXMSW__
	#define BIN_EXT _T(".exe")
#else
	#define BIN_EXT _T("")
#endif

bool Settings::m_user_defined_config = false;
wxString Settings::m_user_defined_config_path = wxEmptyString;
const wxChar sep = wxFileName::GetPathSeparator();
const wxString sepstring = wxString(sep);


const wxColour defaultHLcolor ( 255, 0, 0 );

Settings& sett()
{
    static LSL::Util::LineInfo<Settings> m( AT );
    static LSL::Util::GlobalObjectHolder<Settings, LSL::Util::LineInfo<Settings> > m_sett( m );
	return m_sett;
}

Settings::Settings()
	:m_forced_springconfig_path(wxEmptyString)
{
	wxString userfilepath = IdentityString( GetConfigfileDir() + sepstring + _T( "%s.conf" ), true );
	wxString localfilepath =  IdentityString( GetExecutableFolder() + sepstring + _T( "%s.conf" ), true );

	if ( m_user_defined_config && wxFileName::IsFileWritable( m_user_defined_config_path ) )
	{
		m_chosen_path = m_user_defined_config_path;
		SetPortableMode( false );
	}
	else
	{
		if ( !wxFileName::FileExists( localfilepath ) || !wxFileName::IsFileWritable( localfilepath ) )
		{
            //either local conf file does not exist, or it exists but is not writable
			m_chosen_path = userfilepath;
			SetPortableMode( false );
		}
		else
		{
			m_chosen_path = localfilepath; // portable mode, use only current app paths
			SetPortableMode ( true );
		}
	}

	// if it doesn't exist, try to create it
	if ( !wxFileName::FileExists( m_chosen_path ) )
	{
		// if directory doesn't exist, try to create it
		if ( !IsPortableMode() && !wxFileName::DirExists( GetUserDataDir() ) )
			wxFileName::Mkdir( GetUserDataDir(), 0755 );

		wxFileOutputStream outstream( m_chosen_path );

		if ( !outstream.IsOk() )
		{
			if ( m_user_defined_config ) {
				wxLogError( _T( "unable to use specified config file" ) );
				exit( -1 );
			}
		}
	}

	wxFileInputStream instream( m_chosen_path );

	if ( !instream.IsOk() )
	{
		if ( m_user_defined_config ) {
			wxLogError( _T( "unable to use specified config file" ) );
			exit( -1 );
		}
	}
	m_final_config_path = m_chosen_path;
	m_config = new slConfig( instream );
	m_config->SetRecordDefaults( true );
}

Settings::~Settings()
{
}

//! @brief Saves the settings to file
void Settings::SaveSettings()
{
	m_config->Write( _T( "/General/firstrun" ), false );
	SetCacheVersion();
	SetSettingsVersion();
	m_config->Flush();
	wxFileOutputStream outstream( m_chosen_path );

	if ( !outstream.IsOk() )
	{
		// TODO: error handling
	}

	m_config->Save( outstream );
}

wxArrayString Settings::GetGroupList( const wxString& base_key )
{
	slConfig::PathGuard pathGuard ( m_config, base_key );
	wxString groupname;
	long dummy;
	wxArrayString ret;
	bool groupexist = m_config->GetFirstGroup( groupname, dummy );
	while ( groupexist )
	{
		ret.Add( groupname );
		groupexist = m_config->GetNextGroup( groupname, dummy );
	}
	return ret;
}

wxArrayString Settings::GetEntryList( const wxString& base_key )
{
	slConfig::PathGuard pathGuard ( m_config, base_key );
	wxString entryname;
	long dummy;
	wxArrayString ret;
	bool entryexist = m_config->GetFirstEntry( entryname, dummy );
	while ( entryexist )
	{
		ret.Add( entryname );
		entryexist = m_config->GetNextEntry( entryname, dummy );
	}
	return ret;
}

unsigned int Settings::GetGroupCount( const wxString& base_key )
{
	slConfig::PathGuard pathGuard ( m_config, base_key );
	return m_config->GetNumberOfGroups( false );
}

bool Settings::IsPortableMode() const
{
	return m_portable_mode;
}

void Settings::SetPortableMode( bool mode )
{
	m_portable_mode = mode;
}


bool Settings::IsFirstRun()
{
	return m_config->Read( _T( "/General/firstrun" ), true );
}


void Settings::SetSettingsVersion()
{
	m_config->Write( _T( "/General/SettingsVersion" ), SETTINGS_VERSION );
}


int Settings::GetSettingsVersion()
{
	return m_config->Read( _T( "/General/SettingsVersion" ), SETTINGS_VERSION );
}


wxString Settings::GetLobbyWriteDir()
{
	wxString path = GetCurrentUsedDataDir() + sepstring + _T( "lobby" );
	if ( !wxFileName::DirExists( path ) )
	{
		if ( !wxFileName::Mkdir(  path, 0755  ) ) return wxEmptyString;
	}
	path += sepstring + _T( "SpringLobby" ) + sepstring;
	if ( !wxFileName::DirExists( path ) )
	{
		if ( !wxFileName::Mkdir(  path, 0755  ) ) return wxEmptyString;
	}
	return path;
}


bool Settings::UseOldSpringLaunchMethod()
{
	return m_config->Read( _T( "/Spring/UseOldLaunchMethod" ), 0l );
}

bool Settings::GetNoUDP()
{
	return m_config->Read( _T( "/General/NoUDP" ), 0l );
}

void Settings::SetNoUDP( bool value )
{
	m_config->Write( _T( "/General/NoUDP" ), value );
}

int Settings::GetClientPort()
{
	return m_config->Read( _T( "/General/ClientPort" ), 0l );
}

void Settings::SetClientPort( int value )
{
	m_config->Write( _T( "/General/ClientPort" ), value );
}

bool Settings::GetShowIPAddresses()
{
	return m_config->Read( _T( "/General/ShowIP" ), 0l );
}

void Settings::SetShowIPAddresses( bool value )
{
	m_config->Write( _T( "/General/ShowIP" ), value );
}

void Settings::SetOldSpringLaunchMethod( bool value )
{
	m_config->Write( _T( "/Spring/UseOldLaunchMethod" ), value );
}


int Settings::GetHTTPMaxParallelDownloads()
{
	return m_config->Read(_T("/General/ParallelHTTPCount"),3);
}
void Settings::SetHTTPMaxParallelDownloads(int value)
{
	m_config->Write(_T("/General/ParallelHTTPCount"),value);
}



bool Settings::GetWebBrowserUseDefault()
{
	// See note on ambiguities, in wx/confbase.h near line 180.
	bool useDefault;
	m_config->Read( _T( "/General/WebBrowserUseDefault" ), &useDefault, DEFSETT_WEB_BROWSER_USE_DEFAULT );
	return useDefault;
}

void Settings::SetWebBrowserUseDefault( bool useDefault )
{
	m_config->Write( _T( "/General/WebBrowserUseDefault" ), useDefault );
}

wxString Settings::GetWebBrowserPath()
{
	return m_config->Read( _T( "/General/WebBrowserPath" ), wxEmptyString );
}


void Settings::SetWebBrowserPath( const wxString& path )
{
	m_config->Write( _T( "/General/WebBrowserPath" ), path );
}


wxString Settings::GetCachePath()
{
	wxString path = GetCurrentUsedDataDir() + sepstring + _T( "cache" ) + sep;
	if ( !wxFileName::DirExists( path ) )
	{
		if ( !wxFileName::Mkdir(  path, 0755  ) ) return wxEmptyString;
	}
	path += _T( "SpringLobby" ) + sepstring;
	if ( !wxFileName::DirExists( path ) )
	{
		if ( !wxFileName::Mkdir(  path, 0755  ) ) return wxEmptyString;
	}
	return path;
}


//! @brief sets version number for the cache, needed to nuke it in case it becomes obsolete & incompatible with new versions
void Settings::SetCacheVersion()
{
	m_config->Write( _T( "/General/CacheVersion" ), CACHE_VERSION );
}


//! @brief returns the cache versioning number, do decide whenever to delete if becomes obsolete & incompatible with new versions
int Settings::GetCacheVersion()
{
	return m_config->Read( _T( "/General/CacheVersion" ), 0l );
}


void Settings::SetMapCachingThreadProgress( unsigned int index )
{
	m_config->Write( _T( "/General/LastMapCachingThreadIndex" ), ( int )index );
}


unsigned int Settings::GetMapCachingThreadProgress()
{
	return m_config->Read( _T( "/General/LastMapCachingThreadIndex" ), 0l );
}


void Settings::SetModCachingThreadProgress( unsigned int index )
{
	m_config->Write( _T( "/General/LastModCachingThreadIndex" ), ( int )index );
}


unsigned int Settings::GetModCachingThreadProgress()
{
	return m_config->Read( _T( "/General/LastModCachingThreadIndex" ), 0l );
}

bool Settings::ShouldAddDefaultServerSettings()
{
	return !m_config->HasSection( _T( "/Server" ) );
}

//! @brief Restores default settings
void Settings::SetDefaultServerSettings()
{
	SetServer(  DEFSETT_DEFAULT_SERVER_NAME,  DEFSETT_DEFAULT_SERVER_HOST, DEFSETT_DEFAULT_SERVER_PORT );
	SetServer( _T( "Backup server 1" ), _T( "lobby1.springlobby.info" ), 8200 );
	SetServer( _T( "Backup server 2" ), _T( "lobby2.springlobby.info" ), 8200 );
	SetServer( _T( "Test server" ), _T( "lobby.springrts.com" ), 7000 );
	SetDefaultServer( DEFSETT_DEFAULT_SERVER_NAME );
}


//! @brief convert old server settings format
void Settings::ConvertOldServerSettings()
{
	wxArrayString servers;
	std::map<wxString, bool> m_autosave_pass;
	std::map<wxString, wxString> m_saved_nicks;
	std::map<wxString, wxString> m_saved_pass;
	std::map<wxString, wxString> m_saved_hosts;
	std::map<wxString, int> m_saved_ports;
	int count = m_config->Read( _T( "/Servers/Count" ), 0l );
	for ( int i = 0; i < count; i++ )
	{
		wxString server_name = m_config->Read( wxFormat( _T( "/Servers/Server%d" ) ) % i, _T( "" ) );
		if ( server_name == _T( "TAS Server" ) ) server_name = DEFSETT_DEFAULT_SERVER_NAME;
		servers.Add( server_name );
		m_saved_nicks[server_name] = m_config->Read( _T( "/Server/" ) + server_name + _T( "/nick" ), _T( "" ) );
		m_saved_pass[server_name] = m_config->Read( _T( "/Server/" ) + server_name + _T( "/pass" ), _T( "" ) );
		m_autosave_pass[server_name] = m_config->Read( _T( "/Server/" ) + server_name + _T( "/savepass" ), 0l );
		m_saved_ports[server_name] = m_config->Read( _T( "/Server/" ) + server_name + _T( "/port" ), DEFSETT_DEFAULT_SERVER_PORT );
		m_saved_hosts[server_name] = m_config->Read( _T( "/Server/" ) + server_name + _T( "/host" ), DEFSETT_DEFAULT_SERVER_HOST );
	}
	m_config->DeleteGroup( _T( "/Server" ) );
	m_config->DeleteGroup( _T( "/Servers" ) );
	SetDefaultServerSettings();
	count = servers.GetCount();
	for ( int i = 0; i < count; i++ )
	{
		wxString server_name = servers[i];
		SetServer( server_name, m_saved_hosts[server_name], m_saved_ports[server_name] );
		SetServerAccountNick( server_name, m_saved_nicks[server_name] );
		SetServerAccountPass( server_name, m_saved_pass[server_name] );
		SetServerAccountSavePass( server_name, m_autosave_pass[server_name] );
	}

}

//! @brief Checks if the server name/alias exists in the settings
bool Settings::ServerExists( const wxString& server_name )
{
	return m_config->Exists( _T( "/Server/Servers/" ) + server_name );
}


//! @brief Get the name/alias of the default server.
//!
//! @note Normally this will be the previously selected server. But at first run it will be a server that is set as the default.
wxString Settings::GetDefaultServer()
{
    wxString serv = DEFSETT_DEFAULT_SERVER_NAME;
    return m_config->Read( _T("/Server/Default"), serv );
}

void Settings::SetAutoConnect( bool do_autoconnect )
{
	m_config->Write( _T( "/Server/Autoconnect" ),  do_autoconnect );
}

bool Settings::GetAutoConnect( )
{
	return m_config->Read( _T( "/Server/Autoconnect" ), 0l );
}


//! @brief Set the name/alias of the default server.
//!
//! @param server_name the server name/alias
//! @see GetDefaultServer()
void   Settings::SetDefaultServer( const wxString& server_name )
{
	m_config->Write( _T( "/Server/Default" ),  server_name );
}


//! @brief Get hostname of a server.
//!
//! @param server_name the server name/alias
wxString Settings::GetServerHost( const wxString& server_name )
{
    wxString host = DEFSETT_DEFAULT_SERVER_HOST;
    return m_config->Read( _T("/Server/Servers/")+ server_name +_T("/Host"), host );
}


//! @brief Set hostname of a server.
//!
//! @param server_name the server name/alias
//! @param the host url address
//! @param the port where the service is run
void   Settings::SetServer( const wxString& server_name, const wxString& url, int port )
{
	m_config->Write( _T( "/Server/Servers/" ) + server_name + _T( "/Host" ), url );
	m_config->Write( _T( "/Server/Servers/" ) + server_name + _T( "/Port" ), port );
}

//! @brief Deletes a server from the list.
//!
//! @param server_name the server name/alias
void Settings::DeleteServer( const wxString& server_name )
{
	m_config->DeleteGroup( _T( "/Server/Servers/" ) + server_name );
}


//! @brief Get port number of a server.
//!
//! @param server_name the server name/alias
int    Settings::GetServerPort( const wxString& server_name )
{
	return m_config->Read( _T( "/Server/Servers/" ) + server_name + _T( "/Port" ), DEFSETT_DEFAULT_SERVER_PORT );
}

//! @brief Get list of server aliases
wxArrayString Settings::GetServers()
{
	return GetGroupList( _T( "/Server/Servers/" ) );
}


//! @brief Get nickname of the default account for a server.
//!
//! @param server_name the server name/alias
wxString Settings::GetServerAccountNick( const wxString& server_name )
{
	return m_config->Read( _T( "/Server/Servers/" ) + server_name + _T( "/Nick" ), _T( "" ) ) ;
}


//! @brief Set nickname of the default account for a server.
//!
//! @param server_name the server name/alias
//! @param value the vaule to be set
void Settings::SetServerAccountNick( const wxString& server_name, const wxString& value )
{
	m_config->Write( _T( "/Server/Servers/" ) + server_name + _T( "/Nick" ), value );
}


//! @brief Get password of the default account for a server.
//!
//! @param server_name the server name/alias
//! @todo Implement
wxString Settings::GetServerAccountPass( const wxString& server_name )
{
	return m_config->Read( _T( "/Server/Servers/" ) + server_name + _T( "/Pass" ), _T( "" ) );
}


//! @brief Set password of the default account for a server.
//!
//! @param server_name the server name/alias
//! @param value the vaule to be set
//! @todo Implement
void   Settings::SetServerAccountPass( const wxString& server_name, const wxString& value )
{
	m_config->Write( _T( "/Server/Servers/" ) + server_name + _T( "/Pass" ), value );
}


//! @brief Get if the password should be saved for a default server account.
//!
//! @param server_name the server name/alias
//! @todo Implement
bool   Settings::GetServerAccountSavePass( const wxString& server_name )
{
	return m_config->Read( _T( "/Server/Servers/" ) + server_name + _T( "/savepass" ), ( long int )false );
}


//! @brief Set if the password should be saved for a default server account.
//!
//! @param server_name the server name/alias
//! @param value the vaule to be set
//! @todo Implement
void Settings::SetServerAccountSavePass( const wxString& server_name, const bool value )
{
	m_config->Write( _T( "/Server/Servers/" ) + server_name + _T( "/savepass" ), ( long int )value );
}


int Settings::GetNumChannelsJoin()
{
	return GetGroupCount( _T( "/Channels/AutoJoin" ) );
}

void Settings::AddChannelJoin( const wxString& channel , const wxString& key )
{
	int index = GetNumChannelsJoin();

	m_config->Write( wxFormat( _T( "/Channels/AutoJoin/Channel%d/Name" ) ) % index, channel );
	m_config->Write( wxFormat( _T( "/Channels/AutoJoin/Channel%d/Password" ) ) % index, key );
}


void Settings::RemoveChannelJoin( const wxString& channel )
{
	int index = GetChannelJoinIndex( channel );
	if ( index == -1 ) return;
	int total = GetNumChannelsJoin();
	m_config->DeleteGroup( _T( "/Channels/AutoJoin/Channel" ) + TowxString( index ) );
	m_config->RenameGroup( _T( "/Channels/AutoJoin/Channel" ) + TowxString( total - 1 ), _T( "/Channels/AutoJoin/Channel" ) + TowxString( index ) );
}

void Settings::RemoveAllChannelsJoin()
{
	m_config->DeleteGroup( _T( "/Channels/AutoJoin" ) );
}


int Settings::GetChannelJoinIndex( const wxString& name )
{
	int numchannels = GetNumChannelsJoin();
	int ret = -1;
	for ( int i = 0; i < numchannels; i++ )
	{
		if ( m_config->Read( wxFormat( _T( "/Channels/AutoJoin/Channel%d/Name" ) ) % i, _T( "" ) ) == name ) ret = i;
	}
	return ret;
}

std::vector<ChannelJoinInfo> Settings::GetChannelsJoin()
{
	std::vector<ChannelJoinInfo> ret;
//	int num = GetNumChannelsJoin();
	wxArrayString channels = GetGroupList( _T("/Channels/AutoJoin/") );
	slConfig::PathGuard pathguard( m_config, _T("/Channels/AutoJoin/") );
	for ( size_t i = 0; i < channels.Count(); ++i )
	{
		if( !channels[i].StartsWith( _T("Channel") ) )
			continue;
		ChannelJoinInfo info;
		info.name = m_config->Read( channels[i] + _T("/Name" ) );
		info.password = m_config->Read( channels[i] + _T("/Password" ) );
		ret.push_back( info );
	}
	return ret;
}

void Settings::ConvertOldChannelSettings()
{
	int numchannels = m_config->Read( _T( "/Channels/Count" ), 0l );
	m_config->DeleteEntry( _T( "/Channels/Count" ) );
	for ( int i = 0; i < numchannels; i++ )
	{
		wxString channelinfo = m_config->Read( _T( "/Channels/Channel" ) + TowxString( i ), _T( "" ) );
		m_config->DeleteEntry( _T( "/Channels/Channel" ) + TowxString( i ) );
		if ( channelinfo.Find( _T( " " ) ) != wxNOT_FOUND ) AddChannelJoin( channelinfo.BeforeFirst( _T( ' ' ) ), channelinfo.AfterLast( _T( ' ' ) ) );
		else AddChannelJoin( channelinfo, _T( "" ) );
	}
}

bool Settings::ShouldAddDefaultChannelSettings()
{
	return !m_config->HasSection( _T( "/Channels" ) );
}

/************* SPRINGLOBBY WINDOW POS/SIZE   ******************/
//! @brief Get width of MainWindow.
int Settings::GetWindowWidth( const wxString& window )
{
	return m_config->Read( _T( "/GUI/" ) + window + _T( "/width" ), DEFSETT_MW_WIDTH );
}


//! @brief Set width position of MainWindow
void Settings::SetWindowWidth( const wxString& window, const int value )
{
	m_config->Write(
	    _T( "/GUI/" ) + window + _T( "/width" ),
	    LSL::Util::Clamp(  value,
	            wxSystemSettings::GetMetric( wxSYS_WINDOWMIN_X ),
	            wxSystemSettings::GetMetric( wxSYS_SCREEN_X )
	         )
	);
}


//! @brief Get height of MainWindow.
int Settings::GetWindowHeight( const wxString& window )
{
	return m_config->Read( _T( "/GUI/" ) + window + _T( "/height" ), DEFSETT_MW_HEIGHT );
}


//! @brief Set height position of MainWindow
void Settings::SetWindowHeight( const wxString& window, const int value )
{
	m_config->Write(
	    _T( "/GUI/" ) + window + _T( "/height" ),
	    LSL::Util::Clamp(  value,
	            wxSystemSettings::GetMetric( wxSYS_WINDOWMIN_Y ),
	            wxSystemSettings::GetMetric( wxSYS_SCREEN_Y )
	         )
	);
}


//! @brief Get top position of MainWindow.
int Settings::GetWindowTop( const wxString& window )
{
	return m_config->Read( _T( "/GUI/" ) + window + _T( "/top" ), DEFSETT_MW_TOP );
}


//! @brief Set top position of MainWindow
void Settings::SetWindowTop( const wxString& window, const int value )
{
	m_config->Write(
	    _T( "/GUI/" ) + window + _T( "/top" ),
	    LSL::Util::Clamp( value,
	           0,
	           wxSystemSettings::GetMetric( wxSYS_SCREEN_Y ) - 20
	         )
	);
}


//! @brief Get left position of MainWindow.
int Settings::GetWindowLeft( const wxString& window )
{
	return m_config->Read( _T( "/GUI/" ) + window + _T( "/left" ), DEFSETT_MW_LEFT );
}

//! @brief Set left position of MainWindow
void Settings::SetWindowLeft( const wxString& window, const int value )
{
	m_config->Write(
	    _T( "/GUI/" ) + window + _T( "/left" ),
	    LSL::Util::Clamp( value,
	           0,
	           wxSystemSettings::GetMetric( wxSYS_SCREEN_X ) - 20
	         )
	);
}

//some code duplication necessary to be able to simply use wx defaults
wxSize  Settings::GetWindowSize( const wxString& window, const wxSize& def )
{
	wxSize ret = def;
	ret.SetHeight( m_config->Read( _T( "/GUI/" ) + window + _T( "/height" ), ret.GetHeight() ) );
	ret.SetWidth( m_config->Read( _T( "/GUI/" ) + window + _T( "/width" ), ret.GetWidth() ) );
	return ret;
}

void Settings::SetWindowSize( const wxString& window, const wxSize& size  )
{
	SetWindowWidth( window, size.GetWidth() );
	SetWindowHeight( window, size.GetHeight() );
}

//some code duplication necessary to be able to simply use wx defaults
wxPoint Settings::GetWindowPos( const wxString& window, const wxPoint& def )
{
	wxPoint ret = def;
	ret.x = m_config->Read( _T( "/GUI/" ) + window + _T( "/left" ), ret.x );
	ret.y = m_config->Read( _T( "/GUI/" ) + window + _T( "/top" ), ret.y );
	return ret;
}

void Settings::SetWindowPos( const wxString& window, const wxPoint& pos )
{
	SetWindowLeft( window, pos.x );
	SetWindowTop( window, pos.y );
}

// ========================================================

wxString Settings::AutoFindSpringBin()
{
	wxPathList pl;

	pl.AddEnvList( _T( "%ProgramFiles%" ) );
	pl.AddEnvList( _T( "PATH" ) );

    pl = PathlistFactory::AdditionalSearchPaths( pl );

	return pl.FindValidPath( SPRING_BIN );
}

wxString Settings::AutoFindUnitSync(wxPathList pl) const
{
	wxString retpath = pl.FindValidPath( _T( "unitsync" ) + GetLibExtension() );
	if ( retpath.IsEmpty() )
		retpath = pl.FindValidPath( _T( "libunitsync" ) + GetLibExtension() );
	return retpath;
}

wxString Settings::AutoFindBundle()
{
    wxPathList pl = PathlistFactory::ConfigFileSearchPaths();
	return pl.FindValidPath( _T( "Spring.app" ) );
}

wxString Settings::AutoFindUikeys()
{
    wxPathList pl = PathlistFactory::ConfigFileSearchPaths();
	return pl.FindValidPath( _T( "uikeys.txt" ) );
}


void Settings::ConvertOldSpringDirsOptions()
{
	SetUnitSync( _T( "default" ), m_config->Read( _T( "/Spring/unitsync_loc" ), _T( "" ) ) );
	SetSpringBinary( _T( "default" ), m_config->Read( _T( "/Spring/exec_loc" ), _T( "" ) ) );

    SetUsedSpringIndex(_T("default"));

	m_config->DeleteEntry( _T( "/Spring/unitsync_loc" ) );
	m_config->DeleteEntry( _T( "/Spring/use_spring_def_loc" ) );
	m_config->DeleteEntry( _T( "/Spring/use_unitsync_def_loc" ) );
	m_config->DeleteEntry( _T( "/Spring/dir" ) );
	m_config->DeleteEntry( _T( "/Spring/exec_loc" ) );
}

std::map<wxString, wxString> Settings::GetSpringVersionList() const
{
	return m_spring_versions;
}

void Settings::RefreshSpringVersionList()
{
	wxLogDebugFunc( _T( "" ) );
	wxArrayString list = GetGroupList( _T( "/Spring/Paths" ) );
	int count = list.GetCount();
	std::map<std::string, std::string> usync_paths;
	for ( int i = 0; i < count; i++ ) {
		const wxString groupname = list[i];
		usync_paths[STD_STRING(groupname)] = STD_STRING(GetUnitSync(groupname));
	}
	if ( sett().GetSearchSpringOnlyInSLPath() || sett().GetUseSpringPathFromBundle() ) {
		usync_paths.clear();
		usync_paths[STD_STRING(sett().GetCurrentUsedSpringIndex())] = STD_STRING(sett().GetCurrentUsedUnitSync());
	}
	m_spring_versions.clear();
	try {
		const auto versions = LSL::susynclib().GetSpringVersionList( usync_paths );
		for(const auto pair : versions) {
			m_spring_versions.insert(std::make_pair(TowxString(pair.first), TowxString(pair.second)));
		}
	} catch (const std::runtime_error& e) {
		wxLogError(wxString::Format(_T("Couln't get list of spring versions: %s"), e.what()));
	} catch ( ...) {
		wxLogError(_T("Unknown Execption caught in Settings::RefreshSpringVersionList"));
	}
}

wxString Settings::GetCurrentUsedSpringIndex()
{
    return m_config->Read( _T( "/Spring/CurrentIndex" ), _T( "default" ) );
}

void Settings::SetUsedSpringIndex( const wxString& index )
{
    m_config->Write( _T( "/Spring/CurrentIndex" ), TowxString(index) );
}


bool Settings::GetSearchSpringOnlyInSLPath()
{
	return m_config->Read( _T( "/Spring/SearchSpringOnlyInSLPath" ), true );
}

void Settings::SetSearchSpringOnlyInSLPath( bool value )
{
	m_config->Write( _T( "/Spring/SearchSpringOnlyInSLPath" ), value );
}

void Settings::DeleteSpringVersionbyIndex( const wxString& index )
{
	m_config->DeleteGroup( _T( "/Spring/Path/" ) + index );
	if ( GetCurrentUsedSpringIndex() == index ) SetUsedSpringIndex( _T( "" ) );
}


bool Settings::IsInsideSpringBundle()
{
	return wxFileName::FileExists(GetExecutableFolder() + sepstring + _T("spring") + BIN_EXT) && wxFileName::FileExists(GetExecutableFolder() + sepstring + _T("unitsync") + GetLibExtension());
}

bool Settings::GetBundleMode()
{
	#ifndef __WXMAC__
		return false;
	#endif
	return m_config->Read(_T("/Spring/EnableBundleMode"), true);
}


bool Settings::GetUseSpringPathFromBundle()
{
	#ifndef __WXMAC__
		return false;
	#endif
	if ( !GetBundleMode() ) return false;
	return m_config->Read(_T("/Spring/UseSpringPathFromBundle"), IsInsideSpringBundle());
}

void Settings::SetUseSpringPathFromBundle( bool value )
{
	m_config->Write(_T("/Spring/UseSpringPathFromBundle"), value );
}

wxString Settings::GetCurrentUsedDataDir()
{
	wxString dir;
	if ( LSL::susynclib().IsLoaded() )
	{
        if ( LSL::susynclib().VersionSupports( LSL::USYNC_GetDataDir ) )
            dir = TowxString(LSL::susynclib().GetSpringDataDir());
        else
            dir = TowxString(LSL::susynclib().GetSpringConfigString("SpringData", ""));
	}
#ifdef __WXMSW__
	if ( dir.IsEmpty() )
        dir = GetExecutableFolder(); // fallback
#else
    if ( IsPortableMode() )
        dir = GetExecutableFolder();
	if ( dir.IsEmpty() )
        dir = wxFileName::GetHomeDir() + sepstring + _T( ".spring" ); // fallback
#endif
	wxString stripped;
	if ( dir.EndsWith(sepstring,&stripped) ) return stripped;
	return dir;
}


wxString Settings::GetCurrentUsedSpringBinary()
{
	if ( IsPortableMode() ) return GetCurrentUsedDataDir() + sepstring + _T( "spring" ) + BIN_EXT;
    else if ( GetSearchSpringOnlyInSLPath() ) return GetExecutableFolder() + sepstring + _T( "spring" ) +  + BIN_EXT;
	else if ( GetUseSpringPathFromBundle() ) return GetExecutableFolder() + sepstring + _T("spring") + BIN_EXT;
	else return GetSpringBinary( GetCurrentUsedSpringIndex() );
}

wxString Settings::GetCurrentUsedUnitSync()
{
    if ( IsPortableMode() ) return AutoFindUnitSync( PathlistFactory::fromSinglePath( GetCurrentUsedDataDir() ) );
	else if ( GetSearchSpringOnlyInSLPath() ) return GetExecutableFolder() + sepstring + _T( "unitsync" ) + GetLibExtension();
	else if ( GetUseSpringPathFromBundle() ) return GetExecutableFolder() + sepstring + _T("unitsync") + GetLibExtension();
	else return GetUnitSync( GetCurrentUsedSpringIndex() );
}

wxString Settings::GetCurrentUsedBundle()
{
	return GetBundle( GetCurrentUsedSpringIndex() );
}

wxString Settings::GetCurrentUsedUikeys()
{
	if ( IsPortableMode() ) return GetCurrentUsedDataDir() + sepstring + _T( "uikeys.txt" );
	else if ( GetSearchSpringOnlyInSLPath() ) return GetExecutableFolder() + sepstring + _T( "uikeys.txt" );
	else return GetUikeys( GetCurrentUsedSpringIndex() );
}

wxString Settings::GetCurrentUsedSpringConfigFilePath()
{
	wxString path;
	try {
		path = TowxString(LSL::susynclib().GetConfigFilePath());
	} catch ( std::runtime_error& e) {
		wxLogError( wxString::Format( _T("Couldn't get SpringConfigFilePath, exception caught:\n %s"), e.what()  ) );
	}

	return path;
}

wxString Settings::GetUnitSync( const wxString& index )
{
	if ( GetBundleMode() ) return GetBundle( index )+ sepstring + _T("Contents") + sepstring + _T("MacOS") + sepstring + _T("libunitsync") +  GetLibExtension();
	return m_config->Read( _T( "/Spring/Paths/" ) + index + _T( "/UnitSyncPath" ), AutoFindUnitSync() );
}


wxString Settings::GetSpringBinary( const wxString& index )
{
	if ( GetBundleMode() )  return GetBundle( index )+ sepstring + _T("Contents") + sepstring + _T("MacOS") + sepstring + _T("spring");
	return m_config->Read( _T( "/Spring/Paths/" ) + index + _T( "/SpringBinPath" ), AutoFindSpringBin() );
}

wxString Settings::GetBundle( const wxString& index )
{
	return m_config->Read( _T( "/Spring/Paths/" ) + index + _T( "/SpringBundlePath" ), AutoFindBundle() );
}

void Settings::SetUnitSync( const wxString& index, const wxString& path )
{
	m_config->Write( _T( "/Spring/Paths/" ) + index + _T( "/UnitSyncPath" ), path );
}

void Settings::SetSpringBinary( const wxString& index, const wxString& path )
{
	m_config->Write( _T( "/Spring/Paths/" ) + index + _T( "/SpringBinPath" ), path );
}

void Settings::SetBundle( const wxString& index, const wxString& path )
{
	m_config->Write( _T( "/Spring/Paths/" ) + index + _T( "/SpringBundlePath" ), path );
}

void Settings::SetForcedSpringConfigFilePath( const wxString& path )
{
	m_forced_springconfig_path = path;
}

wxString Settings::GetForcedSpringConfigFilePath()
{
	if ( IsPortableMode() )
        return GetCurrentUsedDataDir() + sepstring + _T( "springsettings.cfg" );
	else
		return m_forced_springconfig_path;
}

// ===================================================

bool Settings::GetChatLogEnable()
{
	if ( !m_config->Exists( _T( "/ChatLog/chatlog_enable" ) ) ) SetChatLogEnable( true );
	return m_config->Read( _T( "/ChatLog/chatlog_enable" ), true );
}


void Settings::SetChatLogEnable( const bool value )
{
	m_config->Write( _T( "/ChatLog/chatlog_enable" ), value );
}


wxString Settings::GetChatLogLoc()
{
	wxString path = GetLobbyWriteDir() + _T( "chatlog" );
	if ( !wxFileName::DirExists( path ) )
	{
		if ( !wxFileName::Mkdir(  path, 0755  ) ) return wxEmptyString;
	}
	return path;
}


wxString Settings::GetLastHostDescription()
{
	return m_config->Read( _T( "/Hosting/LastDescription" ), _T( "" ) );
}


wxString Settings::GetLastHostMod()
{
	return m_config->Read( _T( "/Hosting/LastMod" ), _T( "" ) );
}


wxString Settings::GetLastHostPassword()
{
	return m_config->Read( _T( "/Hosting/LastPassword" ), _T( "" ) );
}


int Settings::GetLastHostPort()
{
	return m_config->Read( _T( "/Hosting/LastPort" ), DEFSETT_SPRING_PORT );
}


int Settings::GetLastHostPlayerNum()
{
	return m_config->Read( _T( "/Hosting/LastPlayerNum" ), 4 );
}


int Settings::GetLastHostNATSetting()
{
	return m_config->Read( _T( "/Hosting/LastNATSetting" ), ( long )0 );
}


wxString Settings::GetLastHostMap()
{
	return m_config->Read( _T( "/Hosting/LastMap" ), _T( "" ) );
}

int Settings::GetLastRankLimit()
{
	return m_config->Read( _T( "/Hosting/LastRank" ), 0l );
}

bool Settings::GetTestHostPort()
{
	return m_config->Read( _T( "/Hosting/TestHostPort" ), 0l );
}

bool Settings::GetLastAutolockStatus()
{
	return m_config->Read( _T( "/Hosting/LastAutoLock" ), true );
}

bool Settings::GetLastHostRelayedMode()
{
	return m_config->Read( _T( "/Hosting/LastRelayedMode" ), 0l );
}

wxColour Settings::GetBattleLastColour()
{
	return  wxColour( m_config->Read( _T( "/Hosting/MyLastColour" ), _T( "#FFFF00" ) ) );
}


void Settings::SetBattleLastColour( const wxColour& col )
{
	m_config->Write( _T( "/Hosting/MyLastColour" ), col.GetAsString( wxC2S_HTML_SYNTAX ) );
}

void Settings::SetLastHostDescription( const wxString& value )
{
	m_config->Write( _T( "/Hosting/LastDescription" ), value );
}


void Settings::SetLastHostMod( const wxString& value )
{
	m_config->Write( _T( "/Hosting/LastMod" ), value );
}


void Settings::SetLastHostPassword( const wxString& value )
{
	m_config->Write( _T( "/Hosting/LastPassword" ), value );
}


void Settings::SetLastHostPort( int value )
{
	m_config->Write( _T( "/Hosting/LastPort" ), value );
}


void Settings::SetLastHostPlayerNum( int value )
{
	m_config->Write( _T( "/Hosting/LastPlayerNum" ), value );
}


void Settings::SetLastHostNATSetting( int value )
{
	m_config->Write( _T( "/Hosting/LastNATSetting" ), value );
}


void Settings::SetLastHostMap( const wxString& value )
{
	m_config->Write( _T( "/Hosting/LastMap" ), value );
}

void Settings::SetLastRankLimit( int rank )
{
	m_config->Write( _T( "/Hosting/LastRank" ), rank );
}

void Settings::SetLastAI( const wxString& ai )
{
	m_config->Write( _T( "/SinglePlayer/LastAI" ), ai );
}

void Settings::SetTestHostPort( bool value )
{
	m_config->Write( _T( "/Hosting/TestHostPort" ), value );
}

void Settings::SetLastAutolockStatus( bool value )
{
	m_config->Write( _T( "/Hosting/LastAutoLock" ), value );
}

void Settings::SetLastHostRelayedMode( bool value )
{
	m_config->Write( _T( "/Hosting/LastRelayedMode" ), value );
}

void Settings::SetHostingPreset( const wxString& name, int optiontype, std::map<wxString, wxString> options )
{
	m_config->DeleteGroup( _T( "/Hosting/Preset/" ) + name + _T( "/" ) + TowxString( optiontype ) );
	for ( std::map<wxString, wxString>::const_iterator it = options.begin(); it != options.end(); ++it )
	{
		m_config->Write( _T( "/Hosting/Preset/" ) + name + _T( "/" ) + TowxString( optiontype ) + _T( "/" ) + it->first , it->second );
	}
}

std::map<wxString, wxString> Settings::GetHostingPreset( const wxString& name, int optiontype )
{
	wxString path_base = _T( "/Hosting/Preset/" ) + name + _T( "/" ) + TowxString( optiontype );
	std::map<wxString, wxString> ret;
	wxArrayString list = GetEntryList( path_base );

	slConfig::PathGuard pathGuard ( m_config, path_base );

	int count = list.GetCount();
	for ( int i = 0; i < count; i ++ )
	{
		wxString keyname = list[i];
		wxString val = m_config->Read( keyname );
		ret[keyname] = val;
	}
	return ret;
}


wxArrayString Settings::GetPresetList()
{
	return GetGroupList( _T( "/Hosting/Preset" ) );
}


void Settings::DeletePreset( const wxString& name )
{
	m_config->DeleteGroup( _T( "/Hosting/Preset/" ) + name );

	//delete mod default preset associated
	wxArrayString list = GetEntryList( _T( "/Hosting/ModDefaultPreset" ) );
	int count = list.GetCount();
	for ( int i = 0; i < count; i ++ )
	{
		wxString keyname = list[i];
		if ( m_config->Read( keyname ) == name ) m_config->DeleteEntry( keyname );
	}
}


wxString Settings::GetModDefaultPresetName( const wxString& modname )
{
	return m_config->Read( _T( "/Hosting/ModDefaultPreset/" ) + modname );
}


void Settings::SetModDefaultPresetName( const wxString& modname, const wxString& presetname )
{
	m_config->Write( _T( "/Hosting/ModDefaultPreset/" ) + modname, presetname );
}


void Settings::SetBalanceMethod( int value )
{
	m_config->Write( _T( "/Hosting/BalanceMethod" ), value );
}
int Settings::GetBalanceMethod()
{
	return m_config->Read( _T( "/Hosting/BalanceMethod" ), 1l );
}

void Settings::SetBalanceClans( bool value )
{
	m_config->Write( _T( "/Hosting/BalanceClans" ), value );
}
bool Settings::GetBalanceClans()
{
	return m_config->Read( _T( "/Hosting/BalanceClans" ), true );
}

void Settings::SetBalanceStrongClans( bool value )
{
	m_config->Write( _T( "/Hosting/BalanceStrongClans" ), value );
}

bool Settings::GetBalanceStrongClans()
{
	return m_config->Read( _T( "/Hosting/BalanceStrongClans" ), 0l );
}

void Settings::SetBalanceGrouping( int value )
{
	m_config->Write( _T( "/Hosting/BalanceGroupingSize" ), value );
}

int Settings::GetBalanceGrouping()
{
	return m_config->Read( _T( "/Hosting/BalanceGroupingSize" ), 0l );
}


void Settings::SetFixIDMethod( int value )
{
	m_config->Write( _T( "/Hosting/FixIDMethod" ), value );
}
int Settings::GetFixIDMethod()
{
	return m_config->Read( _T( "/Hosting/FixIDMethod" ), 1l );
}

void Settings::SetFixIDClans( bool value )
{
	m_config->Write( _T( "/Hosting/FixIDClans" ), value );
}
bool Settings::GetFixIDClans()
{
	return m_config->Read( _T( "/Hosting/FixIDClans" ), true );
}

void Settings::SetFixIDStrongClans( bool value )
{
	m_config->Write( _T( "/Hosting/FixIDStrongClans" ), value );
}

bool Settings::GetFixIDStrongClans()
{
	return m_config->Read( _T( "/Hosting/FixIDStrongClans" ), 0l );
}

void Settings::SetFixIDGrouping( int value )
{
	m_config->Write( _T( "/Hosting/FixIDGroupingSize" ), value );
}

int Settings::GetFixIDGrouping()
{
	return m_config->Read( _T( "/Hosting/FixIDGroupingSize" ), 0l );
}


wxString Settings::GetLastAI()
{
	return m_config->Read( _T( "/SinglePlayer/LastAI" ), wxEmptyString );
}

void Settings::SetDisplayJoinLeave( bool display, const wxString& channel  )
{
	m_config->Write( _T( "/Channels/DisplayJoinLeave/" )  + channel, display );
}

bool Settings::GetDisplayJoinLeave( const wxString& channel  )
{
    return m_config->Read( _T( "/Channels/DisplayJoinLeave/" ) +  channel, 0l );
}


void Settings::SetChatHistoryLenght( int historylines )
{
	m_config->Write( _T( "/Chat/HistoryLinesLenght" ), historylines );
}


int Settings::GetChatHistoryLenght()
{
	return m_config->Read( _T( "/Chat/HistoryLinesLenght" ), 1000l );
}


void Settings::SetChatPMSoundNotificationEnabled( bool enabled )
{
	m_config->Write( _T( "/Chat/PMSound" ), enabled );
}


bool Settings::GetChatPMSoundNotificationEnabled()
{
	return m_config->Read( _T( "/Chat/PMSound" ), 1l );
}

wxColour ConvertOldRGBFormat( wxString color )
{
	long R = 0, G = 0, B = 0;
	color.BeforeFirst( _T( ' ' ) ).ToLong( &R );
	color = color.AfterFirst( _T( ' ' ) );
	color.BeforeFirst( _T( ' ' ) ).ToLong( &G );
	color = color.AfterFirst( _T( ' ' ) );
	color.BeforeFirst( _T( ' ' ) ).ToLong( &B );
	return wxColour( R % 256, G % 256, B % 256 );
}

void Settings::ConvertOldColorSettings()
{
	SetChatColorNormal( ConvertOldRGBFormat( m_config->Read( _T( "/Chat/Colour/Normal" ), _T( "0 0 0" ) ) ) );
	SetChatColorBackground( ConvertOldRGBFormat( m_config->Read( _T( "/Chat/Colour/Background" ), _T( "255 255 255" ) ) ) );
	SetChatColorHighlight( ConvertOldRGBFormat( m_config->Read( _T( "/Chat/Colour/Highlight" ), _T( "255 0 0" ) ) ) );
	SetChatColorMine( ConvertOldRGBFormat( m_config->Read( _T( "/Chat/Colour/Mine" ), _T( "138 138 138" ) ) ) );
	SetChatColorNotification( ConvertOldRGBFormat( m_config->Read( _T( "/Chat/Colour/Notification" ), _T( "255 40 40" ) ) ) );
	SetChatColorServer( ConvertOldRGBFormat( m_config->Read( _T( "/Chat/Colour/Server" ), _T( "0 80 128" ) ) ) );
	SetChatColorClient( ConvertOldRGBFormat( m_config->Read( _T( "/Chat/Colour/Client" ), _T( "20 200 25" ) ) ) );
	SetChatColorJoinPart( ConvertOldRGBFormat( m_config->Read( _T( "/Chat/Colour/JoinPart" ), _T( "66 204 66" ) ) ) );
	SetChatColorError( ConvertOldRGBFormat( m_config->Read( _T( "/Chat/Colour/Error" ), _T( "128 0 0" ) ) ) );
	SetChatColorTime( ConvertOldRGBFormat( m_config->Read( _T( "/Chat/Colour/Time" ), _T( "100 100 140" ) ) ) );
	SetChatColorAction( ConvertOldRGBFormat( m_config->Read( _T( "/Chat/Colour/Action" ), _T( "230 0 255" ) ) ) );
	SetBattleLastColour( ConvertOldRGBFormat( m_config->Read( _T( "/Hosting/MyLastColour" ), _T( "255 255 0" ) ) ) );

	//convert custom color palette, note 16 colors is wx limit
	wxArrayString palettes = GetGroupList( _T( "/CustomColors" ) );
	for ( unsigned int j = 0; j < palettes.GetCount(); j++ )
	{
		wxString paletteName = palettes[j];
		for ( int i = 0; i < 16; ++i )
		{
			wxColour col( ConvertOldRGBFormat( m_config->Read( _T( "/CustomColors/" ) + paletteName + _T( "/" ) + TowxString( i ), _T( "255 255 255" ) ) ) );
			m_config->Write( _T( "/CustomColors/" ) + paletteName + _T( "/" ) + TowxString( i ), col.GetAsString( wxC2S_HTML_SYNTAX ) );
		}
	}

	wxArrayString groups = GetGroupList( _T( "/Groups" ) );
	for ( unsigned int j = 0; j < groups.GetCount(); j++ )
	{
		wxString group = groups[j];
		wxColour col( ConvertOldRGBFormat ( m_config->Read( _T( "/Groups/" ) + group + _T( "/Opts/HLColor" ) , _T( "100 100 140" ) ) ) );
		m_config->Write( _T( "/Groups/" ) + group + _T( "/Opts/HLColor" ), col.GetAsString( wxC2S_HTML_SYNTAX ) );
	}

}

wxColour Settings::GetChatColorNormal()
{
	return wxColour( m_config->Read( _T( "/Chat/Colour/Normal" ), _T( "#000000" ) ) );
}

void Settings::SetChatColorNormal( wxColour value )
{
	m_config->Write( _T( "/Chat/Colour/Normal" ), value.GetAsString( wxC2S_CSS_SYNTAX ) );
}


wxColour Settings::GetChatColorBackground()
{
	return wxColour( m_config->Read( _T( "/Chat/Colour/Background" ), _T( "#FFFFFF" ) ) );
}

void Settings::SetChatColorBackground( wxColour value )
{
	m_config->Write( _T( "/Chat/Colour/Background" ), value.GetAsString( wxC2S_CSS_SYNTAX ) );
}

wxColour Settings::GetChatColorHighlight()
{
	return wxColour( m_config->Read( _T( "/Chat/Colour/Highlight" ), _T( "#FF0000" ) ) );
}

void Settings::SetChatColorHighlight( wxColour value )
{
	m_config->Write( _T( "/Chat/Colour/Highlight" ), value.GetAsString( wxC2S_CSS_SYNTAX ) );
}

wxColour Settings::GetChatColorMine()
{
	return wxColour( m_config->Read( _T( "/Chat/Colour/Mine" ), _T( "#8A8A8A" ) ) );
}

void Settings::SetChatColorMine( wxColour value )
{
	m_config->Write( _T( "/Chat/Colour/Mine" ), value.GetAsString( wxC2S_CSS_SYNTAX ) );
}

wxColour Settings::GetChatColorNotification()
{
	return wxColour( m_config->Read( _T( "/Chat/Colour/Notification" ), _T( "#FF2828" ) ) );
}

void Settings::SetChatColorNotification( wxColour value )
{
	m_config->Write( _T( "/Chat/Colour/Notification" ), value.GetAsString( wxC2S_CSS_SYNTAX ) );
}

wxColour Settings::GetChatColorAction()
{
	return wxColour( m_config->Read( _T( "/Chat/Colour/Action" ), _T( "#E600FF" ) ) );
}

void Settings::SetChatColorAction( wxColour value )
{
	m_config->Write( _T( "/Chat/Colour/Action" ), value.GetAsString( wxC2S_CSS_SYNTAX ) );
}

wxColour Settings::GetChatColorServer()
{
	return wxColour( m_config->Read( _T( "/Chat/Colour/Server" ), _T( "#005080" ) ) );
}

void Settings::SetChatColorServer( wxColour value )
{
	m_config->Write( _T( "/Chat/Colour/Server" ), value.GetAsString( wxC2S_CSS_SYNTAX ) );
}

wxColour Settings::GetChatColorClient()
{
	return wxColour( m_config->Read( _T( "/Chat/Colour/Client" ), _T( "#14C819" ) ) );
}

void Settings::SetChatColorClient( wxColour value )
{
	m_config->Write( _T( "/Chat/Colour/Client" ), value.GetAsString( wxC2S_CSS_SYNTAX ) );
}

wxColour Settings::GetChatColorJoinPart()
{
	return wxColour( m_config->Read( _T( "/Chat/Colour/JoinPart" ), _T( "#42CC42" ) ) );
}

void Settings::SetChatColorJoinPart( wxColour value )
{
	m_config->Write( _T( "/Chat/Colour/JoinPart" ), value.GetAsString( wxC2S_CSS_SYNTAX ) );
}

wxColour Settings::GetChatColorError()
{
	return wxColour( m_config->Read( _T( "/Chat/Colour/Error" ), _T( "#800000" ) ) );
}

void Settings::SetChatColorError( wxColour value )
{
	m_config->Write( _T( "/Chat/Colour/Error" ), value.GetAsString( wxC2S_CSS_SYNTAX ) );
}

wxColour Settings::GetChatColorTime()
{
	return wxColour( m_config->Read( _T( "/Chat/Colour/Time" ), _T( "#64648C" ) ) );
}

void Settings::SetChatColorTime( wxColour value )
{
	m_config->Write( _T( "/Chat/Colour/Time" ), value.GetAsString( wxC2S_CSS_SYNTAX ) );
}

wxFont Settings::GetChatFont()
{
	wxString info = m_config->Read( _T( "/Chat/Font" ), wxEmptyString );
	if ( info != wxEmptyString ) {
		wxFont f;
		f.SetNativeFontInfo( info );
		return f;
	}
	else {
		wxFont f( 8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL );
		return f;
	}
}

void Settings::SetChatFont( wxFont value )
{
	m_config->Write( _T( "/Chat/Font" ), value.GetNativeFontInfoDesc() );
}


bool Settings::GetSmartScrollEnabled()
{
	return m_config->Read( _T( "/Chat/SmartScrollEnabled" ), true );
}

void Settings::SetSmartScrollEnabled( bool value ) {
	m_config->Write( _T( "/Chat/SmartScrollEnabled" ), value );
}

bool Settings::GetAlwaysAutoScrollOnFocusLost()
{
	return m_config->Read( _T( "/Chat/AlwaysAutoScrollOnFocusLost" ), true );
}

void Settings::SetAlwaysAutoScrollOnFocusLost( bool value )
{
	m_config->Write( _T( "/Chat/AlwaysAutoScrollOnFocusLost" ), value );
}

void Settings::ConvertOldHiglightSettings()
{
	SetHighlightedWords( wxStringTokenize( m_config->Read( _T( "/Chat/HighlightedWords" ), _T( "" ) ), _T( ";" ) ) );
}

void Settings::SetUseIrcColors( bool value )
{
	m_config->Write( _T( "/Chat/UseIrcColors" ), value );
}

bool Settings::GetUseIrcColors()
{
	return m_config->Read( _T( "/Chat/UseIrcColors" ), true );
}

void Settings::setFromList(const wxArrayString& list, const wxString& path)
{
    wxString string;
    for ( unsigned int i = 0; i < list.GetCount(); i++ )
        string << list[i] << _T( ";" );
    m_config->Write( path, string );
}

wxArrayString Settings::getFromList(const wxString& path)
{
    return wxStringTokenize( m_config->Read( path, wxString() ), _T(";") );
}

void Settings::SetHighlightedWords( const wxArrayString& words )
{
    setFromList( words, _T("/Chat/HighlightedWords") );
}

wxArrayString Settings::GetHighlightedWords()
{
    return getFromList( _T("/Chat/HighlightedWords") );
}

void Settings::ConvertLists()
{
    const wxArrayString current_hl = GetEntryList( _T( "/Chat/HighlightedWords" ) );
    m_config->DeleteGroup( _T( "/Chat/HighlightedWords" ) );
    SaveSettings();
    SetHighlightedWords( current_hl );
    SaveSettings();
}

void Settings::SetRequestAttOnHighlight( const bool req )
{
	m_config->Write( _T( "/Chat/ReqAttOnHighlight" ), req );
}

bool Settings::GetRequestAttOnHighlight( )
{
	return m_config->Read( _T( "/Chat/ReqAttOnHighlight" ), 0l );
}

BattleListFilterValues Settings::GetBattleFilterValues( const wxString& profile_name )
{
	BattleListFilterValues filtervalues;
	filtervalues.description =      m_config->Read( _T( "/BattleFilter/" ) + profile_name + _T( "/description" ), _T( "" ) );
	filtervalues.host =             m_config->Read( _T( "/BattleFilter/" ) + profile_name + _T( "/host" ), _T( "" ) );
	filtervalues.map =               m_config->Read( _T( "/BattleFilter/" ) + profile_name + _T( "/map" ), _T( "" ) );
	filtervalues.map_show =         m_config->Read( _T( "/BattleFilter/" ) + profile_name + _T( "/map_show" ), 0L );
	filtervalues.maxplayer =        m_config->Read( _T( "/BattleFilter/" ) + profile_name + _T( "/maxplayer" ), _T( "All" ) );
	filtervalues.maxplayer_mode =   m_config->Read( _T( "/BattleFilter/" ) + profile_name + _T( "/maxplayer_mode" ), _T( "=" ) );
	filtervalues.mod =              m_config->Read( _T( "/BattleFilter/" ) + profile_name + _T( "/mod" ), _T( "" ) );
	filtervalues.mod_show =         m_config->Read( _T( "/BattleFilter/" ) + profile_name + _T( "/mod_show" ), 0L );
	filtervalues.player_mode =      m_config->Read( _T( "/BattleFilter/" ) + profile_name + _T( "/player_mode" ), _T( "=" ) );
	filtervalues.player_num  =      m_config->Read( _T( "/BattleFilter/" ) + profile_name + _T( "/player_num" ), _T( "All" ) );
	filtervalues.rank =             m_config->Read( _T( "/BattleFilter/" ) + profile_name + _T( "/rank" ), _T( "All" ) );
	filtervalues.rank_mode =        m_config->Read( _T( "/BattleFilter/" ) + profile_name + _T( "/rank_mode" ), _T( "<" ) );
	filtervalues.spectator =        m_config->Read( _T( "/BattleFilter/" ) + profile_name + _T( "/spectator" ), _T( "All" ) );
	filtervalues.spectator_mode =   m_config->Read( _T( "/BattleFilter/" ) + profile_name + _T( "/spectator_mode" ), _T( "=" ) );
	filtervalues.status_full =      m_config->Read( _T( "/BattleFilter/" ) + profile_name + _T( "/status_full" ), true );
	filtervalues.status_locked =    m_config->Read( _T( "/BattleFilter/" ) + profile_name + _T( "/status_locked" ), true );
	filtervalues.status_open =      m_config->Read( _T( "/BattleFilter/" ) + profile_name + _T( "/status_open" ), true );
	filtervalues.status_passworded = m_config->Read( _T( "/BattleFilter/" ) + profile_name + _T( "/status_passworded" ), true );
	filtervalues.status_start =     m_config->Read( _T( "/BattleFilter/" ) + profile_name + _T( "/status_start" ), true );
	filtervalues.highlighted_only = m_config->Read( _T( "/BattleFilter/" ) + profile_name + _T( "/highlighted_only" ), 0l );
	return filtervalues;
}

void Settings::SetBattleFilterValues( const BattleListFilterValues& filtervalues, const wxString& profile_name )
{
	m_config->Write( _T( "/BattleFilter/" ) + profile_name + _T( "/description" ), filtervalues.description );
	m_config->Write( _T( "/BattleFilter/" ) + profile_name + _T( "/host" ), filtervalues.host );
	m_config->Write( _T( "/BattleFilter/" ) + profile_name + _T( "/map" ), filtervalues.map );
	m_config->Write( _T( "/BattleFilter/" ) + profile_name + _T( "/map_show" ), filtervalues.map_show );
	m_config->Write( _T( "/BattleFilter/" ) + profile_name + _T( "/maxplayer" ), filtervalues.maxplayer );
	m_config->Write( _T( "/BattleFilter/" ) + profile_name + _T( "/maxplayer_mode" ), filtervalues.maxplayer_mode );
	m_config->Write( _T( "/BattleFilter/" ) + profile_name + _T( "/mod" ), filtervalues.mod );
	m_config->Write( _T( "/BattleFilter/" ) + profile_name + _T( "/mod_show" ), filtervalues.mod_show );
	m_config->Write( _T( "/BattleFilter/" ) + profile_name + _T( "/player_mode" ), filtervalues.player_mode );
	m_config->Write( _T( "/BattleFilter/" ) + profile_name + _T( "/player_num" ), filtervalues.player_num );
	m_config->Write( _T( "/BattleFilter/" ) + profile_name + _T( "/rank" ), filtervalues.rank );
	m_config->Write( _T( "/BattleFilter/" ) + profile_name + _T( "/rank_mode" ), filtervalues.rank_mode );
	m_config->Write( _T( "/BattleFilter/" ) + profile_name + _T( "/spectator" ), filtervalues.spectator );
	m_config->Write( _T( "/BattleFilter/" ) + profile_name + _T( "/spectator_mode" ), filtervalues.spectator_mode );
	m_config->Write( _T( "/BattleFilter/" ) + profile_name + _T( "/status_full" ), filtervalues.status_full );
	m_config->Write( _T( "/BattleFilter/" ) + profile_name + _T( "/status_locked" ), filtervalues.status_locked );
	m_config->Write( _T( "/BattleFilter/" ) + profile_name + _T( "/status_open" ), filtervalues.status_open );
	m_config->Write( _T( "/BattleFilter/" ) + profile_name + _T( "/status_passworded" ), filtervalues.status_passworded );
	m_config->Write( _T( "/BattleFilter/" ) + profile_name + _T( "/status_start" ), filtervalues.status_start );
	m_config->Write( _T( "/BattleFilter/" ) + profile_name + _T( "/highlighted_only" ), filtervalues.highlighted_only );
	m_config->Write( _T( "/BattleFilter/lastprofile" ), profile_name );
}

bool Settings::GetBattleFilterActivState() const
{
	return m_config->Read( _T( "/BattleFilter/Active" ) , 0l );
}

void Settings::SetBattleFilterActivState( const bool state )
{
	m_config->Write( _T( "/BattleFilter/Active" ) , state );
}

bool Settings::GetBattleLastAutoStartState()
{
	return m_config->Read( _T( "/Hosting/AutoStart" ) , 0l );
}

void Settings::SetBattleLastAutoStartState( bool value )
{
	m_config->Write( _T( "/Hosting/AutoStart" ), value );
}

bool Settings::GetBattleLastAutoControlState()
{
	return m_config->Read( _T( "/Hosting/AutoControl" ) , 0l );
}

void Settings::SetBattleLastAutoControlState( bool value )
{
	m_config->Write( _T( "/Hosting/AutoControl" ), value );
}

int Settings::GetBattleLastAutoSpectTime()
{
	return m_config->Read( _T( "/Hosting/AutoSpectTime" ) , 0l );
}

void Settings::SetBattleLastAutoSpectTime( int value )
{
	m_config->Write( _T( "/Hosting/AutoSpectTime" ) , value );
}

bool Settings::GetBattleLastAutoAnnounceDescription()
{
	return m_config->Read( _T( "/Hosting/AutoAnnounceDescription" ) , 0l );
}

void Settings::SetBattleLastAutoAnnounceDescription( bool value )
{
	m_config->Write( _T( "/Hosting/AutoAnnounceDescription" ) , value );
}

void Settings::SetBattleLastSideSel( const wxString& modname, int sidenum )
{
	m_config->Write(_T("/Battle/Sides/" + modname), sidenum);
}

int Settings::GetBattleLastSideSel( const wxString& modname )
{
	if (modname.IsEmpty())
		return 0;
	return m_config->Read( _T("/Battle/Sides/" + modname), 0l );
}

void Settings::SetMapLastStartPosType( const wxString& mapname, const wxString& startpostype )
{
	m_config->Write( _T( "/Hosting/MapLastValues/" ) + mapname + _T( "/startpostype" ), startpostype );
}

void Settings::SetMapLastRectPreset( const wxString& mapname, std::vector<Settings::SettStartBox> rects )
{
	wxString basepath = _T( "/Hosting/MapLastValues/" ) + mapname + _T( "/Rects" );
	m_config->DeleteGroup( basepath );
	for ( std::vector<Settings::SettStartBox>::const_iterator itor = rects.begin(); itor != rects.end(); ++itor )
	{
		SettStartBox box = *itor;
		wxString additionalpath = basepath + _T( "/Rect" ) + TowxString( box.ally ) + _T( "/" );
		m_config->Write( additionalpath + _T( "TopLeftX" ), box.topx );
		m_config->Write( additionalpath + _T( "TopLeftY" ), box.topy );
		m_config->Write( additionalpath + _T( "BottomRightX" ), box.bottomx );
		m_config->Write( additionalpath + _T( "BottomRightY" ), box.bottomy );
		m_config->Write( additionalpath + _T( "AllyTeam" ), box.ally );
	}
}

wxString Settings::GetMapLastStartPosType( const wxString& mapname )
{
	return m_config->Read( _T( "/Hosting/MapLastValues/" ) + mapname + _T( "/startpostype" ), _T( "" ) );
}

std::vector<Settings::SettStartBox> Settings::GetMapLastRectPreset( const wxString& mapname )
{
	wxString basepath = _T( "/Hosting/MapLastValues/" ) + mapname + _T( "/Rects" );
	wxArrayString boxes = GetGroupList( basepath );
	std::vector<Settings::SettStartBox> ret;
	for ( unsigned int i = 0; i < boxes.GetCount(); i++ )
	{
		wxString additionalpath = basepath + _T( "/" ) + boxes[i] + _T( "/" );
		SettStartBox box;
		box.topx = m_config->Read( additionalpath + _T( "TopLeftX" ), -1 );
		box.topy = m_config->Read( additionalpath + _T( "TopLeftY" ), -1 );
		box.bottomx = m_config->Read( additionalpath + _T( "BottomRightX" ), -1 );
		box.bottomy = m_config->Read( additionalpath + _T( "BottomRightY" ), -1 );
		box.ally = m_config->Read( additionalpath + _T( "AllyTeam" ), -1 );
		ret.push_back( box );
	}
	return ret;
}

bool Settings::GetDisableSpringVersionCheck()
{
	bool ret;
	m_config->Read( _T( "/Spring/DisableVersionCheck" ), &ret, false );
	return ret;
}

void Settings::SetDisableSpringVersionCheck(bool disable)
{
    m_config->Write( _T( "/Spring/DisableVersionCheck" ), ( bool )disable );
}

wxString Settings::GetLastBattleFilterProfileName()
{
	return  m_config->Read( _T( "/BattleFilter/lastprofile" ), _T( "default" ) );
}


unsigned int Settings::GetTorrentPort()
{
	return  ( unsigned int )m_config->Read( _T( "/Torrent/Port" ), DEFSETT_SPRING_PORT + 1 );
}


void Settings::SetTorrentPort( unsigned int port )
{
	m_config->Write( _T( "/Torrent/port" ), ( int )port );
}


int Settings::GetTorrentUploadRate()
{
	return  m_config->Read( _T( "/Torrent/UploadRate" ), -1 );
}


void Settings::SetTorrentUploadRate( int speed )
{
	m_config->Write( _T( "/Torrent/UploadRate" ), speed );
}


int Settings::GetTorrentDownloadRate()
{
	return  m_config->Read( _T( "/Torrent/DownloadRate" ), -1 );
}



void Settings::SetTorrentDownloadRate( int speed )
{
	m_config->Write( _T( "/Torrent/DownloadRate" ), speed );
}


int Settings::GetTorrentSystemSuspendMode()
{
	return  m_config->Read( _T( "/Torrent/SuspendMode" ), 0l );
}



void Settings::SetTorrentSystemSuspendMode( int mode )
{
	m_config->Write( _T( "/Torrent/SuspendMode" ), mode );
}


int Settings::GetTorrentThrottledUploadRate()
{
	return  m_config->Read( _T( "/Torrent/ThrottledUploadRate" ), 0l );
}


void Settings::SetTorrentThrottledUploadRate( int speed )
{
	m_config->Write( _T( "/Torrent/ThrottledUploadRate" ), speed );
}


int Settings::GetTorrentThrottledDownloadRate()
{
	return  m_config->Read( _T( "/Torrent/ThrottledDownloadRate" ), 0l );
}


void Settings::SetTorrentThrottledDownloadRate( int speed )
{
	m_config->Write( _T( "/Torrent/ThrottledDownloadRate" ), speed );
}


void Settings::SetTorrentMaxConnections( int connections )
{
	m_config->Write( _T( "/Torrent/MaxConnections" ), connections );
}


int Settings::GetTorrentMaxConnections()
{
	return  m_config->Read( _T( "/Torrent/MaxConnections" ), 250 );
}

void Settings::SetTorrentListToResume( const std::vector<wxString>& list )
{
	unsigned int TorrentCount = list.size();
	m_config->DeleteGroup( _T( "/Torrent/ResumeList" ) );
	for ( unsigned int i = 0; i < TorrentCount; i++ )
	{
		m_config->Write( _T( "/Torrent/ResumeList/" ) + TowxString( i ), list[i] );
	}
}


std::vector<wxString> Settings::GetTorrentListToResume()
{
	std::vector<wxString> list;
	slConfig::PathGuard pathGuard ( m_config, _T( "/Torrent/ResumeList" ) );
	wxArrayString entries = GetEntryList( _T( "/Torrent/ResumeList" ) );
	for ( size_t i = 0; i < entries.Count(); ++i )
	{
		wxString ToAdd;
		if ( m_config->Read( entries[i], &ToAdd ) )
			list.push_back( ToAdd );
	}
	return list;
}

void Settings::ClearTorrentListToResume()
{
	m_config->DeleteGroup( _T("/Torrent/ResumeList") );
}

wxFileName Settings::GetTorrentDir()
{

	wxFileName torrentDir( GetLobbyWriteDir() );
	torrentDir.AppendDir( _T( "torrents" ) );
/*
	if ( !torrentDir.DirExists() )
	{
		if ( !torrentDir.Mkdir( 0755 ) ) torrentDir.Clear();
	}
*/
	return torrentDir;
}

wxFileName Settings::GetTorrentDataDir()
{
	wxFileName torrentDir( GetLobbyWriteDir() );
	torrentDir.AppendDir( _T( "downloads" ) );
/*
	if ( !torrentDir.DirExists() )
	{
		if ( !torrentDir.Mkdir( 0755 ) ) torrentDir.Clear();
	}
*/
	return torrentDir;
}

wxString Settings::GetTempStorage()
{
	return wxFileName::GetTempDir();
}


void Settings::SetShowTooltips( bool show )
{
	m_config->Write( _T( "/GUI/ShowTooltips" ), show );
}

bool Settings::GetShowTooltips()
{
	return m_config->Read( _T( "/GUI/ShowTooltips" ), 1l );
}

void Settings::RemoveLayouts()
{
	m_config->DeleteEntry(_T("/GUI/DefaultLayout"));
	m_config->DeleteGroup(_T("/Layout"));
	m_config->DeleteGroup(_T("/GUI/AUI"));
}

void Settings::SetColumnWidth( const wxString& list_name, const int column_ind, const int column_width )
{
	m_config->Write( _T( "GUI/ColumnWidths/" ) + list_name + _T( "/" ) + TowxString( column_ind ), column_width );
}

int Settings::GetColumnWidth( const wxString& list_name, const int column )
{
	const int orgwidth = m_config->Read( _T( "/GUI/ColumnWidths/" ) + list_name + _T( "/" ) + TowxString( column ), columnWidthUnset );
	int width = orgwidth;
	if ( orgwidth > -1 ) //-3 is unset, -2 and -1 used for auto size by wx
		width = std::max ( width, int( Settings::columnWidthMinimum ) ); //removing the temporary creation here gives me undefined ref error (koshi)
	return width;
}

void Settings::NukeColumnWidths()
{
	m_config->DeleteGroup(_T("/GUI/ColumnWidths/"));
}

void Settings::SetPeopleList( const wxArrayString& friends, const wxString& group  )
{
	unsigned int friendsCount = friends.GetCount();
	m_config->DeleteGroup( _T( "/Groups/" ) + group + _T( "/Members/" ) );
	for ( unsigned int i = 0; i < friendsCount ; i++ )
	{
		m_config->Write( _T( "/Groups/" ) + group + _T( "/Members/" ) + TowxString( i ), friends[i] );
	}
}

wxArrayString Settings::GetPeopleList( const wxString& group  ) const
{
	wxArrayString list;
	slConfig::PathGuard pathGuard ( m_config, _T( "/Groups/" ) + group + _T( "/Members/" ) );
	unsigned int friendsCount  = m_config->GetNumberOfEntries( false );
	for ( unsigned int i = 0; i < friendsCount ; i++ )
	{
		wxString ToAdd;
		if ( m_config->Read( _T( "/Groups/" ) + group + _T( "/Members/" ) +  TowxString( i ), &ToAdd ) ) list.Add( ToAdd );
	}
	return list;
}

void Settings::SetGroupHLColor( const wxColour& color, const wxString& group )
{
	m_config->Write( _T( "/Groups/" ) + group + _T( "/Opts/HLColor" ), color.GetAsString( wxC2S_HTML_SYNTAX ) );
}

wxColour Settings::GetGroupHLColor( const wxString& group  ) const
{
	return wxColour( m_config->Read( _T( "/Groups/" ) + group + _T( "/Opts/HLColor" ) , _T( "#64648C" ) ) );
}

wxArrayString Settings::GetGroups( )
{
	return GetGroupList( _T( "/Groups/" ) );
}

void Settings::AddGroup( const wxString& group )
{
	if ( !m_config->Exists( _T( "/Groups/" ) + group ) )
	{
		m_config->Write( _T( "/Groups/" ) , group );
		//set defaults
		SetGroupActions( group, UserActions::ActNone );
		SetGroupHLColor( defaultHLcolor, group );
	}
}

void Settings::DeleteGroup( const wxString& group )
{
	if ( m_config->Exists( _T( "/Groups/" ) + group ) )
	{
		m_config->DeleteGroup( _T( "/Groups/" ) + group );
	}
}

void Settings::SetGroupActions( const wxString& group, UserActions::ActionType action )
{
	//m_config->Write( _T("/Groups/") + group + _T("/Opts/Actions"), action );
	wxString key = _T( "/Groups/" ) + group + _T( "/Opts/ActionsList" );
	m_config->DeleteGroup( key );
	key += _T( "/" );
	unsigned int tmp = action & ( ( UserActions::ActLast << 1 ) - 1 );
	int i = 0;
	while ( tmp != 0 )
	{
		if ( tmp&1 )m_config->Write( key + m_configActionNames[i], true );
		i++;
		tmp >>= 1;
	}
}

UserActions::ActionType Settings::GetGroupActions( const wxString& group ) const
{
	wxString key = _T( "/Groups/" ) + group + _T( "/Opts/Actions" );
	if ( m_config->HasEntry( key ) )// Backward compatibility.
	{
		wxLogMessage( _T( "loading deprecated group actions and updating config" ) );
		UserActions::ActionType action = ( UserActions::ActionType )m_config->Read( key, ( long )UserActions::ActNone ) ;
		m_config->DeleteEntry( key );

		// a bit ugly, but i want to update options
		Settings *this_nonconst = const_cast<Settings*>( this );
		this_nonconst->SetGroupActions( group, action );

		return action;
	}
	key = _T( "/Groups/" ) + group + _T( "/Opts/ActionsList" );
	if ( !m_config->Exists( key ) ) return UserActions::ActNone;
	key += _T( "/" );
	int i = 0;
	int mask = 1;
	int result = 0;
	while ( mask <= UserActions::ActLast )
	{
		if ( m_config->Read( key + m_configActionNames[i], 0l ) )
		{
			result |= mask;
		}
		i++;
		mask <<= 1;
	}
	if ( result == 0 ) return UserActions::ActNone;
	return ( UserActions::ActionType )result;
}

bool Settings::ShouldAddDefaultGroupSettings()
{
	return !m_config->Exists( _T( "/Groups" ) );
}

void Settings::SaveCustomColors( const wxColourData& _cdata, const wxString& paletteName  )
{
	//note 16 colors is wx limit
	wxColourData cdata = _cdata;
	for ( int i = 0; i < 16; ++i )
	{
		wxColour col = cdata.GetCustomColour( i );
		if ( !col.IsOk() )
			col = wxColour ( 255, 255, 255 );
		m_config->Write( _T( "/CustomColors/" ) + paletteName + _T( "/" ) + TowxString( i ),  col.GetAsString( wxC2S_HTML_SYNTAX ) ) ;
	}
}

wxColourData Settings::GetCustomColors( const wxString& paletteName )
{
	wxColourData cdata;
	//note 16 colors is wx limit
	for ( int i = 0; i < 16; ++i )
	{
		wxColour col( m_config->Read( _T( "/CustomColors/" ) + paletteName + _T( "/" ) + TowxString( i ), wxColour ( 255, 255, 255 ).GetAsString( wxC2S_HTML_SYNTAX ) ) );
		cdata.SetCustomColour( i, col );
	}

	return cdata;
}


bool Settings::GetReportStats()
{
	return m_config->Read( _T( "/General/reportstats" ), 1l );
}


void Settings::SetReportStats( const bool value )
{
	m_config->Write( _T( "/General/reportstats" ), value );
}


void Settings::SetAutoUpdate( const bool value )
{
	m_config->Write( _T( "/General/AutoUpdate" ), value );
}


bool Settings::GetAutoUpdate()
{
	return m_config->Read( _T( "/General/AutoUpdate" ), true );
}

PlaybackListFilterValues Settings::GetReplayFilterValues( const wxString& profile_name )
{
	PlaybackListFilterValues filtervalues;
	filtervalues.duration =         m_config->Read( _T( "/ReplayFilter/" ) + profile_name + _T( "/duration" ), _T( "" ) );
	filtervalues.map =               m_config->Read( _T( "/ReplayFilter/" ) + profile_name + _T( "/map" ), _T( "" ) );
	filtervalues.map_show =         m_config->Read( _T( "/ReplayFilter/" ) + profile_name + _T( "/map_show" ), 0L );
	filtervalues.filesize  =        m_config->Read( _T( "/ReplayFilter/" ) + profile_name + _T( "/filesize" ), _T( "" ) );
	filtervalues.filesize_mode  =   m_config->Read( _T( "/ReplayFilter/" ) + profile_name + _T( "/filesize_mode" ), _T( ">" ) );
	filtervalues.duration_mode  =   m_config->Read( _T( "/ReplayFilter/" ) + profile_name + _T( "/duration_mode" ), _T( ">" ) );
	filtervalues.mod =              m_config->Read( _T( "/ReplayFilter/" ) + profile_name + _T( "/mod" ), _T( "" ) );
	filtervalues.mod_show =         m_config->Read( _T( "/ReplayFilter/" ) + profile_name + _T( "/mod_show" ), 0L );
	filtervalues.player_mode =      m_config->Read( _T( "/ReplayFilter/" ) + profile_name + _T( "/player_mode" ), _T( "=" ) );
	filtervalues.player_num  =      m_config->Read( _T( "/ReplayFilter/" ) + profile_name + _T( "/player_num" ), _T( "All" ) );

	return filtervalues;
}

void Settings::SetReplayFilterValues( const PlaybackListFilterValues& filtervalues, const wxString& profile_name )
{
	m_config->Write( _T( "/ReplayFilter/" ) + profile_name + _T( "/duration" ), filtervalues.duration );
	m_config->Write( _T( "/ReplayFilter/" ) + profile_name + _T( "/map" ), filtervalues.map );
	m_config->Write( _T( "/ReplayFilter/" ) + profile_name + _T( "/map_show" ), filtervalues.map_show );
	m_config->Write( _T( "/ReplayFilter/" ) + profile_name + _T( "/filesize" ), filtervalues.filesize );
	m_config->Write( _T( "/ReplayFilter/" ) + profile_name + _T( "/filesize_mode" ), filtervalues.filesize_mode );
	m_config->Write( _T( "/ReplayFilter/" ) + profile_name + _T( "/duration_mode" ), filtervalues.duration_mode );
	m_config->Write( _T( "/ReplayFilter/" ) + profile_name + _T( "/mod" ), filtervalues.mod );
	m_config->Write( _T( "/ReplayFilter/" ) + profile_name + _T( "/mod_show" ), filtervalues.mod_show );
	m_config->Write( _T( "/ReplayFilter/" ) + profile_name + _T( "/player_mode" ), filtervalues.player_mode );
	m_config->Write( _T( "/ReplayFilter/" ) + profile_name + _T( "/player_num" ), filtervalues.player_num );
	m_config->Write( _T( "/ReplayFilter/lastprofile" ), profile_name );
}

bool Settings::GetReplayFilterActivState() const
{
	return m_config->Read( _T( "/ReplayFilter/Active" ) , 0l );
}

void Settings::SetReplayFilterActivState( const bool state )
{
	m_config->Write( _T( "/ReplayFilter/Active" ) , state );
}

wxString Settings::GetLastReplayFilterProfileName()
{
	return  m_config->Read( _T( "/ReplayFilter/lastprofile" ), _T( "default" ) );
}
wxString Settings::GetLastRelayedHost()
{
    return  m_config->Read(_T("/General/RelayHost"),_T(""));
}
void Settings::SetLastRelayedHost(wxString relhost)
{
    m_config->Write(_T("/General/RelayHost"),relhost);
}
void Settings::SetCompletionMethod( CompletionMethod method )
{
	m_config->Write( _T( "/General/CompletionMethod" ), ( int )method );
}
Settings::CompletionMethod Settings::GetCompletionMethod(  ) const
{
	return  ( CompletionMethod )m_config->Read( _T( "/General/CompletionMethod" ), ( int )MatchExact );
}


unsigned int Settings::GetHorizontalSortkeyIndex()
{
	return m_config->Read( _T( "/GUI/MapSelector/HorizontalSortkeyIndex" ), 0l );
}

void Settings::SetHorizontalSortkeyIndex( const unsigned int idx )
{
	m_config->Write( _T( "/GUI/MapSelector/HorizontalSortkeyIndex" ), ( int ) idx );
}

unsigned int Settings::GetVerticalSortkeyIndex()
{
	return m_config->Read( _T( "/GUI/MapSelector/VerticalSortkeyIndex" ), 0l );
}

void Settings::SetVerticalSortkeyIndex( const unsigned int idx )
{
	m_config->Write( _T( "/GUI/MapSelector/VerticalSortkeyIndex" ), ( int ) idx );
}

bool Settings::GetHorizontalSortorder()
{
	return m_config->Read( _T( "/GUI/MapSelector/HorizontalSortorder" ), 0l );
}

void Settings::SetHorizontalSortorder( const bool order )
{
	m_config->Write( _T( "/GUI/MapSelector/HorizontalSortorder" ), order );
}

bool Settings::GetVerticalSortorder()
{
	return m_config->Read( _T( "/GUI/MapSelector/VerticalSortorder" ), 0l );
}

void Settings::SetVerticalSortorder( const bool order )
{
	m_config->Write( _T( "/GUI/MapSelector/VerticalSortorder" ), order );
}

void Settings::SetMapSelectorFollowsMouse( bool value )
{
	m_config->Write( _T( "/GUI/MapSelector/SelectionFollowsMouse" ), value );
}

bool Settings::GetMapSelectorFollowsMouse()
{
	return m_config->Read( _T( "/GUI/MapSelector/SelectionFollowsMouse" ), 0l );
}

unsigned int Settings::GetMapSelectorFilterRadio()
{
	return m_config->Read( _T( "/GUI/MapSelector/FilterRadio" ), 0l );
}

void Settings::SetMapSelectorFilterRadio( const unsigned int val )
{
	m_config->Write( _T( "/GUI/MapSelector/FilterRadio" ), ( int ) val );
}

//////////////////////////////////////////////////////////////////////////////
///                            SpringSettings                              ///
//////////////////////////////////////////////////////////////////////////////


long Settings::getMode()
{
	return m_config->Read( _T( "/SpringSettings/mode" ), SET_MODE_SIMPLE );
}

void Settings::setMode( long mode )
{
	m_config->Write( _T( "/SpringSettings/mode" ), mode );
}

bool Settings::getDisableWarning()
{
	return m_config->Read( _T( "/SpringSettings/disableWarning" ), 0l );
}

void Settings::setDisableWarning( bool disable )
{
	m_config->Write( _T( "/SpringSettings/disableWarning" ), disable );
}


wxString Settings::getSimpleRes()
{
	wxString def = vl_Resolution_Str[1];
	m_config->Read( _T( "/SpringSettings/SimpleRes" ), &def );
	return def;
}
void Settings::setSimpleRes( wxString res )
{
	m_config->Write( _T( "/SpringSettings/SimpleRes" ), res );
}

wxString Settings::getSimpleQuality()
{
	wxString def = wxT( "medium" );
	m_config->Read( _T( "/SpringSettings/SimpleQuality" ), &def );
	return def;
}

void Settings::setSimpleQuality( wxString qual )
{
	m_config->Write( _T( "/SpringSettings/SimpleQuality" ), qual );
}

wxString Settings::getSimpleDetail()
{
	wxString def = wxT( "medium" );
	m_config->Read( _T( "/SpringSettings/SimpleDetail" ), &def );
	return def;
}

void Settings::setSimpleDetail( wxString det )
{
	m_config->Write( _T( "/SpringSettings/SimpleDetail" ), det );
}

bool Settings::IsSpringBin( const wxString& path )
{
	if ( !wxFile::Exists( path ) ) return false;
	if ( !wxFileName::IsFileExecutable( path ) ) return false;
	return true;
}

void Settings::SetLanguageID ( const long id )
{
	m_config->Write( _T( "/General/LanguageID" ) , id );
}

long Settings::GetLanguageID ( )
{
	return m_config->Read( _T( "/General/LanguageID" ) , wxLANGUAGE_DEFAULT  );
}

SortOrder Settings::GetSortOrder( const wxString& list_name )
{
	SortOrder order;
	slConfig::PathGuard pathGuard ( m_config, _T( "/UI/SortOrder/" ) + list_name + _T( "/" ) );
	unsigned int entries  = m_config->GetNumberOfGroups( false ); //do not recurse
	for ( unsigned int i = 0; i < entries ; i++ )
	{
		SortOrderItem it;
		it.direction = m_config->Read( TowxString( i ) + _T( "/dir" ), 1 );
		it.col = m_config->Read( TowxString( i ) + _T( "/col" ), i );
		order[i] = it;
	}
	return order;
}

void Settings::SetSortOrder( const wxString& list_name, const SortOrder& order  )
{
	SortOrder::const_iterator it = order.begin();
	for ( ; it != order.end(); ++it ) {
		m_config->Write( _T( "/UI/SortOrder/" ) + list_name + _T( "/" ) + TowxString( it->first ) + _T( "/dir" ), it->second.direction );
		m_config->Write( _T( "/UI/SortOrder/" ) + list_name + _T( "/" ) + TowxString( it->first ) + _T( "/col" ), it->second.col );
	}
}

bool Settings::GetUseTabIcons()
{
	return m_config->Read( _T( "/GUI/UseTabIcons" ), 1l );
}

void Settings::SetUseTabIcons( bool use )
{
	m_config->Write( _T( "/GUI/UseTabIcons" ), use );
}

int Settings::GetSashPosition( const wxString& window_name )
{
	return m_config->Read( _T( "/GUI/SashPostion/" ) + window_name , 200l );
}

void Settings::SetSashPosition( const wxString& window_name, const int pos )
{
	m_config->Write( _T( "/GUI/SashPostion/" ) + window_name , pos );
}

bool Settings::GetSplitBRoomHorizontally()
{
	return m_config->Read( _T( "/GUI/SplitBRoomHorizontally" ) , 1l );
}

void Settings::SetSplitBRoomHorizontally( const bool vertical )
{
	m_config->Write( _T( "/GUI/SplitBRoomHorizontally" ) , vertical );
}

void Settings::SetStartTab( const int idx )
{
	m_config->Write( _T( "/GUI/StartTab" ) , idx );
}

unsigned int Settings::GetStartTab( )
{
	return m_config->Read( _T( "/GUI/StartTab" ) , MainWindow::PAGE_SINGLE ); //default is SP tab
}

//! simply move saved col size +1 to account for dummy col, force dummy col width to 0
void Settings::TranslateSavedColumWidths()
{
#ifdef SL_DUMMY_COL
	wxString old_path = m_config->GetPath();
	bool old_record = m_config->IsRecordingDefaults( );
	m_config->SetRecordDefaults( false );
	std::vector<wxString> ctrls;
	ctrls.push_back( _T( "NickListCtrl" ) );
	ctrls.push_back( _T( "BattleroomListCtrl" ) );
	ctrls.push_back( _T( "BattleListCtrl" ) );
	ctrls.push_back( _T( "WidgetDownloadListCtrl" ) );
	ctrls.push_back( _T( "ChannelListCtrl" ) );
	ctrls.push_back( _T( "PlaybackListCtrl" ) );
	for ( std::vector<wxString>::const_iterator it = ctrls.begin(); it != ctrls.end(); ++it ) {
		m_config->SetPath( _T( "/GUI/ColumnWidths/" ) + *it );
		unsigned int entries  = m_config->GetNumberOfEntries( false ); //do not recurse
		for ( unsigned int i = entries; i > 0 ; --i )
		{
			m_config->Write( TowxString( i ), m_config->Read( TowxString( i - 1 ) , -1 ) );
		}
		m_config->Write( TowxString( 0 ), 0 );
	}

	m_config->SetPath( old_path );
	m_config->SetRecordDefaults( old_record );
	customMessageBoxNoModal( SL_MAIN_ICON, _( "The way column widths are saved has changed, you may need to re-adjust your col widths manually once." ), _( "Important" ) );
#endif
}

wxString Settings::GetEditorPath( )
{
    #if defined(__WXMSW__)
        wxString def = wxGetOSDirectory() + sepstring + _T("system32") + sepstring + _T("notepad.exe");
        if ( !wxFile::Exists( def ) )
            def = wxEmptyString;
    #else
        wxString def = wxEmptyString;
    #endif
    return m_config->Read( _T( "/GUI/Editor" ) , def );
}

void Settings::SetEditorPath( const wxString& path )
{
    m_config->Write( _T( "/GUI/Editor" ) , path );
}

bool Settings::GetShowXallTabs()
{
    return m_config->Read( _T( "/GUI/CloseOnAll" ) , 0l );
}

void Settings::SetShowXallTabs( bool show )
{
    m_config->Write( _T( "/GUI/CloseOnAll" ) , show );
}

void Settings::SavePerspective( const wxString& notebook_name, const wxString& perspective_name, const wxString& layout_string )
{
	wxString entry = wxFormat( _T( "/GUI/AUI/%s/%s" ) ) % perspective_name % notebook_name;
    m_config->Write( entry, layout_string );
}

wxString Settings::LoadPerspective( const wxString& notebook_name, const wxString& perspective_name )
{
	wxString entry = wxFormat( _T( "/GUI/AUI/%s/%s" ) ) % perspective_name % notebook_name;
    return m_config->Read( entry , _T("") );
}

wxString Settings::GetLastPerspectiveName( )
{
    return m_config->Read( _T( "/GUI/AUI/lastperspective_name" ), _T("default") );
}

void Settings::SetLastPerspectiveName( const wxString&  name )
{
    m_config->Write( _T( "/GUI/AUI/lastperspective_name" ), name );
}

void Settings::SetAutosavePerspective( bool autosave )
{
    m_config->Write( _T( "/GUI/AUI/autosave" ), autosave );
}

bool Settings::GetAutosavePerspective( )
{
    return m_config->Read( _T( "/GUI/AUI/autosave" ), 1l );
}

wxArrayString Settings::GetPerspectives()
{
    wxArrayString list = GetGroupList( _T( "/GUI/AUI" ) );
    wxArrayString ret;
    for ( size_t i = 0; i < list.GetCount(); ++i) {
    	if ( !list[i].EndsWith( BattlePostfix ) )
            ret.Add( list[i] );
        else  {
            wxString stripped = list[i].Left( list[i].Len() - BattlePostfix.Len() );
            if ( !PerspectiveExists( stripped ) )
                ret.Add( stripped );
        }
    }
    return ret;
}

bool Settings::PerspectiveExists( const wxString& perspective_name )
{
    wxArrayString list = GetGroupList( _T( "/GUI/AUI" ) );
    for ( size_t i = 0; i < list.GetCount(); ++i) {
        if ( list[i] == perspective_name )
            return true;
    }
    return false;
}

void Settings::SetAutoloadedChatlogLinesCount( const int count )
{
    m_config->Write( _T( "/GUI/AutoloadedChatlogLinesCount" ), std::abs( count ) );
}

int Settings::GetAutoloadedChatlogLinesCount( )
{
    return m_config->Read( _T( "/GUI/AutoloadedChatlogLinesCount" ), 10l );
}

void Settings::SetUseNotificationPopups( const bool use )
{
	m_config->Write( _T("/GUI/UseNotificationPopups"), use );
}

bool Settings::GetUseNotificationPopups()
{
	return m_config->Read( _T("/GUI/UseNotificationPopups"), true );
}

void Settings::SetNotificationPopupPosition( const size_t index )
{
	m_config->Write( _T("/GUI/NotificationPopupPosition"), (long)index );
}

size_t Settings::GetNotificationPopupPosition()
{
	return m_config->Read( _T("/GUI/NotificationPopupPosition"), (long)ScreenPosition::bottom_right );
}

bool Settings::GetWindowMaximized( const wxString& window )
{
	return m_config->Read(_T( "/GUI/" ) + window + _T( "/maximized" ), 0l );
}

void Settings::GetWindowMaximized( const wxString& window, bool maximized )
{
	m_config->Write(_T( "/GUI/" ) + window + _T( "/maximized" ), maximized );
}

void Settings::SetNotificationPopupDisplayTime( const unsigned int seconds )
{
	m_config->Write( _T("/GUI/NotificationPopupDisplayTime"), (long)seconds );
}

unsigned int Settings::GetNotificationPopupDisplayTime( )
{
	return m_config->Read( _T("/GUI/NotificationPopupDisplayTime"), 5l );
}

bool Settings::DoResetPerspectives()
{
	return m_config->Read(_T( "/reset_perspectives" ) , 0l );
}

void Settings::SetDoResetPerspectives( bool do_it )
{
	m_config->Write(_T( "/reset_perspectives" ) , (long)do_it );
}

bool Settings::GetBroadcastEverywhere()
{
	return m_config->Read( _T("/Chat/BroadcastEverywhere") ,true);
}

void Settings::SetBroadcastEverywhere(bool value)
{
    m_config->Write( _T("/Chat/BroadcastEverywhere"), value);
}

//Hotkeys stuff (for springsettings)
void Settings::SetHotkeyMeta( const wxString& profileName, const wxString& keyStr )
{
	m_config->Write(_T( "/HotkeyProfiles/") + profileName + _T("/Meta"), keyStr);
}

wxString Settings::GetHotkeyMeta( const wxString& profileName )
{
	return m_config->Read(_T( "/HotkeyProfiles/") + profileName + _T("/Meta"), _T("") );
}

void Settings::SetHotkeyKeySymSet( const wxString& profileName, const wxString& setName, const wxString& keyStr )
{
	m_config->Write(_T( "/HotkeyProfiles/") + profileName + _T("/KeySets/") + setName, keyStr);
}

wxString Settings::GetHotkeyKeySymSet( const wxString& profileName, const wxString& setName )
{
	return m_config->Read( _T( "/HotkeyProfiles/") + profileName + _T("/KeySets/") + setName, _T("") );
}

wxArrayString Settings::GetHotkeyKeySymSetNames( const wxString& profileName )
{
	return GetEntryList( _T( "/HotkeyProfiles/" ) + profileName + _T("/KeySets/") );
}

void Settings::SetHotkeyKeySym( const wxString& profileName, const wxString& symName, const wxString& keyStr )
{
	m_config->Write(_T( "/HotkeyProfiles/") + profileName + _T("/KeySyms/") + symName, keyStr);
}

wxString Settings::GetHotkeyKeySym( const wxString& profileName, const wxString& symName )
{
	return m_config->Read( _T( "/HotkeyProfiles/") + profileName + _T("/KeySyms/") + symName, _T("") );
}

wxArrayString Settings::GetHotkeyKeySymNames( const wxString& profileName )
{
	return GetEntryList( _T( "/HotkeyProfiles/" ) + profileName + _T("/KeySyms/") );
}

// oderidx == -1 means unbind
void Settings::SetHotkey( const wxString& profileName, const wxString& command, const wxString& key, int orderIdx )
{
	m_config->Write(_T( "/HotkeyProfiles/") + profileName + _T("/Bindings/") + (wxFormat(wxT("%i") ) % orderIdx) + _T("/") + key, command );
}

wxString Settings::GetHotkey( const wxString& profileName, const wxString& orderIdx, const wxString& key )
{
	return m_config->Read( _T( "/HotkeyProfiles/") + profileName + _T("/Bindings/") + orderIdx + _T("/") + key, _T("") );
}

wxArrayString Settings::GetHotkeyProfiles()
{
	return GetGroupList( _T( "/HotkeyProfiles/" ) );
}

wxArrayString Settings::GetHotkeyProfileOrderIndices( const wxString& profileName )
{
	return GetGroupList( _T( "/HotkeyProfiles/" ) + profileName + _T("/Bindings/") );
}

wxArrayString Settings::GetHotkeyProfileCommandKeys( const wxString& profileName, const wxString& orderIdx )
{
	return GetEntryList( _T( "/HotkeyProfiles/" ) + profileName + _T("/Bindings/") + orderIdx + _T("/") );
}

void Settings::DeleteHotkeyProfiles()
{
	m_config->DeleteGroup( _T( "/HotkeyProfiles/" ) );
}


wxString Settings::GetUikeys( const wxString& index )
{
	return m_config->Read( _T( "/Spring/Paths/" ) + index + _T( "/Uikeys" ), AutoFindUikeys() );
}

//END OF Hotkeys stuff (for springsettings)

bool Settings::IsSelfUpdateDisabled()
{
	return m_config->Read( _T( "/General/SelfUpdateDisabled" ), 0l );
}

std::set<int> Settings::KnownMatchmakerCPU()
{
    const wxArrayString cpus = getFromList(_T("/Hosting/KnownMatchmakerCPU"));
    std::set<int> ret;
    for ( unsigned int i = 0; i < cpus.GetCount(); i++ )
        ret.insert( FromwxString<int>(cpus[i]) );
    return ret;
}

void Settings::AddKnownMatchmakerCPU(int cpu)
{
    std::set<int> current = KnownMatchmakerCPU();
    current.insert(cpu);
    wxArrayString new_cpus;
    for (std::set<int>::const_iterator it = current.begin();
         it != current.end(); ++it) {
        new_cpus.Add( TowxString(*it) );
    }
    setFromList( new_cpus, _T("/Hosting/KnownMatchmakerCPU") );
}
