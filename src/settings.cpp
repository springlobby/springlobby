/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */


//
// Class: Settings
//
// Created on: Thu May 10 23:28:05 2007
//

#include "settings.h"
#include <wx/intl.h>
#include <iostream>


//! @brief Constructor
Settings::Settings()
{
  cout << "** Settings::Settings()" << endl;
  m_config = new wxConfig( _("SpringLobby") );
  if ( !m_config->Exists( _("/Servers") ) )
    SetDefaultSettings();
}


//! @brief Destructor
Settings::~Settings()
{
  cout << "** Settings::~Settings()" << endl;
  m_config->Flush();
  delete m_config;
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
bool Settings::ServerExists( const string server_name )
{
  return m_config->Exists( _("/Server/")+WX_STRING(server_name) );
}


//! @brief Get the name/alias of the default server.
//!
//! @note Normally this will be the previously selected server. But at first run it will be a server that is set as the default.
string Settings::GetDefaultServer()
{
  return STL_STRING( m_config->Read( _("/Servers/Default"), _(DEFSETT_DEFAULT_SERVER) ) );
}


//! @brief Set the name/alias of the default server.
//! 
//! @param server_name the server name/alias
//! @see GetDefaultServer()
void   Settings::SetDefaultServer( const string server_name )
{
  m_config->Write( _("/Servers/Default"), WX_STRING( server_name ) );
}


//! @brief Get hostname of a server.
//! 
//! @param server_name the server name/alias
string Settings::GetServerHost( const string server_name )
{
  return STL_STRING( m_config->Read( _("/Server/")+WX_STRING(server_name)+_("/host"), _(DEFSETT_DEFAULT_SERVER_HOST) ) );
}


//! @brief Set hostname of a server.
//! 
//! @param server_name the server name/alias
//! @param value the vaule to be set
void   Settings::SetServerHost( const string server_name, const string value )
{
  m_config->Write( _("/Server/")+WX_STRING(server_name)+_("/host"), WX_STRING(value) );
}


//! @brief Get port number of a server.
//! 
//! @param server_name the server name/alias
int    Settings::GetServerPort( const string server_name )
{
  return m_config->Read( _("/Server/")+WX_STRING(server_name)+_("/port"), DEFSETT_DEFAULT_SERVER_PORT );
}


//! @brief Set port number of a server.
//! 
//! @param server_name the server name/alias
//! @param value the vaule to be set
void   Settings::SetServerPort( const string server_name, const int value )
{
  m_config->Write( _("/Server/")+WX_STRING(server_name)+_("/port"), value );
}


//! @brief Get name/alias of a server.
//! 
//! @param server_name the server name/alias
//! @todo Implement
string Settings::GetServerName( const string server_name )
{
}


//! @brief Set name/alias of a server.
//! 
//! @param server_name the server name/alias
//! @param value the vaule to be set
//! @todo Implement
void   Settings::SetServerName( const string server_name, const string value )
{
}


//! @brief Get nickname of the default account for a server.
//! 
//! @param server_name the server name/alias
string Settings::GetServerAccountNick( const string server_name )
{
  return STL_STRING( m_config->Read( _("/Server/")+WX_STRING(server_name)+_("/nick"), _("") ) );
}


//! @brief Set nickname of the default account for a server.
//! 
//! @param server_name the server name/alias
//! @param value the vaule to be set
void   Settings::SetServerAccountNick( const string server_name, const string value )
{
  m_config->Write( _("/Server/")+WX_STRING(server_name)+_("/nick"), WX_STRING(value) );
}


//! @brief Get password of the default account for a server.
//! 
//! @param server_name the server name/alias
//! @todo Implement
string Settings::GetServerAccountPass( const string server_name )
{
}


//! @brief Set password of the default account for a server.
//! 
//! @param server_name the server name/alias
//! @param value the vaule to be set
//! @todo Implement
void   Settings::SetServerAccountPass( const string server_name, const string value )
{
}


//! @brief Get if the password should be saved for a default server account.
//! 
//! @param server_name the server name/alias
//! @todo Implement
bool   Settings::GetServerAccountSavePass( const string server_name )
{
}


//! @brief Set if the password should be saved for a default server account.
//! 
//! @param server_name the server name/alias
//! @param value the vaule to be set
//! @todo Implement
void   Settings::SetServerAccountSavePass( const string server_name, const bool value )
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



