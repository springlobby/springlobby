/* Copyright (C) 2007, 2008 The SpringLobby Team. All rights reserved. */
//
// Class: Settings
//

#ifdef __WXMSW__
#include <wx/fileconf.h>
#include <wx/msw/registry.h>
#else
#include <wx/config.h>
#endif
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

#include "nonportable.h"
#include "settings.h"
#include "utils/conversion.h"
#include "utils/debug.h"
#include "utils/math.h"
#include "utils/platform.h"
#include "uiutils.h"
#include "battlelistfiltervalues.h"
#include "playback/playbackfiltervalues.h"
#include "globalsmanager.h"
#include "springunitsynclib.h"
#include "customlistctrl.h"
#include "settings++/presets.h"
#include "Helper/sortutil.h"
#include "mainwindow.h"

bool Settings::m_user_defined_config = false;
wxString Settings::m_user_defined_config_path = wxEmptyString;


const wxColor defaultHLcolor (255,0,0);

Settings& sett()
{
    static GlobalObjectHolder<Settings> m_sett;
    return m_sett;
}

SL_WinConf::SL_WinConf(wxFileInputStream& in):
wxFileConfig( in )
{
}

bool SL_WinConf::DoWriteLong(const wxString& key, long lValue)
{
		return wxFileConfig::DoWriteString(key, TowxString<long>( lValue ) );
}

Settings::Settings()
{
  #if defined(__WXMSW__) || defined(__WXMAC__)
  wxString userfilepath = wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + _T("springlobby.conf");
  wxString globalfilepath =  GetExecutableFolder() + wxFileName::GetPathSeparator() + _T("springlobby.conf");

  if ( !wxFileName::FileExists( globalfilepath ) || !wxFileName::IsFileWritable( globalfilepath ) )
  {
     m_chosed_path = userfilepath;
     SetPortableMode( false );
  }
  else
  {
     m_chosed_path = globalfilepath; // portable mode, use only current app paths
     SetPortableMode ( true );
  }

  // if it doesn't exist, try to create it
  if ( !wxFileName::FileExists( m_chosed_path ) )
  {
     // if directory doesn't exist, try to create it
     if ( !IsPortableMode() && !wxFileName::DirExists( wxStandardPaths::Get().GetUserDataDir() ) )
         wxFileName::Mkdir( wxStandardPaths::Get().GetUserDataDir(), 0755 );

     wxFileOutputStream outstream( m_chosed_path );

     if ( !outstream.IsOk() )
     {
         if ( m_user_defined_config ) {
            wxLogError( _T("unable to use specified config file") );
            exit(-1);
         }
     }
  }

  wxFileInputStream instream( m_chosed_path );

  if ( !instream.IsOk() )
  {
      if ( m_user_defined_config ) {
            wxLogError( _T("unable to use specified config file") );
            exit(-1);
         }
  }
	#ifdef __WXMSW__
  m_config = new SL_WinConf( instream );
  #else
  m_config = new wxFileConfig( instream );
  #endif
  #else
  //removed temporarily because it's suspected to cause a bug with userdir creation
 // m_config = new wxConfig( _T("SpringLobby"), wxEmptyString, _T(".springlobby/springlobby.conf"), _T("springlobby.global.conf"), wxCONFIG_USE_LOCAL_FILE | wxCONFIG_USE_GLOBAL_FILE  );
  wxString path = m_user_defined_config ? m_user_defined_config_path : _T(".springlobby/springlobby.conf");
  m_config = new wxConfig( _T("SpringLobby"), wxEmptyString, path, _T("springlobby.global.conf") );
  SetPortableMode ( false );
  #endif
	m_config->SetRecordDefaults( true );
}

Settings::~Settings()
{
}

//! @brief Saves the settings to file
void Settings::SaveSettings()
{
  m_config->Write( _T("/General/firstrun"), false );
  SetCacheVersion();
  SetSettingsVersion();
  m_config->Flush();
  #if defined(__WXMSW__) || defined(__WXMAC__)
  wxFileOutputStream outstream( m_chosed_path );

  if ( !outstream.IsOk() )
  {
      // TODO: error handling
  }

  m_config->Save( outstream );
  #endif
}


#ifdef __WXMSW__
void Settings::SetDefaultConfigs( SL_WinConf& conf )
#else
void Settings::SetDefaultConfigs( wxConfig& conf )
#endif
{
  wxString str;
  long dummy;
	wxString previousgroup;

  // now all groups...
  bool groupcontinue = conf.GetFirstGroup(str, dummy);
  while ( groupcontinue )
  {
  	// climb all tree branches until you hit the most further
		groupcontinue = conf.GetFirstGroup(str, dummy);
    if ( groupcontinue && ( previousgroup != str ) )
    {
			conf.SetPath( str );
			previousgroup = str;
    }
    else
    {
			// enum all entries and add to the config
			wxString currentpath = conf.GetPath();
			bool exist = conf.GetFirstEntry(str, dummy);
			while ( exist )
			{
				if ( !m_config->Exists( currentpath + _T("/") + str ) ) // in theory "main" config should be blank at this point, but better be paranoyd and don't overwrite existing keys...
				{
					m_config->Write( currentpath + _T("/") + str, conf.Read( str, _T("") ) ); // append to main config
				}

				exist = conf.GetNextEntry(str, dummy);
			}

			if ( !currentpath.IsEmpty() )
			{
				wxString todelete = currentpath.AfterLast(_T('/'));
				currentpath = currentpath.BeforeLast(_T('/'));
				conf.SetPath( currentpath ); // go to the parent folder
				conf.DeleteGroup( todelete ); // remove last analyzed group so it doesn't get iterated again
				groupcontinue = true;
			}
			previousgroup = _T("");
    }
  }
  m_config->Flush();
}


wxArrayString Settings::GetGroupList( const wxString& base_key )
{
  wxString old_path = m_config->GetPath();
  m_config->SetPath( base_key );
  wxString groupname;
  long dummy;
  wxArrayString ret;
  bool groupexist = m_config->GetFirstGroup(groupname, dummy);
  while ( groupexist )
  {
		ret.Add( groupname );
    groupexist = m_config->GetNextGroup(groupname, dummy);
  }
  m_config->SetPath( old_path );
  return ret;
}

wxArrayString Settings::GetEntryList( const wxString& base_key )
{
  wxString old_path = m_config->GetPath();
  m_config->SetPath( base_key );
  wxString entryname;
  long dummy;
  wxArrayString ret;
  bool entryexist = m_config->GetFirstEntry(entryname, dummy);
  while ( entryexist )
  {
		ret.Add( entryname );
    entryexist = m_config->GetNextEntry(entryname, dummy);
  }
  m_config->SetPath( old_path );
  return ret;
}

unsigned int Settings::GetGroupCount( const wxString& base_key )
{
		wxString currentpath = m_config->GetPath();
		m_config->SetPath( base_key );
    unsigned int count = m_config->GetNumberOfGroups( false );
    m_config->SetPath( currentpath );
    return count;
}

bool Settings::IsPortableMode()
{
  return m_portable_mode;
}

void Settings::SetPortableMode( bool mode )
{
  m_portable_mode = mode;
}


bool Settings::IsFirstRun()
{
  return m_config->Read( _T("/General/firstrun"), true );
}


void Settings::SetSettingsVersion()
{
   m_config->Write( _T("/General/SettingsVersion"), SETTINGS_VERSION );
}


unsigned int  Settings::GetSettingsVersion()
{
   return m_config->Read( _T("/General/SettingsVersion"), 0l );
}


wxString Settings::GetLobbyWriteDir()
{
  wxString sep = wxFileName::GetPathSeparator();
  wxString path = GetCurrentUsedDataDir() + sep + _T("lobby");
  if ( !wxFileName::DirExists( path ) )
  {
    if ( !wxFileName::Mkdir(  path, 0755  ) ) return wxEmptyString;
  }
  path += sep + _T("SpringLobby") + sep;
  if ( !wxFileName::DirExists( path ) )
  {
    if ( !wxFileName::Mkdir(  path, 0755  ) ) return wxEmptyString;
  }
  return path;
}


bool Settings::UseOldSpringLaunchMethod()
{
    return m_config->Read( _T("/Spring/UseOldLaunchMethod"), 0l );
}

bool Settings::GetNoUDP()
{
    return m_config->Read( _T("/General/NoUDP"), 0l );
}

void Settings::SetNoUDP(bool value)
{
    m_config->Write( _T("/General/NoUDP"), value );
}

int Settings::GetClientPort()
{
    return m_config->Read( _T("/General/ClientPort"), 0l );
}

void Settings::SetClientPort(int value)
{
    m_config->Write( _T("/General/ClientPort"), value );
}

bool Settings::GetShowIPAddresses()
{
    return m_config->Read( _T("/General/ShowIP"), 0l );
}

void Settings::SetShowIPAddresses(bool value)
{
    m_config->Write( _T("/General/ShowIP"), value );
}

void Settings::SetOldSpringLaunchMethod( bool value )
{
    m_config->Write( _T("/Spring/UseOldLaunchMethod"), value );
}


bool Settings::GetWebBrowserUseDefault()
{
  // See note on ambiguities, in wx/confbase.h near line 180.
  bool useDefault;
  m_config->Read(_T("/General/WebBrowserUseDefault"), &useDefault, DEFSETT_WEB_BROWSER_USE_DEFAULT);
  return useDefault;
}

void Settings::SetWebBrowserUseDefault(bool useDefault)
{
  m_config->Write(_T("/General/WebBrowserUseDefault"), useDefault);
}

wxString Settings::GetWebBrowserPath()
{
  return m_config->Read( _T("/General/WebBrowserPath"), wxEmptyString);
}


void Settings::SetWebBrowserPath( const wxString path )
{
    m_config->Write( _T("/General/WebBrowserPath"), path );
}


wxString Settings::GetCachePath()
{
  wxString path = GetLobbyWriteDir() + _T("cache") + wxFileName::GetPathSeparator();
  if ( !wxFileName::DirExists( path ) )
  {
    if ( !wxFileName::Mkdir(  path, 0755  ) ) return wxEmptyString;
  }
  return path;
}


