/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: Settings
//

#include <wx/config.h>
#include <wx/filefn.h>
#include <wx/intl.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <wx/font.h>

#include "nonportable.h"
#include "settings.h"
#include "utils.h"
#include "uiutils.h"
#include "battlelistfiltervalues.h"
#include "iunitsync.h"

Settings& sett()
{
  static Settings m_sett;
  return m_sett;
}

Settings::Settings()
{
  m_config = new wxConfig( _T("SpringLobby"), wxEmptyString, _T(".springlobby/springlobby.conf"), _T("springlobby.global.conf") );
  if ( !m_config->Exists( _T("/Server") ) ) SetDefaultSettings();
}

Settings::~Settings()
{
  m_config->Write( _T("/General/firstrun"), false );
  delete m_config;
}

//! @brief Saves the settings to file
void Settings::SaveSettings()
{
  m_config->Write( _T("/General/firstrun"), false );
  SetCacheVersion();
  m_config->Flush();
  if (usync()->IsLoaded()) usync()->SetSpringDataPath( GetSpringDir() );
}


bool Settings::IsFirstRun()
{
  bool first;
  m_config->Read( _T("/General/firstrun"), &first, true );
  return first;
}


bool Settings::UseOldSpringLaunchMethod()
{
  bool old;
  m_config->Read( _T("/Spring/UseOldLaunchMethod"), &old, false );
  return old;
}

bool Settings::GetNoUDP()
{
  bool tmp;
  m_config->Read( _T("/General/NoUDP"), &tmp, false );
  return tmp;
}

void Settings::SetNoUDP(bool value)
{
  m_config->Write( _T("/General/NoUDP"), value );
}

int Settings::GetClientPort(){
  int tmp;
  m_config->Read( _T("/General/ClientPort"), &tmp, 0 );
  return tmp;
}

void Settings::SetClientPort(int value){
  m_config->Write( _T("/General/ClientPort"), value );
}

bool Settings::GetShowIPAddresses()
{
  bool tmp;
  m_config->Read( _T("/General/ShowIP"), &tmp, false );
  return tmp;
}

void Settings::SetShowIPAddresses(bool value){
  m_config->Write( _T("/General/ShowIP"), value );
}

void Settings::SetOldSpringLaunchMethod( bool value )
{
  m_config->Write( _T("/Spring/UseOldLaunchMethod"), value );
}


wxString Settings::GetWebBrowserPath()
{
  wxString path;
  m_config->Read( _T("/General/WebBrowserPath"), &path, _T("use default") );
  return path;
}


void Settings::SetWebBrowserPath( const wxString path )
{
  m_config->Write( _T("/General/WebBrowserPath"), path );
}


wxString Settings::GetCachePath()
{
  wxString path = wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + _T("cache");
  m_config->Read( _T("/General/CachePath"), &path, path );
  return path + wxFileName::GetPathSeparator();
}


