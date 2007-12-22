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
  m_config->Flush();
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


//! @brief Restores default settings
void Settings::SetDefaultSettings()
{
  AddServer( DEFSETT_DEFAULT_SERVER );
  SetServerHost( DEFSETT_DEFAULT_SERVER, DEFSETT_DEFAULT_SERVER_HOST );
  SetServerPort( DEFSETT_DEFAULT_SERVER, DEFSETT_DEFAULT_SERVER_PORT );
  SetDefaultServer( DEFSETT_DEFAULT_SERVER );
}


//! @brief Checks if the server name/alias exists in the settings
bool Settings::ServerExists( const std::string& server_name )
{
  return m_config->Exists( _T("/Server/")+WX_STRING(server_name) );
}


//! @brief Get the name/alias of the default server.
//!
//! @note Normally this will be the previously selected server. But at first run it will be a server that is set as the default.
std::string Settings::GetDefaultServer()
{
  std::string serv = DEFSETT_DEFAULT_SERVER;
  return STD_STRING( m_config->Read( _T("/Servers/Default"), WX_STRING(serv) ) );
}


//! @brief Set the name/alias of the default server.
//!
//! @param server_name the server name/alias
//! @see GetDefaultServer()
void   Settings::SetDefaultServer( const std::string& server_name )
{
  m_config->Write( _T("/Servers/Default"), WX_STRING( server_name ) );
}


//! @brief Get hostname of a server.
//!
//! @param server_name the server name/alias
std::string Settings::GetServerHost( const std::string& server_name )
{
  std::string host = DEFSETT_DEFAULT_SERVER_HOST;
  return STD_STRING( m_config->Read( _T("/Server/")+WX_STRING(server_name)+_T("/host"), WX_STRING(host) ) );
}


//! @brief Set hostname of a server.
//!
//! @param server_name the server name/alias
//! @param value the vaule to be set
void   Settings::SetServerHost( const std::string& server_name, const std::string& value )
{
  m_config->Write( _T("/Server/")+WX_STRING(server_name)+_T("/host"), WX_STRING(value) );
}


//! @brief Get port number of a server.
//!
//! @param server_name the server name/alias
int    Settings::GetServerPort( const std::string& server_name )
{
  return m_config->Read( _T("/Server/")+WX_STRING(server_name)+_T("/port"), DEFSETT_DEFAULT_SERVER_PORT );
}


//! @brief Set port number of a server.
//!
//! @param server_name the server name/alias
//! @param value the vaule to be set
void   Settings::SetServerPort( const std::string& server_name, const int value )
{
  m_config->Write( _T("/Server/")+WX_STRING(server_name)+_T("/port"), value );
}


void Settings::AddServer( const std::string& server_name )
{
  int index = GetServerIndex( server_name );
  if ( index != -1 ) return;

  index = GetNumServers();
  SetNumServers( index + 1 );

  m_config->Write( _T("/Servers/Server")+WX_STRING(i2s(index)), WX_STRING(server_name) );
}

int Settings::GetNumServers()
{
  return m_config->Read( _T("/Servers/Count"), (long)0 );
}



void Settings::SetNumServers( int num )
{
  m_config->Write( _T("/Servers/Count"), num );
}


int Settings::GetServerIndex( const std::string& server_name )
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
std::string Settings::GetServerName( int index )
{
  return STD_STRING(m_config->Read( wxString::Format( _T("/Servers/Server%d"), index ), _T("") ));
}


//! @brief Get nickname of the default account for a server.
//!
//! @param server_name the server name/alias
std::string Settings::GetServerAccountNick( const std::string& server_name )
{
  return STD_STRING( m_config->Read( _T("/Server/")+WX_STRING(server_name)+_T("/nick"), _T("") ) );
}


//! @brief Set nickname of the default account for a server.
//!
//! @param server_name the server name/alias
//! @param value the vaule to be set
void   Settings::SetServerAccountNick( const std::string& server_name, const std::string& value )
{
  m_config->Write( _T("/Server/")+WX_STRING(server_name)+_T("/nick"), WX_STRING(value) );
}


//! @brief Get password of the default account for a server.
//!
//! @param server_name the server name/alias
//! @todo Implement
std::string Settings::GetServerAccountPass( const std::string& server_name )
{
  return STD_STRING( m_config->Read( _T("/Server/")+WX_STRING(server_name)+_T("/pass"), _T("") ) );
}