//! @brief sets version number for the cache, needed to nuke it in case it becomes obsolete & incompatible with new versions
void Settings::SetCacheVersion()
{
    m_config->Write( _T("/General/CacheVersion"), CACHE_VERSION );
}


//! @brief returns the cache versioning number, do decide whenever to delete if becomes obsolete & incompatible with new versions
int Settings::GetCacheVersion()
{
    return m_config->Read( _T("/General/CacheVersion"), 0l );
}


void Settings::SetMapCachingThreadProgress( unsigned int index )
{
    m_config->Write( _T("/General/LastMapCachingThreadIndex"), (int)index );
}


unsigned int Settings::GetMapCachingThreadProgress()
{
    return m_config->Read( _T("/General/LastMapCachingThreadIndex"), 0l );
}


void Settings::SetModCachingThreadProgress( unsigned int index )
{
    m_config->Write( _T("/General/LastModCachingThreadIndex"), (int)index );
}


unsigned int Settings::GetModCachingThreadProgress()
{
    return m_config->Read( _T("/General/LastModCachingThreadIndex"), 0l );
}

bool Settings::ShouldAddDefaultServerSettings()
{
		return !m_config->Exists( _T("/Server") );
}

//! @brief Restores default settings
void Settings::SetDefaultServerSettings()
{
    SetServer( WX_STRINGC(DEFSETT_DEFAULT_SERVER_NAME), WX_STRINGC(DEFSETT_DEFAULT_SERVER_HOST), DEFSETT_DEFAULT_SERVER_PORT );
		SetServer( _T("Backup server 1"), _T("springbackup1.servegame.com"), 8200 );
		SetServer( _T("Backup server 2"), _T("springbackup2.servegame.org"), 8200 );
		SetServer( _T("Test server"), _T("taspringmaster.servegame.com"), 8300 );
    SetDefaultServer( WX_STRINGC(DEFSETT_DEFAULT_SERVER_NAME) );
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
		int count = m_config->Read( _T("/Servers/Count"), 0l );
		for ( int i = 0; i < count; i++ )
		{
			wxString server_name = m_config->Read( wxString::Format( _T("/Servers/Server%d"), i ), _T("") );
			if ( server_name == _T("TAS Server") ) server_name = WX_STRINGC( DEFSETT_DEFAULT_SERVER_NAME );
			servers.Add( server_name );
			m_saved_nicks[server_name] = m_config->Read( _T("/Server/")+ server_name +_T("/nick"), _T("") );
			m_saved_pass[server_name] = m_config->Read( _T("/Server/")+ server_name +_T("/pass"), _T("") );
			m_autosave_pass[server_name] = m_config->Read( _T("/Server/")+ server_name +_T("/savepass"), 0l );
			m_saved_ports[server_name] = m_config->Read( _T("/Server/")+ server_name +_T("/port"), DEFSETT_DEFAULT_SERVER_PORT );
			m_saved_hosts[server_name] = m_config->Read( _T("/Server/")+ server_name +_T("/host"), DEFSETT_DEFAULT_SERVER_HOST );
		}
    m_config->DeleteGroup( _T("/Server") );
    m_config->DeleteGroup( _T("/Servers") );
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
    return m_config->Exists( _T("/Server/Servers/")+ server_name );
}


//! @brief Get the name/alias of the default server.
//!
//! @note Normally this will be the previously selected server. But at first run it will be a server that is set as the default.
wxString Settings::GetDefaultServer()
{
    wxString serv = WX_STRINGC(DEFSETT_DEFAULT_SERVER_NAME);
    return m_config->Read( _T("/Server/Default"), serv );
}

void Settings::SetAutoConnect( bool do_autoconnect )
{
    m_config->Write( _T("/Server/Autoconnect"),  do_autoconnect );
}

bool Settings::GetAutoConnect( )
{
    return m_config->Read( _T("/Server/Autoconnect"), 0l );
}


//! @brief Set the name/alias of the default server.
//!
//! @param server_name the server name/alias
//! @see GetDefaultServer()
void   Settings::SetDefaultServer( const wxString& server_name )
{
    m_config->Write( _T("/Server/Default"),  server_name );
}


//! @brief Get hostname of a server.
//!
//! @param server_name the server name/alias
wxString Settings::GetServerHost( const wxString& server_name )
{
    wxString host = WX_STRINGC(DEFSETT_DEFAULT_SERVER_HOST);
    return m_config->Read( _T("/Server/Servers/")+ server_name +_T("/Host"), host );
}


//! @brief Set hostname of a server.
//!
//! @param server_name the server name/alias
//! @param the host url address
//! @param the port where the service is run
void   Settings::SetServer( const wxString& server_name, const wxString& url, int port )
{
    m_config->Write( _T("/Server/Servers/")+ server_name +_T("/Host"), url );
    m_config->Write( _T("/Server/Servers/")+ server_name +_T("/Port"), port );
}

//! @brief Deletes a server from the list.
//!
//! @param server_name the server name/alias
void Settings::DeleteServer( const wxString& server_name )
{
		m_config->DeleteGroup( _T("/Server/Servers/") + server_name );
}


//! @brief Get port number of a server.
//!
//! @param server_name the server name/alias
int    Settings::GetServerPort( const wxString& server_name )
{
    return m_config->Read( _T("/Server/Servers/")+ server_name +_T("/Port"), DEFSETT_DEFAULT_SERVER_PORT );
}

//! @brief Get list of server aliases
wxArrayString Settings::GetServers()
{
    return GetGroupList( _T("/Server/Servers/") );
}


//! @brief Get nickname of the default account for a server.
//!
//! @param server_name the server name/alias
wxString Settings::GetServerAccountNick( const wxString& server_name )
{
    return m_config->Read( _T("/Server/Servers/")+ server_name +_T("/Nick"), _T("") ) ;
}


//! @brief Set nickname of the default account for a server.
//!
//! @param server_name the server name/alias
//! @param value the vaule to be set
void Settings::SetServerAccountNick( const wxString& server_name, const wxString& value )
{
    m_config->Write( _T("/Server/Servers/")+ server_name +_T("/Nick"), value );
}


//! @brief Get password of the default account for a server.
//!
//! @param server_name the server name/alias
//! @todo Implement
wxString Settings::GetServerAccountPass( const wxString& server_name )
{
    return m_config->Read( _T("/Server/Servers/")+ server_name +_T("/Pass"), _T("") );
}


//! @brief Set password of the default account for a server.
//!
//! @param server_name the server name/alias
//! @param value the vaule to be set
//! @todo Implement
void   Settings::SetServerAccountPass( const wxString& server_name, const wxString& value )
{
    m_config->Write( _T("/Server/Servers/")+ server_name +_T("/Pass"), value );
}


//! @brief Get if the password should be saved for a default server account.
//!
//! @param server_name the server name/alias
//! @todo Implement
bool   Settings::GetServerAccountSavePass( const wxString& server_name )
{
    return m_config->Read( _T("/Server/Servers/")+ server_name +_T("/savepass"), (long int)false );
}


//! @brief Set if the password should be saved for a default server account.
//!
//! @param server_name the server name/alias
//! @param value the vaule to be set
//! @todo Implement
void Settings::SetServerAccountSavePass( const wxString& server_name, const bool value )
{
    m_config->Write( _T("/Server/Servers/")+ server_name +_T("/savepass"), (long int)value );
}


int Settings::GetNumChannelsJoin()
{
	return GetGroupCount( _T("/Channels/AutoJoin") );
}

void Settings::AddChannelJoin( const wxString& channel , const wxString& key )
{
    int index = GetNumChannelsJoin();

    m_config->Write( wxString::Format( _T("/Channels/AutoJoin/Channel%d/Name"), index ), channel );
    m_config->Write( wxString::Format( _T("/Channels/AutoJoin/Channel%d/Password"), index ), key );
}


void Settings::RemoveChannelJoin( const wxString& channel )
{
    int index = GetChannelJoinIndex( channel );
    if ( index == -1 ) return;
    int total = GetNumChannelsJoin();
    m_config->DeleteGroup( _T("/Channels/AutoJoin/Channel") + TowxString( index ) );
    m_config->RenameGroup( _T("/Channels/AutoJoin/Channel") + TowxString(total - 1), _T("/Channels/AutoJoin/Channel") + TowxString( index ) );
}

void Settings::RemoveAllChannelsJoin()
{
	m_config->DeleteGroup( _T("/Channels/AutoJoin") );
}


int Settings::GetChannelJoinIndex( const wxString& name )
{
	int numchannels = GetNumChannelsJoin();
	int ret = -1;
	for ( int i = 0; i < numchannels; i++ )
	{
		if ( m_config->Read( wxString::Format( _T("/Channels/AutoJoin/Channel%d/Name"), i ), _T("") ) == name ) ret = i;
	}
	return ret;
}

std::vector<ChannelJoinInfo> Settings::GetChannelsJoin()
{
		std::vector<ChannelJoinInfo> ret;
    int num = GetNumChannelsJoin();
    for ( int i= 0; i < num; i++ )
    {
    	  ChannelJoinInfo info;
    	  info.name = m_config->Read( wxString::Format( _T("/Channels/AutoJoin/Channel%d/Name"), i ), _T("") );
    	  info.password = m_config->Read( wxString::Format( _T("/Channels/AutoJoin/Channel%d/Password"), i ), _T("") );
        ret.push_back( info );
    }
    return ret;
}

void Settings::ConvertOldChannelSettings()
{
	int numchannels = m_config->Read( _T("/Channels/Count"), 0l);
	m_config->DeleteEntry( _T("/Channels/Count") );
	for ( int i = 0; i < numchannels; i++ )
	{
		wxString channelinfo = m_config->Read( _T("/Channels/Channel") + TowxString( i ), _T("") );
		m_config->DeleteEntry( _T("/Channels/Channel") + TowxString( i ) );
		if ( channelinfo.Contains(_T(" ") ) ) AddChannelJoin( channelinfo.BeforeFirst(_T(' ')), channelinfo.AfterLast(_T(' ')) );
		else AddChannelJoin( channelinfo, _T("") );
	}
}