void Settings::SetCachePath( const wxString path )
{
  m_config->Write( _T("/General/CachePath"), path );
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


//! @brief Restores default settings
void Settings::SetDefaultSettings()
{
  AddServer( WX_STRINGC(DEFSETT_DEFAULT_SERVER) );
  SetServerHost( WX_STRINGC(DEFSETT_DEFAULT_SERVER), WX_STRINGC(DEFSETT_DEFAULT_SERVER_HOST) );
  SetServerPort( WX_STRINGC(DEFSETT_DEFAULT_SERVER), DEFSETT_DEFAULT_SERVER_PORT );
  SetDefaultServer( WX_STRINGC(DEFSETT_DEFAULT_SERVER) );
}


//! @brief Checks if the server name/alias exists in the settings
bool Settings::ServerExists( const wxString& server_name )
{
  return m_config->Exists( _T("/Server/")+ server_name );
}


//! @brief Get the name/alias of the default server.
//!
//! @note Normally this will be the previously selected server. But at first run it will be a server that is set as the default.
wxString Settings::GetDefaultServer()
{
  wxString serv = WX_STRINGC(DEFSETT_DEFAULT_SERVER);
  return m_config->Read( _T("/Servers/Default"), serv );
}


//! @brief Set the name/alias of the default server.
//!
//! @param server_name the server name/alias
//! @see GetDefaultServer()
void   Settings::SetDefaultServer( const wxString& server_name )
{
  m_config->Write( _T("/Servers/Default"),  server_name );
}


//! @brief Get hostname of a server.
//!
//! @param server_name the server name/alias
wxString Settings::GetServerHost( const wxString& server_name )
{
  wxString host = WX_STRINGC(DEFSETT_DEFAULT_SERVER_HOST);
  return m_config->Read( _T("/Server/")+ server_name +_T("/host"), host );
}


//! @brief Set hostname of a server.
//!
//! @param server_name the server name/alias
//! @param value the vaule to be set
void   Settings::SetServerHost( const wxString& server_name, const wxString& value )
{
  m_config->Write( _T("/Server/")+ server_name +_T("/host"), value );
}


//! @brief Get port number of a server.
//!
//! @param server_name the server name/alias
int    Settings::GetServerPort( const wxString& server_name )
{
  return m_config->Read( _T("/Server/")+ server_name +_T("/port"), DEFSETT_DEFAULT_SERVER_PORT );
}


//! @brief Set port number of a server.
//!
//! @param server_name the server name/alias
//! @param value the vaule to be set
void   Settings::SetServerPort( const wxString& server_name, const int value )
{
  m_config->Write( _T("/Server/")+ server_name +_T("/port"), value );
}


void Settings::AddServer( const wxString& server_name )
{
  int index = GetServerIndex( server_name );
  if ( index != -1 ) return;

  index = GetNumServers();
  SetNumServers( index + 1 );

  m_config->Write( _T("/Servers/Server")+ wxString::Format( _T("%d"), index ), server_name );
}

int Settings::GetNumServers()
{
  return m_config->Read( _T("/Servers/Count"), (long)0 );
}



void Settings::SetNumServers( int num )
{
  m_config->Write( _T("/Servers/Count"), num );
}


int Settings::GetServerIndex( const wxString& server_name )
{
  int num = GetNumServers();
  for ( int i= 0; i < num; i++ ) {
    if ( GetServerName( i ) == server_name ) return i;
  }
  return -1;
}


//! @brief Get name/alias of a server.
//!
//! @param index the server index
wxString Settings::GetServerName( int index )
{
  return m_config->Read( wxString::Format( _T("/Servers/Server%d"), index ), _T("") );
}


//! @brief Get nickname of the default account for a server.
//!
//! @param server_name the server name/alias
wxString Settings::GetServerAccountNick( const wxString& server_name )
{
  return m_config->Read( _T("/Server/")+ server_name +_T("/nick"), _T("") ) ;
}


//! @brief Set nickname of the default account for a server.
//!
//! @param server_name the server name/alias
//! @param value the vaule to be set
void Settings::SetServerAccountNick( const wxString& server_name, const wxString& value )
{
  m_config->Write( _T("/Server/")+ server_name +_T("/nick"), value );
}


//! @brief Get password of the default account for a server.
//!
//! @param server_name the server name/alias
//! @todo Implement
wxString Settings::GetServerAccountPass( const wxString& server_name )
{
  return m_config->Read( _T("/Server/")+ server_name +_T("/pass"), _T("") );
}


//! @brief Set password of the default account for a server.
//!
//! @param server_name the server name/alias
//! @param value the vaule to be set
//! @todo Implement
void   Settings::SetServerAccountPass( const wxString& server_name, const wxString& value )
{
  m_config->Write( _T("/Server/")+ server_name +_T("/pass"), value );
}


//! @brief Get if the password should be saved for a default server account.
//!
//! @param server_name the server name/alias
//! @todo Implement
bool   Settings::GetServerAccountSavePass( const wxString& server_name )
{
  return m_config->Read( _T("/Server/")+ server_name +_T("/savepass"), (long int)false );
}


//! @brief Set if the password should be saved for a default server account.
//!
//! @param server_name the server name/alias
//! @param value the vaule to be set
//! @todo Implement
void Settings::SetServerAccountSavePass( const wxString& server_name, const bool value )
{
  m_config->Write( _T("/Server/")+ server_name +_T("/savepass"), (long int)value );
}


int Settings::GetNumChannelsJoin()
{
  return m_config->Read( _T("/Channels/Count"), (long)0 );
}

void Settings::SetNumChannelsJoin( int num )
{
  m_config->Write( _T("/Channels/Count"), num );
}

void Settings::AddChannelJoin( const wxString& channel , const wxString& key )
{
  int index = GetChannelJoinIndex( channel );
  if ( index != -1 ) return;

  index = GetNumChannelsJoin();
  SetNumChannelsJoin( index + 1 );

  m_config->Write( wxString::Format( _T("/Channels/Channel%d"), index ), channel + _T(" ") + key );
}


void Settings::RemoveChannelJoin( const wxString& channel )
{
  int index = GetChannelJoinIndex( channel );
  if ( index == -1 ) return;
  int total = GetNumChannelsJoin();
  wxString LastEntry;
  m_config->Read( _T("/Channels/Channel") +  wxString::Format( _T("%d"), total - 1 ), &LastEntry);
  m_config->Write( _T("/Channels/Channel") + wxString::Format( _T("%d"), index ), LastEntry );
  m_config->DeleteEntry( _T("/Channels/Channel") + wxString::Format( _T("%d"), total - 1 ) );
  SetNumChannelsJoin( total -1 );
}


int Settings::GetChannelJoinIndex( const wxString& server_name )
{
  int num = GetNumChannelsJoin();
  for ( int i= 0; i < num; i++ ) {
    wxString name = GetChannelJoinName( i );
    name = name.BeforeFirst( ' ' );
    if ( name == server_name ) return i;
  }
  return -1;
}

wxString Settings::GetChannelJoinName( int index )
{
  return m_config->Read( wxString::Format( _T("/Channels/Channel%d"), index ), _T("") );
}
/************* SPRINGLOBBY WINDOW POS/SIZE   ******************/
//! @brief Get width of MainWindow.
int Settings::GetMainWindowWidth()
{
  return m_config->Read( _T("/Mainwin/width"), DEFSETT_MW_WIDTH );
}


//! @brief Set width position of MainWindow
void Settings::SetMainWindowWidth( const int value )
{
  m_config->Write( _T("/Mainwin/width"), value );
}


//! @brief Get height of MainWindow.
int Settings::GetMainWindowHeight()
{
  return m_config->Read( _T("/Mainwin/height"), DEFSETT_MW_HEIGHT );
}


//! @brief Set height position of MainWindow
void Settings::SetMainWindowHeight( const int value )
{
  m_config->Write( _T("/Mainwin/height"), value );
}


//! @brief Get top position of MainWindow.
int Settings::GetMainWindowTop()
{
  return m_config->Read( _T("/Mainwin/top"), DEFSETT_MW_TOP );
}


//! @brief Set top position of MainWindow
void Settings::SetMainWindowTop( const int value )
{
  m_config->Write( _T("/Mainwin/top"), value );
}


//! @brief Get left position of MainWindow.
int Settings::GetMainWindowLeft()
{
  return m_config->Read( _T("/Mainwin/left"), DEFSETT_MW_LEFT );
}

//! @brief Set left position of MainWindow
void Settings::SetMainWindowLeft( const int value )
{
  m_config->Write( _T("/Mainwin/left"), value );
}

wxString Settings::GetSpringDir()
{
  return m_config->Read( _T("/Spring/dir"), WX_STRINGC(DEFSETT_SPRING_DIR) );
}


void Settings::SetSpringDir( const wxString& spring_dir )
{
  m_config->Write( _T("/Spring/dir"), spring_dir );
}


bool Settings::GetUnitSyncUseDefLoc()
{
  return m_config->Read( _T("/Spring/use_unitsync_def_loc"), true );
}


void Settings::SetUnitSyncUseDefLoc( const bool usedefloc )
{
  m_config->Write( _T("/Spring/use_unitsync_def_loc"), usedefloc );
}



wxString Settings::GetUnitSyncLoc()
{
  return m_config->Read( _T("/Spring/unitsync_loc"), _T("") );
}



void Settings::SetUnitSyncLoc( const wxString& loc )
{
  m_config->Write( _T("/Spring/unitsync_loc"), loc );
}



bool Settings::GetSpringUseDefLoc()
{
  return m_config->Read( _T("/Spring/use_spring_def_loc"), true );
}



void Settings::SetSpringUseDefLoc( const bool usedefloc )
{
  m_config->Write( _T("/Spring/use_spring_def_loc"), usedefloc );
}



wxString Settings::GetSpringLoc()
{
  return m_config->Read( _T("/Spring/exec_loc"), _T("") );
}



void Settings::SetSpringLoc( const wxString& loc )
{
  m_config->Write( _T("/Spring/exec_loc"), loc );
}


wxString Settings::GetSpringUsedLoc( bool force, bool defloc )
{
  bool df;
  if ( force ) df = defloc;
  else df = GetSpringUseDefLoc();

  if ( df ) {
    wxString tmp = GetSpringDir();
    if ( tmp.Last() != wxFILE_SEP_PATH ) tmp += wxFILE_SEP_PATH;
    tmp += SPRING_BIN;
    return tmp;
  } else {
    return GetSpringLoc();
  }
}

wxString Settings::GetUnitSyncUsedLoc( bool force, bool defloc )
{
  bool df;
  if ( force ) df = defloc;
  else df = GetUnitSyncUseDefLoc();

  if ( df ) {
    wxString tmp = sett().GetSpringDir();
    if ( tmp.Last() != wxFILE_SEP_PATH ) tmp += wxFILE_SEP_PATH;
    tmp += _T("unitsync") + GetLibExtension();
    return tmp;
  } else {
    return sett().GetUnitSyncLoc();
  }
}

bool Settings::GetChatLogEnable()
{
    if (!m_config->Exists(_T("/ChatLog/chatlog_enable"))) SetChatLogEnable( false );
    return m_config->Read( _T("/ChatLog/chatlog_enable"), true );
}

void Settings::SetChatLogEnable( const bool value )
{
    m_config->Write( _T("/ChatLog/chatlog_enable"), value );
}

wxString Settings::GetChatLogLoc()
{
  return m_config->Read( _T("/ChatLog/chatlog_loc"), GetSpringDir() + _T("/logs") );
}

void Settings::SetChatLogLoc( const wxString& loc )
{
  m_config->Write( _T("/ChatLog/chatlog_loc"), loc );
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
  return m_config->Read( _T("/Hosting/TestHostPort"), 1 );
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




void Settings::SetBalanceMethod(int value){
  m_config->Write( _T("/Hosting/BalanceMethod"), value );
}
int Settings::GetBalanceMethod(){
  return m_config->Read( _T("/Hosting/BalanceMethod"), 1l);
}

void Settings::SetBalanceClans(bool value){
  m_config->Write( _T("/Hosting/BalanceClans"), value );
}
bool Settings::GetBalanceClans(){
  return m_config->Read( _T("/Hosting/BalanceClans"), true);
}

void Settings::SetBalanceStrongClans(bool value){
  m_config->Write( _T("/Hosting/BalanceStrongClans"), value );
}

bool Settings::GetBalanceStrongClans(){
  return m_config->Read( _T("/Hosting/BalanceStrongClans"), 0l);
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


void Settings::SetChatHistoryLenght( unsigned int historylines )
{
  m_config->Write( _T("/Chat/HistoryLinesLenght/"), (int)historylines);
}


unsigned int Settings::GetChatHistoryLenght()
{
  return (unsigned int)m_config->Read( _T("/Chat/HistoryLinesLenght/"), 1000);
}


wxColour Settings::GetChatColorNormal()
{
  return wxColour( GetColorFromStrng( m_config->Read( _T("/Chat/Colour/Normal"), _T( "0 0 0" ) ) ) );
}

void Settings::SetChatColorNormal( wxColour value )
{
  m_config->Write( _T("/Chat/Colour/Normal"), GetColorString(value) );
}

wxColour Settings::GetChatColorBackground()
{
  return wxColour( GetColorFromStrng( m_config->Read( _T("/Chat/Colour/Background"), _T( "255 255 255" ) ) ) );
}

void Settings::SetChatColorBackground( wxColour value )
{
  m_config->Write( _T("/Chat/Colour/Background"), GetColorString(value) );
}

wxColour Settings::GetChatColorHighlight()
{
  return wxColour( GetColorFromStrng( m_config->Read( _T("/Chat/Colour/Highlight"), _T( "255 0 0" ) ) ) );
}

void Settings::SetChatColorHighlight( wxColour value )
{
  m_config->Write( _T("/Chat/Colour/Highlight"), GetColorString(value) );
}

wxColour Settings::GetChatColorMine()
{
  return wxColour( GetColorFromStrng( m_config->Read( _T("/Chat/Colour/Mine"), _T( "100 100 140" ) ) ) );
}

void Settings::SetChatColorMine( wxColour value )
{
  m_config->Write( _T("/Chat/Colour/Mine"), GetColorString(value) );
}

wxColour Settings::GetChatColorNotification()
{
  return wxColour( GetColorFromStrng( m_config->Read( _T("/Chat/Colour/Notification"), _T( "255 40 40" ) ) ) );
}

void Settings::SetChatColorNotification( wxColour value )
{
  m_config->Write( _T("/Chat/Colour/Notification"), GetColorString(value) );
}

wxColour Settings::GetChatColorAction()
{
  return wxColour( GetColorFromStrng( m_config->Read( _T("/Chat/Colour/Action"), _T( "230 0 255" ) ) ) );
}

void Settings::SetChatColorAction( wxColour value )
{
  m_config->Write( _T("/Chat/Colour/Action"), GetColorString(value) );
}

wxColour Settings::GetChatColorServer()
{
  return wxColour( GetColorFromStrng( m_config->Read( _T("/Chat/Colour/Server"), _T( "0 80 128" ) ) ) );
}

void Settings::SetChatColorServer( wxColour value )
{
  m_config->Write( _T("/Chat/Colour/Server"), GetColorString(value) );
}

wxColour Settings::GetChatColorClient()
{
  return wxColour( GetColorFromStrng( m_config->Read( _T("/Chat/Colour/Client"), _T( "20 200 25" ) ) ) );
}

void Settings::SetChatColorClient( wxColour value )
{
  m_config->Write( _T("/Chat/Colour/Client"), GetColorString(value) );
}

wxColour Settings::GetChatColorJoinPart()
{
  return wxColour( GetColorFromStrng( m_config->Read( _T("/Chat/Colour/JoinPart"), _T( "0 80 0" ) ) ) );
}

void Settings::SetChatColorJoinPart( wxColour value )
{
  m_config->Write( _T("/Chat/Colour/JoinPart"), GetColorString(value) );
}

wxColour Settings::GetChatColorError()
{
  return wxColour( GetColorFromStrng( m_config->Read( _T("/Chat/Colour/Error"), _T( "128 0 0" ) ) ) );
}

void Settings::SetChatColorError( wxColour value )
{
  m_config->Write( _T("/Chat/Colour/Error"), GetColorString(value) );
}

wxColour Settings::GetChatColorTime()
{
  return wxColour( GetColorFromStrng( m_config->Read( _T("/Chat/Colour/Time"), _T( "100 100 140" ) ) ) );
}

void Settings::SetChatColorTime( wxColour value )
{
  m_config->Write( _T("/Chat/Colour/Time"), GetColorString(value) );
}

wxFont Settings::GetChatFont()
{
  wxFont f;
  f.SetNativeFontInfo( m_config->Read( _T("/Chat/Font"), wxEmptyString ) );
  return f;
}

void Settings::SetChatFont( wxFont value )
{
  m_config->Write( _T("/Chat/Font"), value.GetNativeFontInfoDesc() );
}


bool Settings::GetSmartScrollEnabled(){
  return m_config->Read( _T("/Chat/SmartScrollEnabled"), true);
}
void Settings::SetSmartScrollEnabled(bool value){
  m_config->Write( _T("/Chat/SmartScrollEnabled"), value);
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
    m_config->Write( _T("/BattleFilter/lastprofile"),profile_name);
}

bool Settings::GetDisableSpringVersionCheck()
{
  bool ret;
  m_config->Read( _T("/Spring/DisableVersionCheck"), &ret, false );
  return ret;
}

wxString Settings::GetLastFilterProfileName()
{
    return  m_config->Read( _T("/BattleFilter/lastprofile"), _T("default") );
}