//! @brief Set password of the default account for a server.
//!
//! @param server_name the server name/alias
//! @param value the vaule to be set
//! @todo Implement
void   Settings::SetServerAccountPass( const std::string& server_name, const std::string& value )
{
  m_config->Write( _T("/Server/")+WX_STRING(server_name)+_T("/pass"), WX_STRING(value) );
}


//! @brief Get if the password should be saved for a default server account.
//!
//! @param server_name the server name/alias
//! @todo Implement
bool   Settings::GetServerAccountSavePass( const std::string& server_name )
{
  return m_config->Read( _T("/Server/")+WX_STRING(server_name)+_T("/savepass"), (long int)false );
}


//! @brief Set if the password should be saved for a default server account.
//!
//! @param server_name the server name/alias
//! @param value the vaule to be set
//! @todo Implement
void   Settings::SetServerAccountSavePass( const std::string& server_name, const bool value )
{
  m_config->Write( _T("/Server/")+WX_STRING(server_name)+_T("/savepass"), (long int)value );
}


int Settings::GetNumChannelsJoin()
{
  return m_config->Read( _T("/Channels/Count"), (long)0 );
}

void Settings::SetNumChannelsJoin( int num )
{
  m_config->Write( _T("/Channels/Count"), num );
}

void Settings::AddChannelJoin( const std::string& channel , const std::string& key )
{
  int index = GetChannelJoinIndex( channel );
  if ( index != -1 ) return;

  index = GetNumChannelsJoin();
  SetNumChannelsJoin( index + 1 );

  m_config->Write( wxString::Format( _T("/Channels/Channel%d"), index ), WX_STRING(channel) + _T(" ") + WX_STRING(key) );
}


void Settings::RemoveChannelJoin( const std::string& channel )
{
  int index = GetChannelJoinIndex( channel );
  if ( index == -1 ) return;
  int total = GetNumChannelsJoin();
  wxString LastEntry;
  m_config->Read( _T("/Channels/Channel") + WX_STRING( i2s( total - 1 ) ), &LastEntry);
  m_config->Write( _T("/Channels/Channel") + WX_STRING( i2s( index ) ), LastEntry );
  m_config->DeleteEntry( _T("/Channels/Channel") + WX_STRING( i2s( total -1 ) ) );
  SetNumChannelsJoin( total -1 );
}


int Settings::GetChannelJoinIndex( const std::string& server_name )
{
  int num = GetNumChannelsJoin();
  for ( int i= 0; i < num; i++ ) {
    wxString name = WX_STRING(GetChannelJoinName( i ));
    name = name.BeforeFirst( ' ' );
    if ( STD_STRING(name) == server_name ) return i;
  }
  return -1;
}

std::string Settings::GetChannelJoinName( int index )
{
  return STD_STRING(m_config->Read( wxString::Format( _T("/Channels/Channel%d"), index ), _T("") ));
}

//! @brief Get width of MainWindow.
int    Settings::GetMainWindowWidth()
{
  return m_config->Read( _T("/Mainwin/width"), DEFSETT_MW_WIDTH );
}


//! @brief Set width position of MainWindow
void   Settings::SetMainWindowWidth( const int value )
{
  m_config->Write( _T("/Mainwin/width"), value );
}


//! @brief Get height of MainWindow.
int    Settings::GetMainWindowHeight()
{
  return m_config->Read( _T("/Mainwin/height"), DEFSETT_MW_HEIGHT );
}


//! @brief Set height position of MainWindow
void   Settings::SetMainWindowHeight( const int value )
{
  m_config->Write( _T("/Mainwin/height"), value );
}


//! @brief Get top position of MainWindow.
int    Settings::GetMainWindowTop()
{
  return m_config->Read( _T("/Mainwin/top"), DEFSETT_MW_TOP );
}


//! @brief Set top position of MainWindow
void   Settings::SetMainWindowTop( const int value )
{
  m_config->Write( _T("/Mainwin/top"), value );
}


//! @brief Get left position of MainWindow.
int    Settings::GetMainWindowLeft()
{
  return m_config->Read( _T("/Mainwin/left"), DEFSETT_MW_LEFT );
}


//! @brief Set left position of MainWindow
void   Settings::SetMainWindowLeft( const int value )
{
  m_config->Write( _T("/Mainwin/left"), value );
}


std::string Settings::GetSpringDir()
{
  return STD_STRING(m_config->Read( _T("/Spring/dir"), DEFSETT_SPRING_DIR ));
}


void   Settings::SetSpringDir( const std::string& spring_dir )
{
  m_config->Write( _T("/Spring/dir"), WX_STRING(spring_dir) );
}