bool Settings::ShouldAddDefaultChannelSettings()
{
		return !m_config->Exists( _T("/Channels" ));
}

/************* SPRINGLOBBY WINDOW POS/SIZE   ******************/
//! @brief Get width of MainWindow.
int Settings::GetWindowWidth( const wxString& window )
{
    return m_config->Read( _T("/GUI/")+ window + _T("/width"), DEFSETT_MW_WIDTH );
}


//! @brief Set width position of MainWindow
void Settings::SetWindowWidth( const wxString& window, const int value )
{
    m_config->Write(
            _T("/GUI/")+ window + _T("/width"),
            clamp(  value,
                    wxSystemSettings::GetMetric( wxSYS_WINDOWMIN_X ),
                    wxSystemSettings::GetMetric( wxSYS_SCREEN_X )
            )
        );
}


//! @brief Get height of MainWindow.
int Settings::GetWindowHeight( const wxString& window )
{
    return m_config->Read( _T("/GUI/")+ window + _T("/height"), DEFSETT_MW_HEIGHT );
}


//! @brief Set height position of MainWindow
void Settings::SetWindowHeight( const wxString& window, const int value )
{
    m_config->Write(
            _T("/GUI/")+ window + _T("/height"),
            clamp(  value,
                    wxSystemSettings::GetMetric( wxSYS_WINDOWMIN_Y ),
                    wxSystemSettings::GetMetric( wxSYS_SCREEN_Y )
            )
        );
}


//! @brief Get top position of MainWindow.
int Settings::GetWindowTop( const wxString& window )
{
    return m_config->Read( _T("/GUI/")+ window + _T("/top"), DEFSETT_MW_TOP );
}


//! @brief Set top position of MainWindow
void Settings::SetWindowTop( const wxString& window, const int value )
{
    m_config->Write(
            _T("/GUI/")+ window + _T("/top"),
            clamp( value,
                    0,
                    wxSystemSettings::GetMetric( wxSYS_SCREEN_Y ) - 20
            )
        );
}


//! @brief Get left position of MainWindow.
int Settings::GetWindowLeft( const wxString& window )
{
    return m_config->Read( _T("/GUI/")+ window + _T("/left"), DEFSETT_MW_LEFT );
}

//! @brief Set left position of MainWindow
void Settings::SetWindowLeft( const wxString& window, const int value )
{
    m_config->Write(
            _T("/GUI/")+ window + _T("/left"),
            clamp( value,
                    0,
                    wxSystemSettings::GetMetric( wxSYS_SCREEN_X ) - 20
            )
        );
}

//some code duplication necessary to be able to simply use wx defaults
wxSize  Settings::GetWindowSize( const wxString& window, const wxSize& def )
{
    wxSize ret = def;
    ret.SetHeight( m_config->Read( _T("/GUI/")+ window + _T("/height"), ret.GetHeight() ) );
    ret.SetWidth( m_config->Read( _T("/GUI/")+ window + _T("/width"), ret.GetWidth() ) );
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
    ret.x = m_config->Read( _T("/GUI/")+ window + _T("/left"), ret.x );
    ret.y = m_config->Read( _T("/GUI/")+ window + _T("/top"), ret.y );
    return ret;
}

void Settings::SetWindowPos( const wxString& window, const wxPoint& pos )
{
    SetWindowLeft( window, pos.x );
    SetWindowTop( window, pos.y );
}

// ========================================================

wxPathList Settings::GetAdditionalSearchPaths( wxPathList& pl )
{
	wxPathList ret;
	wxChar sep = wxFileName::GetPathSeparator();
	wxStandardPathsBase& sp = wxStandardPathsBase::Get();

  pl.Add( wxFileName::GetCwd() );
  pl.Add( sp.GetExecutablePath() );
  pl.Add( wxFileName::GetCwd() );
  pl.Add( sp.GetExecutablePath() );
  pl.Add( wxFileName::GetHomeDir() );
  pl.Add( sp.GetUserDataDir().BeforeLast( sep ) );
  pl.Add( sp.GetDataDir().BeforeLast( sep ) );
  pl.Add( sp.GetResourcesDir().BeforeLast( sep ) );

	pl.Add( wxGetOSDirectory() );

  for ( size_t i = 0; i < pl.GetCount(); i++ )
  {
    wxString path = pl[i];
    if ( path.Last() != sep ) path += sep;
    ret.Add( path );
    ret.Add( path + _T("Spring") + sep );
    ret.Add( path + _T("spring") + sep );
    ret.Add( path + _T("games") + sep + _T("Spring") + sep );
    ret.Add( path + _T("games") + sep + _T("spring") + sep );
  }
  return ret;
}

wxString Settings::AutoFindSpringBin()
{
  wxPathList pl;

  pl.AddEnvList( _T("%ProgramFiles%") );
  pl.AddEnvList( _T("PATH") );

  pl = GetAdditionalSearchPaths( pl );

  return pl.FindValidPath( SPRING_BIN );
}


wxString Settings::AutoFindUnitSync()
{
    wxPathList pl;

    pl.AddEnvList( _T("%ProgramFiles%") );

    pl.AddEnvList( _T("LDPATH") );
    pl.AddEnvList( _T("LD_LIBRARY_PATH") );

    pl.Add( _T("/usr/local/lib64") );
    pl.Add( _T("/usr/local/games") );
    pl.Add( _T("/usr/local/games/lib") );
    pl.Add( _T("/usr/local/lib") );
    pl.Add( _T("/usr/lib64") );
    pl.Add( _T("/usr/lib") );
    pl.Add( _T("/usr/games") );
    pl.Add( _T("/usr/games/lib64") );
    pl.Add( _T("/usr/games/lib") );

	pl = GetAdditionalSearchPaths( pl );

	wxString retpath = pl.FindValidPath( _T("unitsync") + GetLibExtension() );
	if ( retpath.IsEmpty() )
        retpath = pl.FindValidPath( _T("libunitsync") + GetLibExtension() );
	return retpath;
}


void Settings::ConvertOldSpringDirsOptions()
{
  SetUnitSync( _T("default"), m_config->Read( _T("/Spring/unitsync_loc"), _T("") ) );
  SetSpringBinary( _T("default"), m_config->Read( _T("/Spring/exec_loc"), _T("") ) );

  SetUsedSpringIndex( _T("default") );

  m_config->DeleteEntry( _T("/Spring/unitsync_loc") );
  m_config->DeleteEntry( _T("/Spring/use_spring_def_loc") );
  m_config->DeleteEntry( _T("/Spring/use_unitsync_def_loc") );
  m_config->DeleteEntry( _T("/Spring/dir") );
  m_config->DeleteEntry( _T("/Spring/exec_loc") );
}

std::map<wxString, wxString> Settings::GetSpringVersionList()
{
	return m_spring_versions;
}

void Settings::RefreshSpringVersionList()
{
  wxLogDebugFunc(_T(""));
  wxArrayString list = GetGroupList( _T("/Spring/Paths") );
  int count = list.GetCount();
  std::map<wxString, wxString> usync_paths;
  for ( int i = 0; i < count; i++ )
  {
  	wxString groupname = list[i];
    usync_paths[groupname] = m_config->Read( _T("/Spring/Paths/") + groupname + _T("/UnitSyncPath"), _T("") );
  }
  m_spring_versions = susynclib().GetSpringVersionList(usync_paths);
}

wxString Settings::GetCurrentUsedSpringIndex()
{
  return m_config->Read( _T("/Spring/CurrentIndex"), _T("default") );
}

void Settings::SetUsedSpringIndex( const wxString& index )
{
  m_config->Write( _T("/Spring/CurrentIndex"), index );
}

bool Settings::GetSearchSpringOnlyInSLPath()
{
	bool defaultval = false;
	#ifdef __WXMSW__
	defaultval = true;
	#endif
	return m_config->Read( _T("/Spring/SearchSpringOnlyInSLPath"), defaultval );
}

void Settings::SetSearchSpringOnlyInSLPath( bool value )
{
	m_config->Write( _T("/Spring/SearchSpringOnlyInSLPath"), value );
}

void Settings::DeleteSpringVersionbyIndex( const wxString& index )
{
  m_config->DeleteGroup( _T("/Spring/Path/") + index );
  if( GetCurrentUsedSpringIndex() == index ) SetUsedSpringIndex( _T("") );
}


wxString Settings::GetCurrentUsedDataDir()
{
  wxString dir;
  if ( susynclib().IsLoaded() )
  {
    if ( susynclib().VersionSupports( IUnitSync::USYNC_GetDataDir ) ) dir = susynclib().GetSpringDataDir();
    else dir = susynclib().GetSpringConfigString( _T("SpringData"), _T("") );
  }
  #ifdef __WXMSW__
  if ( dir.IsEmpty() ) dir = GetExecutableFolder(); // fallback
  #else
  if ( dir.IsEmpty() ) dir = wxFileName::GetHomeDir() + wxFileName::GetPathSeparator() + _T(".spring"); // fallback
  #endif
  return dir;
}


wxString Settings::GetCurrentUsedSpringBinary()
{
		if ( IsPortableMode() ) return GetCurrentUsedDataDir() + wxFileName::GetPathSeparator() + _T("spring.exe");
		#ifdef __WXMSW__
		else if ( GetSearchSpringOnlyInSLPath() ) return GetExecutableFolder() + wxFileName::GetPathSeparator() + _T("spring.exe");
		#endif
    else return GetSpringBinary( GetCurrentUsedSpringIndex() );
}


wxString Settings::GetCurrentUsedUnitSync()
{
		if ( IsPortableMode() ) return GetCurrentUsedDataDir() + wxFileName::GetPathSeparator() + _T("unitsync") + GetLibExtension();
		#ifdef __WXMSW__
		else if ( GetSearchSpringOnlyInSLPath() ) return GetExecutableFolder() + wxFileName::GetPathSeparator() + _T("unitsync") + GetLibExtension();
		#endif
    else return GetUnitSync( GetCurrentUsedSpringIndex() );
}

