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


//! @brief Constructor
Settings::Settings()
{
  m_config = new wxConfig( _("SpringLobby") );
}


//! @brief Destructor
Settings::~Settings()
{
  delete m_config;
}


//! @brief Get the name/alias of the default server.
//!
//! @note Normally this will be the previously selected server. But at first run it will be a server that is set as the default.
string Settings::GetDefaultServer()
{
//  return string( (char*)m_config->Read(_("/Servers/Default"), _(DEFSETT_DEFAULT_SERVER) ).c_str() );
  return STL_STRING( m_config->Read(_("/Servers/Default"), _(DEFSETT_DEFAULT_SERVER) ) );
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
//! @todo Implement
string Settings::GetServerHost( const string server_name )
{
}


//! @brief Set hostname of a server.
//! 
//! @param server_name the server name/alias
//! @param value the vaule to be set
//! @todo Implement
void   Settings::SetServerHost( const string server_name, const string value )
{
}


//! @brief Get port number of a server.
//! 
//! @param server_name the server name/alias
//! @todo Implement
int    Settings::GetServerPort( const string server_name )
{
}


//! @brief Set port number of a server.
//! 
//! @param server_name the server name/alias
//! @param value the vaule to be set
//! @todo Implement
void   Settings::SetServerPort( const string server_name, const int value )
{
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
//! @todo Implement
string Settings::GetServerAccountNick( const string server_name )
{
}


//! @brief Set nickname of the default account for a server.
//! 
//! @param server_name the server name/alias
//! @param value the vaule to be set
//! @todo Implement
void   Settings::SetServerAccountNick( const string server_name, const string value )
{
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