bool   Settings::GetUnitSyncUseDefLoc()
{
  return m_config->Read( _T("/Spring/use_unitsync_def_loc"), true );
}


void   Settings::SetUnitSyncUseDefLoc( const bool usedefloc )
{
  m_config->Write( _T("/Spring/use_unitsync_def_loc"), usedefloc );
}



std::string Settings::GetUnitSyncLoc()
{
  return STD_STRING(m_config->Read( _T("/Spring/unitsync_loc"), _T("") ));
}



void   Settings::SetUnitSyncLoc( const std::string& loc )
{
  m_config->Write( _T("/Spring/unitsync_loc"), WX_STRING(loc) );
}



bool   Settings::GetSpringUseDefLoc()
{
  return m_config->Read( _T("/Spring/use_spring_def_loc"), true );
}



void   Settings::SetSpringUseDefLoc( const bool usedefloc )
{
  m_config->Write( _T("/Spring/use_spring_def_loc"), usedefloc );
}



std::string Settings::GetSpringLoc()
{
  return STD_STRING(m_config->Read( _T("/Spring/exec_loc"), _T("") ));
}



void   Settings::SetSpringLoc( const std::string& loc )
{
  m_config->Write( _T("/Spring/exec_loc"), WX_STRING(loc) );
}


std::string Settings::GetSpringUsedLoc( bool force, bool defloc )
{
  bool df;
  if ( force ) df = defloc;
  else df = GetSpringUseDefLoc();

  if ( df ) {
    wxString tmp = WX_STRING(GetSpringDir());
    if ( tmp.Last() != wxFILE_SEP_PATH ) tmp += wxFILE_SEP_PATH;
    tmp += SPRING_BIN;
    return STD_STRING(tmp);
  } else {
    return GetSpringLoc();
  }
}

std::string Settings::GetUnitSyncUsedLoc( bool force, bool defloc )
{
  bool df;
  if ( force ) df = defloc;
  else df = GetUnitSyncUseDefLoc();

  if ( df ) {
    wxString tmp = WX_STRING( sett().GetSpringDir() );
    if ( tmp.Last() != wxFILE_SEP_PATH ) tmp += wxFILE_SEP_PATH;
    tmp += UNITSYNC_BIN;
    return STD_STRING( tmp );
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
  return m_config->Read( _T("/ChatLog/chatlog_loc"), _T("") );
}

void   Settings::SetChatLogLoc( const wxString& loc )
{
  m_config->Write( _T("/ChatLog/chatlog_loc"), loc );
}

std::string Settings::GetLastHostDescription()
{
  return STD_STRING( m_config->Read( _T("/Hosting/LastDescription"), _T("") ) );
}


std::string Settings::GetLastHostMod()
{
  return STD_STRING( m_config->Read( _T("/Hosting/LastMod"), _T("") ) );
}


std::string Settings::GetLastHostPassword()
{
  return STD_STRING( m_config->Read( _T("/Hosting/LastPassword"), _T("") ) );
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


std::string Settings::GetLastHostMap()
{
  return STD_STRING( m_config->Read( _T("/Hosting/LastMap"), _T("") ) );
}


void Settings::SetLastHostDescription( const std::string& value )
{
  m_config->Write( _T("/Hosting/LastDescription"), WX_STRING( value ) );
}


void Settings::SetLastHostMod( const std::string& value )
{
  m_config->Write( _T("/Hosting/LastMod"), WX_STRING( value ) );
}


void Settings::SetLastHostPassword( const std::string& value )
{
  m_config->Write( _T("/Hosting/LastPassword"), WX_STRING( value ) );
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


void Settings::SetLastHostMap( const std::string& value )
{
  m_config->Write( _T("/Hosting/LastMap"), WX_STRING(value) );
}


void Settings::SetLastAI( const std::string& ai )
{
  m_config->Write( _T("/SinglePlayer/LastAI"), WX_STRING(ai) );
}


std::string Settings::GetLastAI()
{
  return STD_STRING(m_config->Read( _T("/SinglePlayer/LastAI"), wxEmptyString ));
}

void Settings::SetDisplayJoinLeave( bool display, const wxString& channel  )
{
  m_config->Write( _T("/Channels/DisplayJoinLeave/")  + channel, display);
}

bool Settings::GetDisplayJoinLeave( const wxString& channel  )
{
  return m_config->Read( _T("/Channels/DisplayJoinLeave/") +  channel, true);
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
