//
// Class: Settings
//

#include "settings.h"
#include <wx/intl.h>
#include <iostream>


Settings& sett()
{
  static Settings m_sett;
  return m_sett;
}


//! @brief Saves the settings to file
void Settings::SaveSettings()
{
  m_config->Flush();
}


//! @brief Restores default settings
void Settings::SetDefaultSettings()
{
  wxString defserver;
  defserver = _(DEFSETT_DEFAULT_SERVER);
  m_config->Write( _("/Servers/Default"), defserver );
  m_config->Write( _("/Server/")+defserver+_("/host"), _(DEFSETT_DEFAULT_SERVER_HOST) );
  m_config->Write( _("/Server/")+defserver+_("/port"), DEFSETT_DEFAULT_SERVER_PORT );
  //! @todo Save all default settings
}


//! @brief Checks if the server name/alias exists in the settings
bool Settings::ServerExists( const std::string& server_name )
{
  return m_config->Exists( _("/Server/")+WX_STRING(server_name) );
}


//! @brief Get the name/alias of the default server.
//!
//! @note Normally this will be the previously selected server. But at first run it will be a server that is set as the default.
std::string Settings::GetDefaultServer()
{
  return STL_STRING( m_config->Read( _("/Servers/Default"), _(DEFSETT_DEFAULT_SERVER) ) );
}


//! @brief Set the name/alias of the default server.
//! 
//! @param server_name the server name/alias
//! @see GetDefaultServer()
void   Settings::SetDefaultServer( const std::string& server_name )
{
  m_config->Write( _("/Servers/Default"), WX_STRING( server_name ) );
}


//! @brief Get hostname of a server.
//! 
//! @param server_name the server name/alias
std::string Settings::GetServerHost( const std::string& server_name )
{
  return STL_STRING( m_config->Read( _("/Server/")+WX_STRING(server_name)+_("/host"), _(DEFSETT_DEFAULT_SERVER_HOST) ) );
}


//! @brief Set hostname of a server.
//! 
//! @param server_name the server name/alias
//! @param value the vaule to be set
void   Settings::SetServerHost( const std::string& server_name, const std::string& value )
{
  m_config->Write( _("/Server/")+WX_STRING(server_name)+_("/host"), WX_STRING(value) );
}


//! @brief Get port number of a server.
//! 
//! @param server_name the server name/alias
int    Settings::GetServerPort( const std::string& server_name )
{
  return m_config->Read( _("/Server/")+WX_STRING(server_name)+_("/port"), DEFSETT_DEFAULT_SERVER_PORT );
}


//! @brief Set port number of a server.
//! 
//! @param server_name the server name/alias
//! @param value the vaule to be set
void   Settings::SetServerPort( const std::string& server_name, const int value )
{
  m_config->Write( _("/Server/")+WX_STRING(server_name)+_("/port"), value );
}


//! @brief Get name/alias of a server.
//! 
//! @param server_name the server name/alias
//! @todo Implement
std::string Settings::GetServerName( const std::string& server_name )
{
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
  return STL_STRING( m_config->Read( _("/Server/")+WX_STRING(server_name)+_("/nick"), _("") ) );
}


//! @brief Set nickname of the default account for a server.
//! 
//! @param server_name the server name/alias
//! @param value the vaule to be set
void   Settings::SetServerAccountNick( const std::string& server_name, const std::string& value )
{
  m_config->Write( _("/Server/")+WX_STRING(server_name)+_("/nick"), WX_STRING(value) );
}


//! @brief Get password of the default account for a server.
//! 
//! @param server_name the server name/alias
//! @todo Implement
std::string Settings::GetServerAccountPass( const std::string& server_name )
{
}


//! @brief Set password of the default account for a server.
//! 
//! @param server_name the server name/alias
//! @param value the vaule to be set
//! @todo Implement
void   Settings::SetServerAccountPass( const std::string& server_name, const std::string& value )
{
}


//! @brief Get if the password should be saved for a default server account.
//! 
//! @param server_name the server name/alias
//! @todo Implement
bool   Settings::GetServerAccountSavePass( const std::string& server_name )
{
}


//! @brief Set if the password should be saved for a default server account.
//! 
//! @param server_name the server name/alias
//! @param value the vaule to be set
//! @todo Implement
void   Settings::SetServerAccountSavePass( const std::string& server_name, const bool value )
{
}


//! @brief Get width of MainWindow.
int    Settings::GetMainWindowWidth()
{
  return m_config->Read( _("/Mainwin/width"), DEFSETT_MW_WIDTH );
}


//! @brief Set width position of MainWindow
void   Settings::SetMainWindowWidth( const int value )
{
  m_config->Write( _("/Mainwin/width"), value );
}


//! @brief Get height of MainWindow.
int    Settings::GetMainWindowHeight()
{
  return m_config->Read( _("/Mainwin/height"), DEFSETT_MW_HEIGHT );
}


//! @brief Set height position of MainWindow
void   Settings::SetMainWindowHeight( const int value )
{
  m_config->Write( _("/Mainwin/height"), value );
}


//! @brief Get top position of MainWindow.
int    Settings::GetMainWindowTop()
{
  return m_config->Read( _("/Mainwin/top"), DEFSETT_MW_TOP );
}


//! @brief Set top position of MainWindow
void   Settings::SetMainWindowTop( const int value )
{
  m_config->Write( _("/Mainwin/top"), value );
}


//! @brief Get left position of MainWindow.
int    Settings::GetMainWindowLeft()
{
  return m_config->Read( _("/Mainwin/left"), DEFSETT_MW_LEFT );
}


//! @brief Set left position of MainWindow
void   Settings::SetMainWindowLeft( const int value )
{
  m_config->Write( _("/Mainwin/left"), value );
}