wxString Settings::GetCurrentUsedSpringConfigFilePath()
{
	wxString path;
	try
	{
    path = susynclib().GetConfigFilePath();
	}
	catch ( unitsync_assert ) {}
	return path;
}

wxString Settings::GetUnitSync( const wxString& index )
{
  return m_config->Read( _T("/Spring/Paths/") + index + _T("/UnitSyncPath"), AutoFindUnitSync() );
}


wxString Settings::GetSpringBinary( const wxString& index )
{
    return m_config->Read( _T("/Spring/Paths/") + index + _T("/SpringBinPath"), AutoFindSpringBin() );
}

void Settings::SetUnitSync( const wxString& index, const wxString& path )
{
  m_config->Write( _T("/Spring/Paths/") + index + _T("/UnitSyncPath"), path );
}

void Settings::SetSpringBinary( const wxString& index, const wxString& path )
{
  m_config->Write( _T("/Spring/Paths/") + index + _T("/SpringBinPath"), path );
}

wxString Settings::GetForcedSpringConfigFilePath()
{
	if ( IsPortableMode() ) return GetCurrentUsedDataDir() + wxFileName::GetPathSeparator() + _T("springsettings.cfg");
	else return _T("");
}

// ===================================================

bool Settings::GetChatLogEnable()
{
    if (!m_config->Exists(_T("/ChatLog/chatlog_enable"))) SetChatLogEnable( true );
    return m_config->Read( _T("/ChatLog/chatlog_enable"), true );
}


void Settings::SetChatLogEnable( const bool value )
{
    m_config->Write( _T("/ChatLog/chatlog_enable"), value );
}


wxString Settings::GetChatLogLoc()
{
    wxString path = GetLobbyWriteDir() + _T("chatlog");
    if ( !wxFileName::DirExists( path ) )
    {
      if ( !wxFileName::Mkdir(  path, 0755  ) ) return wxEmptyString;
    }
    return path;
}


wxString Settings::GetLastHostDescription()
{
    return m_config->Read( _T("/Hosting/LastDescription"), _T("") );
}


wxString Settings::GetLastHostMod()
{
    return m_config->Read( _T("/Hosting/LastMod"), _T("") );
}


wxString Settings::GetLastHostPassword()
{
    return m_config->Read( _T("/Hosting/LastPassword"), _T("") );
}


int Settings::GetLastHostPort()
{
    return m_config->Read( _T("/Hosting/LastPort"), DEFSETT_SPRING_PORT );
}


int Settings::GetLastHostPlayerNum()
{
    return m_config->Read( _T("/Hosting/LastPlayerNum"), 4 );
}


int Settings::GetLastHostNATSetting()
{
    return m_config->Read( _T("/Hosting/LastNATSetting"), (long)0 );
}


wxString Settings::GetLastHostMap()
{
    return m_config->Read( _T("/Hosting/LastMap"), _T("") );
}

int Settings::GetLastRankLimit()
{
    return m_config->Read( _T("/Hosting/LastRank"), 0l );
}

bool Settings::GetTestHostPort()
{
    return m_config->Read( _T("/Hosting/TestHostPort"), 0l );
}

bool Settings::GetLastAutolockStatus()
{
    return m_config->Read( _T("/Hosting/LastAutoLock"), true );
}

bool Settings::GetLastHostRelayedMode()
{
    return m_config->Read( _T("/Hosting/LastRelayedMode"), 0l );
}

wxColour Settings::GetBattleLastColour()
{
   return  wxColor( m_config->Read( _T("/Hosting/MyLastColour"), _T("#FFFF00") ) );
}


void Settings::SetBattleLastColour( const wxColour& col )
{
  m_config->Write( _T("/Hosting/MyLastColour"), col.GetAsString( wxC2S_HTML_SYNTAX ) );
}

void Settings::SetLastHostDescription( const wxString& value )
{
    m_config->Write( _T("/Hosting/LastDescription"), value );
}


void Settings::SetLastHostMod( const wxString& value )
{
    m_config->Write( _T("/Hosting/LastMod"), value );
}


void Settings::SetLastHostPassword( const wxString& value )
{
    m_config->Write( _T("/Hosting/LastPassword"), value );
}


void Settings::SetLastHostPort( int value )
{
    m_config->Write( _T("/Hosting/LastPort"), value );
}


void Settings::SetLastHostPlayerNum( int value )
{
    m_config->Write( _T("/Hosting/LastPlayerNum"), value );
}


void Settings::SetLastHostNATSetting( int value )
{
    m_config->Write( _T("/Hosting/LastNATSetting"), value );
}


void Settings::SetLastHostMap( const wxString& value )
{
    m_config->Write( _T("/Hosting/LastMap"), value );
}

void Settings::SetLastRankLimit( int rank )
{
    m_config->Write( _T("/Hosting/LastRank"), rank );
}

void Settings::SetLastAI( const wxString& ai )
{
    m_config->Write( _T("/SinglePlayer/LastAI"), ai );
}

void Settings::SetTestHostPort( bool value )
{
    m_config->Write( _T("/Hosting/TestHostPort"), value );
}

void Settings::SetLastAutolockStatus( bool value )
{
    m_config->Write( _T("/Hosting/LastAutoLock"), value );
}

void Settings::SetLastHostRelayedMode( bool value )
{
    m_config->Write( _T("/Hosting/LastRelayedMode"), value );
}

void Settings::SetHostingPreset( const wxString& name, int optiontype, std::map<wxString,wxString> options )
{
    m_config->DeleteGroup( _T("/Hosting/Preset/") + name + _T("/") + TowxString( optiontype ) );
    for (std::map<wxString,wxString>::iterator it = options.begin(); it != options.end(); ++it)
    {
        m_config->Write( _T("/Hosting/Preset/") + name + _T("/") + TowxString( optiontype ) + _T("/") + it->first , it->second );
    }
}

std::map<wxString,wxString> Settings::GetHostingPreset( const wxString& name, int optiontype )
{
    wxString path_base = _T("/Hosting/Preset/") + name + _T("/") + TowxString( optiontype );
    std::map<wxString,wxString> ret;
    wxArrayString list = GetEntryList( path_base );

    wxString old_path = m_config->GetPath();
    m_config->SetPath( path_base );

    int count = list.GetCount();
    for( int i = 0; i < count; i ++ )
    {
        wxString keyname = list[i];
        wxString val = m_config->Read( keyname );
        ret[keyname] = val;
    }

    m_config->SetPath( old_path );

    return ret;
}


wxArrayString Settings::GetPresetList()
{
  return GetGroupList( _T("/Hosting/Preset") );
}


void Settings::DeletePreset( const wxString& name )
{
  m_config->DeleteGroup( _T("/Hosting/Preset/") + name );

  //delete mod default preset associated
  wxArrayString list = GetEntryList( _T("/Hosting/ModDefaultPreset") );
  int count = list.GetCount();
  for( int i = 0; i < count; i ++ )
  {
  	wxString keyname = list[i];
    if ( m_config->Read( keyname ) == name ) m_config->DeleteEntry( keyname );
  }
}


wxString Settings::GetModDefaultPresetName( const wxString& modname )
{
  return m_config->Read( _T("/Hosting/ModDefaultPreset/") + modname );
}


void Settings::SetModDefaultPresetName( const wxString& modname, const wxString& presetname )
{
  m_config->Write( _T("/Hosting/ModDefaultPreset/") + modname, presetname );
}


void Settings::SetBalanceMethod(int value)
{
    m_config->Write( _T("/Hosting/BalanceMethod"), value );
}
int Settings::GetBalanceMethod()
{
    return m_config->Read( _T("/Hosting/BalanceMethod"), 1l);
}

void Settings::SetBalanceClans(bool value)
{
    m_config->Write( _T("/Hosting/BalanceClans"), value );
}
bool Settings::GetBalanceClans()
{
    return m_config->Read( _T("/Hosting/BalanceClans"), true);
}

void Settings::SetBalanceStrongClans(bool value)
{
    m_config->Write( _T("/Hosting/BalanceStrongClans"), value );
}

bool Settings::GetBalanceStrongClans()
{
    return m_config->Read( _T("/Hosting/BalanceStrongClans"), 0l );
}

void Settings::SetBalanceGrouping( int value )
{
    m_config->Write( _T("/Hosting/BalanceGroupingSize"), value );
}

int Settings::GetBalanceGrouping()
{
    return m_config->Read( _T("/Hosting/BalanceGroupingSize"), 0l );
}


void Settings::SetFixIDMethod(int value)
{
    m_config->Write( _T("/Hosting/FixIDMethod"), value );
}
int Settings::GetFixIDMethod()
{
    return m_config->Read( _T("/Hosting/FixIDMethod"), 1l);
}

void Settings::SetFixIDClans(bool value)
{
    m_config->Write( _T("/Hosting/FixIDClans"), value );
}
bool Settings::GetFixIDClans()
{
    return m_config->Read( _T("/Hosting/FixIDClans"), true);
}

void Settings::SetFixIDStrongClans(bool value)
{
    m_config->Write( _T("/Hosting/FixIDStrongClans"), value );
}

bool Settings::GetFixIDStrongClans()
{
    return m_config->Read( _T("/Hosting/FixIDStrongClans"), 0l );
}

void Settings::SetFixIDGrouping( int value )
{
    m_config->Write( _T("/Hosting/FixIDGroupingSize"), value );
}

int Settings::GetFixIDGrouping()
{
    return m_config->Read( _T("/Hosting/FixIDGroupingSize"), 0l );
}


wxString Settings::GetLastAI()
{
    return m_config->Read( _T("/SinglePlayer/LastAI"), wxEmptyString );
}

void Settings::SetDisplayJoinLeave( bool display, const wxString& channel  )
{
    m_config->Write( _T("/Channels/DisplayJoinLeave/")  + channel, display);
}

