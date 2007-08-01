//
// Class: Settings
//

#include <wx/config.h>
#include <wx/filefn.h>
#include <wx/intl.h>

#include "nonportable.h"
#include "settings.h"
#include "utils.h"

Settings& sett()
{
  static Settings m_sett;
  return m_sett;
}

Settings::Settings()
{
  m_config = new wxConfig( _T("SpringLobby") );
  if ( !m_config->Exists( _T("/Servers") ) )
    SetDefaultSettings();
}

Settings::~Settings()
{
  m_config->Write( _T("/General/firstrun"), false );
  delete m_config;
}

//! @brief Saves the settings to file
void Settings::SaveSettings()
{
  m_config->Flush();
}


bool Settings::IsFirstRun()
{
  return m_config->Read( _T("/General/firstrun"), true );
}


//! @brief Restores default settings
void Settings::SetDefaultSettings()
{
  wxString defserver;
  defserver = DEFSETT_DEFAULT_SERVER;
  m_config->Write( _T("/Servers/Default"), defserver );
  m_config->Write( _T("/Server/")+defserver+_T("/host"), DEFSETT_DEFAULT_SERVER_HOST );
  m_config->Write( _T("/Server/")+defserver+_T("/port"), DEFSETT_DEFAULT_SERVER_PORT );
  //! @todo Save all default settings
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
  return STD_STRING( m_config->Read( _T("/Servers/Default"), DEFSETT_DEFAULT_SERVER ) );
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
  return STD_STRING( m_config->Read( _T("/Server/")+WX_STRING(server_name)+_T("/host"), DEFSETT_DEFAULT_SERVER_HOST ) );
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


//! @brief Get name/alias of a server.
//!
//! @param server_name the server name/alias
//! @todo Implement
std::string Settings::GetServerName( const std::string& server_name )
{
  return "";
}


//! @brief Set name/alias of a server.
//!
//! @param server_name the server name/alias
//! @param value the vaule to be set
//! @todo Implement
void   Settings::SetServerName( const std::string& server_name, const std::string& value )
{
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


std::string Settings::GetSpringUsedLoc()
{
  if ( GetSpringUseDefLoc() ) {
    wxString tmp = WX_STRING(GetSpringDir());
    tmp += wxFILE_SEP_PATH;
    tmp += SPRING_BIN;
    return STD_STRING(tmp);
  } else {
    return GetSpringLoc();
  }
}


