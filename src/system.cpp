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
// Class: System
//
#include "system.h"
#include "ui.h"
#include "settings.h"
#include "tasserver.h"
#include "serverevents.h"

System::System()
{
  
}


System::~System()
{
  
}


System& sys()
{
  static System m_sys;
  return m_sys;
}


//! @brief Returns the currently used Server as a pointer
//!
//! @note May be NULL
//! @return The currently used instance of Server
Server* System::serv()
{
  return m_serv;
}


//! @brief Set the currently used Server
//!
//! @param serv The new Server instance to use
void System::SetServer( Server* serv )
{
  m_serv = serv;
}


//! @brief Connects to server
//!
//! @param servername Name/alias of the server to connect to as named in the settings
//! @param username The username login with
//! @param password The password login with
//! @see ServerEvents::on_connected
void System::Connect( const string servername, const string username, const string password )
{
  string host;
  int port;
  Socket* sock;
  
  if ( !sett().ServerExists( servername ) ) {
    assert( false );
    return;
  }
  
  if ( m_serv != NULL ) {
    // Delete old Server object
    
    if ( ui().Ask( _T("Allready connected"), _T("You are allready connected to a\nserver. Do you want to disconnect?") ) ) {
      m_serv->Disconnect();
      sock =  m_serv->GetSocket();
      m_serv->SetSocket( NULL );
      delete sock;
      delete m_serv;
    } else {
      return;
    }
    
  }
  
  // Create new Server object
  m_serv = new TASServer();
  sock = new Socket();
  m_serv->SetSocket( sock );
  m_serv->SetServerEvents( &se() );
  
  m_serv->SetUsername( username );
  m_serv->SetPassword( password );
  
  host = sett().GetServerHost( servername );
  port = sett().GetServerPort( servername );
  
  // Connect
  m_serv->Connect( host, port );

}


//! @brief Disconnect from Server currently connected to
//!
//! @todo Implement
void System::Disconnect()
{
}