bool Settings::GetDisplayJoinLeave( const wxString& channel  )
{
    return m_config->Read( _T("/Channels/DisplayJoinLeave/") +  channel, true);
}


void Settings::SetChatHistoryLenght( int historylines )
{
    m_config->Write( _T("/Chat/HistoryLinesLenght/"), historylines);
}


int Settings::GetChatHistoryLenght()
{
    return m_config->Read( _T("/Chat/HistoryLinesLenght/"), 1000);
}


void Settings::SetChatPMSoundNotificationEnabled( bool enabled )
{
  m_config->Write( _T("/Chat/PMSound"), enabled);
}


bool Settings::GetChatPMSoundNotificationEnabled()
{
  return m_config->Read( _T("/Chat/PMSound"), 1l);
}

wxColor ConvertOldRGBFormat( wxString color )
{
	long R = 0, G = 0, B = 0;
	color.BeforeFirst( _T(' ') ).ToLong( &R );
	color = color.AfterFirst( _T(' ') );
	color.BeforeFirst( _T(' ') ).ToLong( &G );
	color = color.AfterFirst( _T(' ') );
	color.BeforeFirst( _T(' ') ).ToLong( &B );
	return wxColor( R % 256, G % 256, B % 256 );
}

void Settings::ConvertOldColorSettings()
{
	SetChatColorNormal( ConvertOldRGBFormat( m_config->Read( _T("/Chat/Colour/Normal"), _T("0 0 0") ) ) );
	SetChatColorBackground( ConvertOldRGBFormat( m_config->Read( _T("/Chat/Colour/Background"), _T("255 255 255") ) ) );
	SetChatColorHighlight( ConvertOldRGBFormat( m_config->Read( _T("/Chat/Colour/Highlight"), _T("255 0 0") ) ) );
	SetChatColorMine( ConvertOldRGBFormat( m_config->Read( _T("/Chat/Colour/Mine"), _T("138 138 138") ) ) );
	SetChatColorNotification( ConvertOldRGBFormat( m_config->Read( _T("/Chat/Colour/Notification"), _T("255 40 40") ) ) );
	SetChatColorServer( ConvertOldRGBFormat( m_config->Read( _T("/Chat/Colour/Server"), _T("0 80 128") ) ) );
	SetChatColorClient( ConvertOldRGBFormat( m_config->Read( _T("/Chat/Colour/Client"), _T("20 200 25") ) ) );
	SetChatColorJoinPart( ConvertOldRGBFormat( m_config->Read( _T("/Chat/Colour/JoinPart"), _T("66 204 66") ) ) );
	SetChatColorError( ConvertOldRGBFormat( m_config->Read( _T("/Chat/Colour/Error"), _T("128 0 0") ) ) );
	SetChatColorTime( ConvertOldRGBFormat( m_config->Read( _T("/Chat/Colour/Time"), _T("100 100 140") ) ) );
	SetChatColorAction( ConvertOldRGBFormat( m_config->Read( _T("/Chat/Colour/Action"), _T("230 0 255") ) ) );
	SetBattleLastColour( ConvertOldRGBFormat( m_config->Read( _T("/Hosting/MyLastColour"), _T("255 255 0") ) ) );

	//convert custom color palette, note 16 colors is wx limit
	wxArrayString palettes = GetGroupList( _T("/CustomColors") );
	for ( unsigned int j = 0; j < palettes.GetCount(); j++ )
	{
		wxString paletteName = palettes[j];
		for ( int i = 0; i < 16; ++i)
		{
				wxColor col( ConvertOldRGBFormat( m_config->Read( _T("/CustomColors/") + paletteName + _T("/") + TowxString(i), _T("255 255 255") ) ) );
				m_config->Write( _T("/CustomColors/") + paletteName + _T("/") + TowxString(i), col.GetAsString( wxC2S_HTML_SYNTAX ) );
		}
	}

	wxArrayString groups = GetGroupList( _T("/Groups") );
	for ( unsigned int j = 0; j < groups.GetCount(); j++ )
	{
		wxString group = groups[j];
		wxColour col( ConvertOldRGBFormat ( m_config->Read( _T("/Groups/") + group + _T("/Opts/HLColor") , _T( "100 100 140" ) ) ) );
		m_config->Write( _T("/Groups/") + group + _T("/Opts/HLColor"), col.GetAsString( wxC2S_HTML_SYNTAX ) );
	}

}

wxColour Settings::GetChatColorNormal()
{
    return wxColour( m_config->Read( _T("/Chat/Colour/Normal"), _T( "#000000" ) ) );
}

void Settings::SetChatColorNormal( wxColour value )
{
    m_config->Write( _T("/Chat/Colour/Normal"), value.GetAsString( wxC2S_CSS_SYNTAX ) );
}


wxColour Settings::GetChatColorBackground()
{
    return wxColour( m_config->Read( _T("/Chat/Colour/Background"), _T( "#FFFFFF" ) ) );
}

void Settings::SetChatColorBackground( wxColour value )
{
    m_config->Write( _T("/Chat/Colour/Background"), value.GetAsString( wxC2S_CSS_SYNTAX ) );
}

wxColour Settings::GetChatColorHighlight()
{
    return wxColour( m_config->Read( _T("/Chat/Colour/Highlight"), _T( "#FF0000" ) ) );
}

void Settings::SetChatColorHighlight( wxColour value )
{
    m_config->Write( _T("/Chat/Colour/Highlight"), value.GetAsString( wxC2S_CSS_SYNTAX ) );
}

wxColour Settings::GetChatColorMine()
{
    return wxColour( m_config->Read( _T("/Chat/Colour/Mine"), _T( "#8A8A8A" ) ) );
}

void Settings::SetChatColorMine( wxColour value )
{
    m_config->Write( _T("/Chat/Colour/Mine"), value.GetAsString( wxC2S_CSS_SYNTAX ) );
}

wxColour Settings::GetChatColorNotification()
{
    return wxColour( m_config->Read( _T("/Chat/Colour/Notification"), _T( "#FF2828" ) ) );
}

void Settings::SetChatColorNotification( wxColour value )
{
    m_config->Write( _T("/Chat/Colour/Notification"), value.GetAsString( wxC2S_CSS_SYNTAX ) );
}

wxColour Settings::GetChatColorAction()
{
    return wxColour( m_config->Read( _T("/Chat/Colour/Action"), _T( "#E600FF" ) ) );
}

void Settings::SetChatColorAction( wxColour value )
{
    m_config->Write( _T("/Chat/Colour/Action"), value.GetAsString( wxC2S_CSS_SYNTAX ) );
}

wxColour Settings::GetChatColorServer()
{
    return wxColour( m_config->Read( _T("/Chat/Colour/Server"), _T( "#005080" ) ) );
}

void Settings::SetChatColorServer( wxColour value )
{
    m_config->Write( _T("/Chat/Colour/Server"), value.GetAsString( wxC2S_CSS_SYNTAX ) );
}

wxColour Settings::GetChatColorClient()
{
    return wxColour( m_config->Read( _T("/Chat/Colour/Client"), _T( "#14C819" ) ) );
}

void Settings::SetChatColorClient( wxColour value )
{
    m_config->Write( _T("/Chat/Colour/Client"), value.GetAsString( wxC2S_CSS_SYNTAX ) );
}

wxColour Settings::GetChatColorJoinPart()
{
    return wxColour( m_config->Read( _T("/Chat/Colour/JoinPart"), _T( "#42CC42" ) ) );
}

void Settings::SetChatColorJoinPart( wxColour value )
{
    m_config->Write( _T("/Chat/Colour/JoinPart"), value.GetAsString( wxC2S_CSS_SYNTAX ) );
}

wxColour Settings::GetChatColorError()
{
    return wxColour( m_config->Read( _T("/Chat/Colour/Error"), _T( "#800000" ) ) );
}

void Settings::SetChatColorError( wxColour value )
{
    m_config->Write( _T("/Chat/Colour/Error"), value.GetAsString( wxC2S_CSS_SYNTAX ) );
}

wxColour Settings::GetChatColorTime()
{
    return wxColour( m_config->Read( _T("/Chat/Colour/Time"), _T( "#64648C" ) ) );
}

void Settings::SetChatColorTime( wxColour value )
{
    m_config->Write( _T("/Chat/Colour/Time"), value.GetAsString( wxC2S_CSS_SYNTAX ) );
}

wxFont Settings::GetChatFont()
{
    wxString info = m_config->Read( _T("/Chat/Font"), wxEmptyString );
    if (info != wxEmptyString) {
        wxFont f;
        f.SetNativeFontInfo( info );
        return f;
    }
    else {
        wxFont f(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
        return f;
    }
}

void Settings::SetChatFont( wxFont value )
{
    m_config->Write( _T("/Chat/Font"), value.GetNativeFontInfoDesc() );
}


bool Settings::GetSmartScrollEnabled()
{
    return m_config->Read( _T("/Chat/SmartScrollEnabled"), true);
}

void Settings::SetSmartScrollEnabled(bool value){
  m_config->Write( _T("/Chat/SmartScrollEnabled"), value);
}

bool Settings::GetAlwaysAutoScrollOnFocusLost()
{
  return m_config->Read( _T("/Chat/AlwaysAutoScrollOnFocusLost"), true );
}

void Settings::SetAlwaysAutoScrollOnFocusLost(bool value)
{
  m_config->Write( _T("/Chat/AlwaysAutoScrollOnFocusLost"), value);
}

void Settings::ConvertOldHiglightSettings()
{
	SetHighlightedWords( wxStringTokenize( m_config->Read( _T("/Chat/HighlightedWords"), _T("") ), _T(";") ) );
}

void Settings::SetUseIrcColors( bool value )
{
	m_config->Write( _T("/Chat/UseIrcColors"), value);
}

bool Settings::GetUseIrcColors()
{
	return m_config->Read( _T("/Chat/UseIrcColors"), true );
}


void Settings::SetHighlightedWords( const wxArrayString& words )
{
	if(m_config->Exists( _T("/Chat/HighlightedWords"))) // flush existing entries
		m_config->DeleteGroup(_T("/Chat/HighlightedWords"));

	for ( unsigned int i = 0; i < words.GetCount(); i++ )
	{
		m_config->Write( _T("/Chat/HighlightedWords/") + words[i], words[i] );
	}
}

wxArrayString Settings::GetHighlightedWords()
{
	return GetEntryList( _T("/Chat/HighlightedWords") );
}

void Settings::SetRequestAttOnHighlight( const bool req )
{
  m_config->Write( _T("/Chat/ReqAttOnHighlight"), req);
}

bool Settings::GetRequestAttOnHighlight( )
{
  return m_config->Read( _T("/Chat/ReqAttOnHighlight"), 0l);
}

BattleListFilterValues Settings::GetBattleFilterValues(const wxString& profile_name)
{
    BattleListFilterValues filtervalues;
    filtervalues.description =      m_config->Read( _T("/BattleFilter/")+profile_name + _T("/description"), _T("") );
    filtervalues.host =             m_config->Read( _T("/BattleFilter/")+profile_name + _T("/host"), _T("") );
    filtervalues.map=               m_config->Read( _T("/BattleFilter/")+profile_name + _T("/map"), _T("") );
    filtervalues.map_show =         m_config->Read( _T("/BattleFilter/")+profile_name + _T("/map_show"), 0L );
    filtervalues.maxplayer =        m_config->Read( _T("/BattleFilter/")+profile_name + _T("/maxplayer"), _T("All") );
    filtervalues.maxplayer_mode =   m_config->Read( _T("/BattleFilter/")+profile_name + _T("/maxplayer_mode"), _T("=") );
    filtervalues.mod =              m_config->Read( _T("/BattleFilter/")+profile_name + _T("/mod"), _T("") );
    filtervalues.mod_show =         m_config->Read( _T("/BattleFilter/")+profile_name + _T("/mod_show"), 0L );
    filtervalues.player_mode =      m_config->Read( _T("/BattleFilter/")+profile_name + _T("/player_mode"), _T("=") );
    filtervalues.player_num  =      m_config->Read( _T("/BattleFilter/")+profile_name + _T("/player_num"), _T("All") );
    filtervalues.rank =             m_config->Read( _T("/BattleFilter/")+profile_name + _T("/rank"), _T("All") );
    filtervalues.rank_mode =        m_config->Read( _T("/BattleFilter/")+profile_name + _T("/rank_mode"), _T("<") );
    filtervalues.spectator =        m_config->Read( _T("/BattleFilter/")+profile_name + _T("/spectator"), _T("All") );
    filtervalues.spectator_mode =   m_config->Read( _T("/BattleFilter/")+profile_name + _T("/spectator_mode"), _T("=") );
    filtervalues.status_full =      m_config->Read( _T("/BattleFilter/")+profile_name + _T("/status_full"), true );
    filtervalues.status_locked =    m_config->Read( _T("/BattleFilter/")+profile_name + _T("/status_locked"),true );
    filtervalues.status_open =      m_config->Read( _T("/BattleFilter/")+profile_name + _T("/status_open"), true );
    filtervalues.status_passworded= m_config->Read( _T("/BattleFilter/")+profile_name + _T("/status_passworded"), true );
    filtervalues.status_start =     m_config->Read( _T("/BattleFilter/")+profile_name + _T("/status_start"), true );
    filtervalues.highlighted_only = m_config->Read( _T("/BattleFilter/")+profile_name + _T("/highlighted_only"), 0l);
    return filtervalues;
}

void Settings::SetBattleFilterValues(const BattleListFilterValues& filtervalues, const wxString& profile_name)
{
    m_config->Write( _T("/BattleFilter/")+profile_name + _T("/description"),filtervalues.description);
    m_config->Write( _T("/BattleFilter/")+profile_name + _T("/host"),filtervalues.host );
    m_config->Write( _T("/BattleFilter/")+profile_name + _T("/map"),filtervalues.map );
    m_config->Write( _T("/BattleFilter/")+profile_name + _T("/map_show"),filtervalues.map_show );
    m_config->Write( _T("/BattleFilter/")+profile_name + _T("/maxplayer"),filtervalues.maxplayer );
    m_config->Write( _T("/BattleFilter/")+profile_name + _T("/maxplayer_mode"),filtervalues.maxplayer_mode);
    m_config->Write( _T("/BattleFilter/")+profile_name + _T("/mod"),filtervalues.mod );
    m_config->Write( _T("/BattleFilter/")+profile_name + _T("/mod_show"),filtervalues.mod_show );
    m_config->Write( _T("/BattleFilter/")+profile_name + _T("/player_mode"),filtervalues.player_mode );
    m_config->Write( _T("/BattleFilter/")+profile_name + _T("/player_num"),filtervalues.player_num );
    m_config->Write( _T("/BattleFilter/")+profile_name + _T("/rank"),filtervalues.rank );
    m_config->Write( _T("/BattleFilter/")+profile_name + _T("/rank_mode"),filtervalues.rank_mode );
    m_config->Write( _T("/BattleFilter/")+profile_name + _T("/spectator"),filtervalues.spectator );
    m_config->Write( _T("/BattleFilter/")+profile_name + _T("/spectator_mode"),filtervalues.spectator_mode );
    m_config->Write( _T("/BattleFilter/")+profile_name + _T("/status_full"),filtervalues.status_full );
    m_config->Write( _T("/BattleFilter/")+profile_name + _T("/status_locked"),filtervalues.status_locked );
    m_config->Write( _T("/BattleFilter/")+profile_name + _T("/status_open"),filtervalues.status_open );
    m_config->Write( _T("/BattleFilter/")+profile_name + _T("/status_passworded"),filtervalues.status_passworded );
    m_config->Write( _T("/BattleFilter/")+profile_name + _T("/status_start"),filtervalues.status_start );
    m_config->Write( _T("/BattleFilter/")+profile_name + _T("/highlighted_only"),filtervalues.highlighted_only );
    m_config->Write( _T("/BattleFilter/lastprofile"),profile_name);
}

bool Settings::GetBattleFilterActivState() const
{
    return m_config->Read( _T("/BattleFilter/Active") , 0l );
}

void Settings::SetBattleFilterActivState(const bool state)
{
    m_config->Write( _T("/BattleFilter/Active") , state );
}

void Settings::SetMapLastStartPosType( const wxString& mapname, const wxString& startpostype )
{
		m_config->Write( _T("/Hosting/MapLastValues/") + mapname + _T("/startpostype"), startpostype );
}

void Settings::SetMapLastRectPreset( const wxString& mapname, std::vector<Settings::SettStartBox> rects )
{
	wxString basepath = _T("/Hosting/MapLastValues/") + mapname + _T("/Rects");
	m_config->DeleteGroup( basepath );
	for ( std::vector<Settings::SettStartBox>::iterator itor = rects.begin(); itor != rects.end(); itor++ )
	{
		SettStartBox box = *itor;
		wxString additionalpath = basepath + _T("/Rect") + TowxString(box.ally) + _T("/");
		m_config->Write( additionalpath + _T("TopLeftX"), box.topx );
		m_config->Write( additionalpath + _T("TopLeftY"), box.topy );
		m_config->Write( additionalpath + _T("BottomRightX"), box.bottomx );
		m_config->Write( additionalpath + _T("BottomRightY"), box.bottomy );
		m_config->Write( additionalpath + _T("AllyTeam"), box.ally );
	}
}

wxString Settings::GetMapLastStartPosType( const wxString& mapname )
{
	return m_config->Read( _T("/Hosting/MapLastValues/") + mapname + _T("/startpostype"), _T("") );
}

std::vector<Settings::SettStartBox> Settings::GetMapLastRectPreset( const wxString& mapname )
{
	wxString basepath = _T("/Hosting/MapLastValues/") + mapname + _T("/Rects");
	wxArrayString boxes = GetGroupList( basepath );
	std::vector<Settings::SettStartBox> ret;
	for ( unsigned int i = 0; i < boxes.GetCount(); i++ )
	{
		wxString additionalpath = basepath + _T("/") + boxes[i] + _T("/");
		SettStartBox box;
		box.topx = m_config->Read( additionalpath + _T("TopLeftX"), -1 );
		box.topy = m_config->Read( additionalpath + _T("TopLeftY"), -1 );
		box.bottomx = m_config->Read( additionalpath + _T("BottomRightX"), -1 );
		box.bottomy = m_config->Read( additionalpath + _T("BottomRightY"), -1 );
		box.ally = m_config->Read( additionalpath + _T("AllyTeam"), -1 );
		ret.push_back( box );
	}
	return ret;
}

bool Settings::GetDisableSpringVersionCheck()
{
    bool ret;
    m_config->Read( _T("/Spring/DisableVersionCheck"), &ret, false );
    return ret;
}

wxString Settings::GetLastBattleFilterProfileName()
{
    return  m_config->Read( _T("/BattleFilter/lastprofile"), _T("default") );
}


unsigned int Settings::GetTorrentPort()
{
    return  (unsigned int)m_config->Read( _T("/Torrent/Port"), DEFSETT_SPRING_PORT + 1 );
}


void Settings::SetTorrentPort( unsigned int port )
{
  m_config->Write( _T("/Torrent/port"), (int)port );
}


int Settings::GetTorrentUploadRate()
{
    return  m_config->Read( _T("/Torrent/UploadRate"), -1 );
}


void Settings::SetTorrentUploadRate( int speed )
{
  m_config->Write( _T("/Torrent/UploadRate"), speed );
}


int Settings::GetTorrentDownloadRate()
{
    return  m_config->Read( _T("/Torrent/DownloadRate"), -1 );
}



void Settings::SetTorrentDownloadRate( int speed )
{
  m_config->Write( _T("/Torrent/DownloadRate"), speed );
}


int Settings::GetTorrentSystemSuspendMode()
{
    return  m_config->Read( _T("/Torrent/SuspendMode"), 0l );
}



void Settings::SetTorrentSystemSuspendMode( int mode )
{
  m_config->Write( _T("/Torrent/SuspendMode"), mode );
}


int Settings::GetTorrentThrottledUploadRate()
{
    return  m_config->Read( _T("/Torrent/ThrottledUploadRate"), 0l );
}


void Settings::SetTorrentThrottledUploadRate( int speed )
{
  m_config->Write( _T("/Torrent/ThrottledUploadRate"), speed );
}


int Settings::GetTorrentThrottledDownloadRate()
{
    return  m_config->Read( _T("/Torrent/ThrottledDownloadRate"), 0l );
}


void Settings::SetTorrentThrottledDownloadRate( int speed )
{
  m_config->Write( _T("/Torrent/ThrottledDownloadRate"), speed );
}


int Settings::GetTorrentSystemAutoStartMode()
{
    return  m_config->Read( _T("/Torrent/AutoStartMode"), 0l );
}


void Settings::SetTorrentSystemAutoStartMode( int mode )
{
  m_config->Write( _T("/Torrent/AutoStartMode"), mode );
}


void Settings::SetTorrentMaxConnections( int connections )
{
  m_config->Write( _T("/Torrent/MaxConnections"), connections );
}


int Settings::GetTorrentMaxConnections()
{
    return  m_config->Read( _T("/Torrent/MaxConnections"), 250 );
}

void Settings::SetTorrentListToResume( const wxArrayString& list )
{
  unsigned int TorrentCount = list.GetCount();
  m_config->DeleteGroup( _T("/Torrent/ResumeList") );
  for ( unsigned int i = 0; i < TorrentCount; i++ )
  {
    m_config->Write( _T("/Torrent/ResumeList/") + TowxString(i), list[i] );
  }
}


wxArrayString Settings::GetTorrentListToResume()
{
  wxArrayString list;
  wxString old_path = m_config->GetPath();
  m_config->SetPath( _T("/Torrent/ResumeList") );
  unsigned int TorrentCount = m_config->GetNumberOfEntries( false );
  for ( unsigned int i = 0; i < TorrentCount; i++ )
  {
    wxString ToAdd;
    if ( m_config->Read( _T("/Torrent/ResumeList/") + TowxString(i), &ToAdd ) ) list.Add( ToAdd );
  }

  m_config->SetPath( old_path );
  return list;
}


wxFileName Settings::GetTorrentDir()
{
  wxFileName torrentDir(GetLobbyWriteDir());
  torrentDir.AppendDir(_T("torrents"));

  if ( !torrentDir.DirExists() )
  {
    if ( !torrentDir.Mkdir(0755) ) torrentDir.Clear();
  }

  return torrentDir;
}

wxFileName Settings::GetTorrentDataDir()
{
  wxFileName torrentDir(GetLobbyWriteDir());
  torrentDir.AppendDir(_T("downloads"));

  if ( !torrentDir.DirExists() )
  {
    if ( !torrentDir.Mkdir(0755) ) torrentDir.Clear();
  }

  return torrentDir;
}

wxString Settings::GetTempStorage()
{
  return wxFileName::GetTempDir();
}


void Settings::SetShowTooltips( bool show)
{
    m_config->Write(_T("/GUI/ShowTooltips"), show );
}

bool Settings::GetShowTooltips()
{
    return m_config->Read(_T("/GUI/ShowTooltips"), 1l);
}

void Settings::SaveLayout( wxString& layout_name, wxString& layout )
{
    m_config->Write( _T("/Layout/") + layout_name, layout );
}

wxString Settings::GetLayout( wxString& layout_name )
{
    return  m_config->Read( _T("/Layout/") + layout_name, _T("") );
}

wxArrayString Settings::GetLayoutList()
{
  return GetEntryList( _T("/Layout") );
}

void Settings::SetDefaultLayout( const wxString& layout_name )
{
	m_config->Write(_T("/GUI/DefaultLayout"), layout_name );
}

wxString Settings::GetDefaultLayout()
{
	return m_config->Read( _T("/GUI/DefaultLayout"), _T("") );
}

void Settings::SetColumnWidth( const wxString& list_name, const int coloumn_ind, const int coloumn_width )
{
    m_config->Write(_T("GUI/ColoumnWidths/") + list_name + _T("/") + TowxString(coloumn_ind), coloumn_width );
}

int Settings::GetColumnWidth( const wxString& list_name, const int coloumn )
{
    return m_config->Read(_T("GUI/ColoumnWidths/") + list_name + _T("/") + TowxString(coloumn), columnWidthUnset);
}

void Settings::SetPeopleList( const wxArrayString& friends, const wxString& group  )
{
    unsigned int friendsCount = friends.GetCount();
    m_config->DeleteGroup( _T("/Groups/") + group + _T("/Members/") );
    for ( unsigned int i = 0; i < friendsCount ; i++ )
    {
        m_config->Write(_T("/Groups/") + group + _T("/Members/") + TowxString(i), friends[i] );
    }
}

wxArrayString Settings::GetPeopleList( const wxString& group  ) const
{
    wxArrayString list;
    wxString old_path = m_config->GetPath();
    m_config->SetPath( _T("/Groups/") + group + _T("/Members/") );
    unsigned int friendsCount  = m_config->GetNumberOfEntries( false );
    for ( unsigned int i = 0; i < friendsCount ; i++ )
    {
        wxString ToAdd;
        if ( m_config->Read( _T("/Groups/") + group + _T("/Members/") +  TowxString(i), &ToAdd ) ) list.Add( ToAdd );
    }
    m_config->SetPath( old_path );
    return list;
}

void Settings::SetGroupHLColor( const wxColor& color, const wxString& group )
{
    m_config->Write( _T("/Groups/") + group + _T("/Opts/HLColor"), color.GetAsString( wxC2S_HTML_SYNTAX ) );

}

wxColor Settings::GetGroupHLColor( const wxString& group  ) const
{
    return wxColour( m_config->Read( _T("/Groups/") + group + _T("/Opts/HLColor") , _T( "#64648C" ) ) );
}

wxArrayString Settings::GetGroups( )
{
    return GetGroupList( _T("/Groups/") );
}

void Settings::AddGroup( const wxString& group )
{
    if ( !m_config->Exists( _T("/Groups/") + group ) )
    {
        m_config->Write( _T("/Groups/") , group );
        //set defaults
        SetGroupActions( group, UserActions::ActNone );
        SetGroupHLColor( defaultHLcolor, group );
    }
}

void Settings::DeleteGroup( const wxString& group )
{
    if ( m_config->Exists( _T("/Groups/") + group ) )
    {
        m_config->DeleteGroup( _T("/Groups/") + group );
    }
}

void Settings::SetGroupActions( const wxString& group, UserActions::ActionType action )
{
    //m_config->Write( _T("/Groups/") + group + _T("/Opts/Actions"), action );
  wxString key=_T("/Groups/")+group+_T("/Opts/ActionsList");
  m_config->DeleteGroup( key );
  key+=_T("/");
  unsigned int tmp=action&((UserActions::ActLast<<1)-1);
  int i=0;
  while(tmp!=0)
  {
    if(tmp&1)m_config->Write(key+m_configActionNames[i], true);
    i++;
    tmp>>=1;
  }
}

UserActions::ActionType Settings::GetGroupActions( const wxString& group ) const
{
  wxString key =_T("/Groups/") + group + _T("/Opts/Actions");
  if(m_config->HasEntry( key ) )// Backward compatibility.
  {
    wxLogMessage( _T("loading deprecated group actions and updating config") );
    UserActions::ActionType action = (UserActions::ActionType)m_config->Read( key, (long)UserActions::ActNone ) ;
    m_config->DeleteEntry(key);

		// a bit ugly, but i want to update options
    Settings *this_nonconst = const_cast<Settings*>(this);
    this_nonconst->SetGroupActions( group, action );

    return action;
  }
  key = _T("/Groups/") + group + _T("/Opts/ActionsList");
  if( !m_config->Exists( key ) ) return UserActions::ActNone;
  key += _T("/");
  int i = 0;
  int mask = 1;
  int result = 0;
  while( mask <= UserActions::ActLast )
  {
    if( m_config->Read( key + m_configActionNames[i], 0l ) )
    {
      result |= mask;
    }
    i++;
    mask <<= 1;
  }
  if( result ==0 ) return UserActions::ActNone;
  return (UserActions::ActionType)result;
}

bool Settings::ShouldAddDefaultGroupSettings()
{
		return !m_config->Exists( _T("/Groups" ));
}

void Settings::SaveCustomColors( const wxColourData& _cdata, const wxString& paletteName  )
{
    //note 16 colors is wx limit
    wxColourData cdata = _cdata;
    for ( int i = 0; i < 16; ++i)
    {
        wxColor col = cdata.GetCustomColour( i );
        if ( !col.IsOk() )
            col = wxColor ( 255, 255, 255 );
        m_config->Write( _T("/CustomColors/") + paletteName + _T("/") + TowxString(i),  col.GetAsString( wxC2S_HTML_SYNTAX ) ) ;
    }
}

wxColourData Settings::GetCustomColors( const wxString& paletteName )
{
    wxColourData cdata;
    //note 16 colors is wx limit
    for ( int i = 0; i < 16; ++i)
    {
        wxColor col( m_config->Read( _T("/CustomColors/") + paletteName + _T("/") + TowxString(i), wxColor ( 255, 255, 255 ).GetAsString( wxC2S_HTML_SYNTAX ) ) );
        cdata.SetCustomColour( i, col );
    }

    return cdata;
}


bool Settings::GetReportStats()
{
    return m_config->Read( _T("/General/reportstats"), 1l );
}


void Settings::SetReportStats(const bool value)
{
    m_config->Write( _T("/General/reportstats"), value );
}


void Settings::SetAutoUpdate( const bool value )
{
    m_config->Write( _T("/General/AutoUpdate"), value );
}


bool Settings::GetAutoUpdate()
{
    return m_config->Read( _T("/General/AutoUpdate"), true );
}

PlaybackListFilterValues Settings::GetReplayFilterValues(const wxString& profile_name)
{
    PlaybackListFilterValues filtervalues;
    filtervalues.duration =         m_config->Read( _T("/ReplayFilter/")+profile_name + _T("/duration"), _T("") );
    filtervalues.map=               m_config->Read( _T("/ReplayFilter/")+profile_name + _T("/map"), _T("") );
    filtervalues.map_show =         m_config->Read( _T("/ReplayFilter/")+profile_name + _T("/map_show"), 0L );
    filtervalues.filesize  =        m_config->Read( _T("/ReplayFilter/")+profile_name + _T("/filesize"), _T("") );
    filtervalues.filesize_mode  =   m_config->Read( _T("/ReplayFilter/")+profile_name + _T("/filesize_mode"), _T(">") );
    filtervalues.duration_mode  =   m_config->Read( _T("/ReplayFilter/")+profile_name + _T("/duration_mode"), _T(">") );
    filtervalues.mod =              m_config->Read( _T("/ReplayFilter/")+profile_name + _T("/mod"), _T("") );
    filtervalues.mod_show =         m_config->Read( _T("/ReplayFilter/")+profile_name + _T("/mod_show"), 0L );
    filtervalues.player_mode =      m_config->Read( _T("/ReplayFilter/")+profile_name + _T("/player_mode"), _T("=") );
    filtervalues.player_num  =      m_config->Read( _T("/ReplayFilter/")+profile_name + _T("/player_num"), _T("All") );

    return filtervalues;
}

void Settings::SetReplayFilterValues(const PlaybackListFilterValues& filtervalues, const wxString& profile_name)
{
    m_config->Write( _T("/ReplayFilter/")+profile_name + _T("/duration"),filtervalues.duration);
    m_config->Write( _T("/ReplayFilter/")+profile_name + _T("/map"),filtervalues.map );
    m_config->Write( _T("/ReplayFilter/")+profile_name + _T("/map_show"),filtervalues.map_show );
    m_config->Write( _T("/ReplayFilter/")+profile_name + _T("/filesize"),filtervalues.filesize );
    m_config->Write( _T("/ReplayFilter/")+profile_name + _T("/filesize_mode"),filtervalues.filesize_mode);
    m_config->Write( _T("/ReplayFilter/")+profile_name + _T("/duration_mode"),filtervalues.duration_mode);
    m_config->Write( _T("/ReplayFilter/")+profile_name + _T("/mod"),filtervalues.mod );
    m_config->Write( _T("/ReplayFilter/")+profile_name + _T("/mod_show"),filtervalues.mod_show );
    m_config->Write( _T("/ReplayFilter/")+profile_name + _T("/player_mode"),filtervalues.player_mode );
    m_config->Write( _T("/ReplayFilter/")+profile_name + _T("/player_num"),filtervalues.player_num );
    m_config->Write( _T("/ReplayFilter/lastprofile"),profile_name);
}

bool Settings::GetReplayFilterActivState() const
{
    return m_config->Read( _T("/ReplayFilter/Active") , 0l );
}

void Settings::SetReplayFilterActivState(const bool state)
{
    m_config->Write( _T("/ReplayFilter/Active") , state );
}

wxString Settings::GetLastReplayFilterProfileName()
{
    return  m_config->Read( _T("/ReplayFilter/lastprofile"), _T("default") );
}

void Settings::SetCompletionMethod( CompletionMethod method )
{
    m_config->Write( _T("/General/CompletionMethod"), (int)method);
}

Settings::CompletionMethod Settings::GetCompletionMethod(  ) const
{
    return  (CompletionMethod )m_config->Read( _T("/General/CompletionMethod"), (int)MatchExact );
}


unsigned int Settings::GetHorizontalSortkeyIndex()
{
    return m_config->Read( _T("/GUI/MapSelector/HorizontalSortkeyIndex"), 0l );
}

void Settings::SetHorizontalSortkeyIndex(const unsigned int idx)
{
    m_config->Write( _T("/GUI/MapSelector/HorizontalSortkeyIndex"), (int) idx );
}

unsigned int Settings::GetVerticalSortkeyIndex()
{
    return m_config->Read( _T("/GUI/MapSelector/VerticalSortkeyIndex"), 0l );
}

void Settings::SetVerticalSortkeyIndex(const unsigned int idx)
{
    m_config->Write( _T("/GUI/MapSelector/VerticalSortkeyIndex"), (int) idx );
}

bool Settings::GetHorizontalSortorder()
{
    return m_config->Read( _T("/GUI/MapSelector/HorizontalSortorder"), 0l );
}

void Settings::SetHorizontalSortorder(const bool order)
{
    m_config->Write( _T("/GUI/MapSelector/HorizontalSortorder"), order );
}

bool Settings::GetVerticalSortorder()
{
    return m_config->Read( _T("/GUI/MapSelector/VerticalSortorder"), 0l );
}

void Settings::SetVerticalSortorder( const bool order )
{
    m_config->Write( _T("/GUI/MapSelector/VerticalSortorder"), order );
}

void Settings::SetMapSelectorFollowsMouse( bool value )
{
    m_config->Write( _T("/GUI/MapSelector/SelectionFollowsMouse"), value);
}

bool Settings::GetMapSelectorFollowsMouse()
{
	return m_config->Read(_T("/GUI/MapSelector/SelectionFollowsMouse"), 0l );
}

unsigned int Settings::GetMapSelectorFilterRadio()
{
    return m_config->Read(_T("/GUI/MapSelector/FilterRadio"), 0l );
}

void Settings::SetMapSelectorFilterRadio( const unsigned int val )
{
    m_config->Write(_T("/GUI/MapSelector/FilterRadio"), (int) val );
}

//////////////////////////////////////////////////////////////////////////////
///                            SpringSettings                              ///
//////////////////////////////////////////////////////////////////////////////


int Settings::getMode()
{
	int mode;
	m_config->Read( _T("/SpringSettings/mode"), &mode, SET_MODE_SIMPLE );
	return mode;
}

void Settings::setMode(int mode)
{
	m_config->Write( _T("/SpringSettings/mode"), mode );
}

bool Settings::getDisableWarning()
{
	bool tmp;
	m_config->Read( _T("/SpringSettings/disableWarning"), &tmp, false );
	return tmp;
}

void Settings::setDisableWarning(bool disable)
{
	m_config->Write( _T("/SpringSettings/disableWarning"), disable);
}


wxString Settings::getSimpleRes()
{
	wxString def = vl_Resolution_Str[1];
	m_config->Read( _T("/SpringSettings/SimpleRes"),&def);
	return def;
}
void Settings::setSimpleRes(wxString res)
{
	m_config->Write(_T("/SpringSettings/SimpleRes"),res);
}

wxString Settings::getSimpleQuality()
{
	wxString def = wxT("medium");
	m_config->Read( _T("/SpringSettings/SimpleQuality"),&def);
	return def;
}

void Settings::setSimpleQuality(wxString qual)
{
	m_config->Write(_T("/SpringSettings/SimpleQuality"),qual);
}

wxString Settings::getSimpleDetail()
{
	wxString def = wxT("medium");
	m_config->Read( _T("/SpringSettings/SimpleDetail"),&def);
	return def;
}

void Settings::setSimpleDetail(wxString det)
{
	m_config->Write(_T("/SpringSettings/SimpleDetail"),det);
}

bool Settings::IsSpringBin( const wxString& path )
{
  if ( !wxFile::Exists( path ) ) return false;
  if ( !wxFileName::IsFileExecutable( path ) ) return false;
  return true;
}

void Settings::SetLanguageID ( const long id )
{
    m_config->Write( _T("/General/LanguageID") , id );
}

long Settings::GetLanguageID ( )
{
    return m_config->Read( _T("/General/LanguageID") , wxLANGUAGE_DEFAULT  );
}

SortOrder Settings::GetSortOrder( const wxString& list_name )
{
    SortOrder order;
    wxString old_path = m_config->GetPath();
    m_config->SetPath( _T("/UI/SortOrder/") + list_name + _T("/") );
    unsigned int entries  = m_config->GetNumberOfGroups( false ); //do not recurse
    for ( unsigned int i = 0; i < entries ; i++ )
    {
        SortOrderItem it;
        it.direction = m_config->Read( TowxString(i) + _T("/dir"), 1 );
        it.col = m_config->Read( TowxString(i) + _T("/col"), i );
        order[i] = it;
    }
    m_config->SetPath( old_path );
    return order;
}

void Settings::SetSortOrder( const wxString& list_name, const SortOrder& order  )
{
    SortOrder::const_iterator it = order.begin();
    for ( ; it != order.end(); ++it) {
        m_config->Write( _T("/UI/SortOrder/" ) + list_name + _T("/") + TowxString( it->first ) + _T("/dir"), it->second.direction );
        m_config->Write( _T("/UI/SortOrder/" ) + list_name + _T("/") + TowxString( it->first ) + _T("/col"), it->second.col );
    }
}

bool Settings::GetUseTabIcons()
{
    return m_config->Read(_T("/GUI/UseTabIcons"), 1l);
}

void Settings::SetUseTabIcons( bool use )
{
    m_config->Write(_T("/GUI/UseTabIcons"), use );
}

int Settings::GetSashPosition( const wxString& window_name )
{
    return m_config->Read(_T("/GUI/SashPostion/") + window_name , 200l);
}

void Settings::SetSashPosition( const wxString& window_name, const int pos )
{
    m_config->Write(_T("/GUI/SashPostion/") + window_name , pos );
}

bool Settings::GetSplitBRoomHorizontally()
{
    return m_config->Read(_T("/GUI/SplitBRoomHorizontally") , 1l );
}

void Settings::SetSplitBRoomHorizontally( const bool vertical )
{
    m_config->Write(_T("/GUI/SplitBRoomHorizontally") , vertical );
}

void Settings::SetStartTab( const int idx )
{
    m_config->Write( _T("/GUI/StartTab") , idx );
}

unsigned int Settings::GetStartTab( )
{
    return m_config->Read( _T("/GUI/StartTab") , MainWindow::PAGE_SINGLE ); //default is SP tab
}

