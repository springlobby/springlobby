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
// Class: SpringLobbyApp
//
// Created on: Fri May 11 19:19:23 2007
//

#include "springlobbyapp.h"
#include "mainwindow.h"
#include "chatlist.h"
#include "connectwindow.h"

IMPLEMENT_APP(SpringLobbyApp)

SpringLobbyApp& app()
{
  return (SpringLobbyApp&)wxGetApp();
}


//! @brief Initializes the application.
//!
//! It will open the main window and connect default to server or open the connect window.
bool SpringLobbyApp::OnInit()
{
  m_main_win = NULL;
  m_con_win = NULL;
  m_serv = NULL;
  
  OpenMainWindow();
  DefaultConnect();

  return true;
}


//! @brief Finalizes the application
bool SpringLobbyApp::OnQuit()
{
  return true;
}


//! @brief Returns the currently used instance of Server
//!
//! @note May be NULL
//! @return The currently used instance of Server
Server* SpringLobbyApp::Serv()
{
  return m_serv;
}


//! @brief Set the currently used instance of Server
//!
//! @param serv The new Server instance to use
void SpringLobbyApp::SetServ( Server* serv )
{
  m_serv = serv;
}


//! @brief Shows the main window on screen
//!
//! @note It will create the MainWindow if not allready created
void SpringLobbyApp::OpenMainWindow()
{
  if ( m_main_win == NULL )
    m_main_win = new MainWindow();
  m_main_win->Show(true);
}


//! @brief Show the connect window on screen
//!
//! @note It will create the ConnectWindow if not allready created
void SpringLobbyApp::OpenConnectWindow()
{
  if ( m_con_win == NULL )
    m_con_win = new ConnectWindow( m_main_win );
  m_con_win->CenterOnParent();
  m_con_win->Show(true);
}


//! @brief Quits the entire application
void SpringLobbyApp::Quit()
{
  assert( m_main_win != NULL );
  m_main_win->Close();
}


//! @brief Connects to default server or opens the ConnectWindow
//!
//! @todo Fix Auto Connect
void SpringLobbyApp::DefaultConnect()
{
  OpenConnectWindow();
}


//! @brief Connects to server
//!
//! @param servername Name/alias of the server to connect to as named in the settings
//! @param username The username login with
//! @param password The password login with
//! @see ServerEvents::on_connected
void SpringLobbyApp::Connect( const string servername, const string username, const string password )
{
  string host;
  int port;
  Socket* sock;
  
  if ( !ServerExists( servername ) ) {
    assert( false );
    return;
  }
  
  if ( m_serv != NULL ) {
    // Delete old Server object
    m_serv->disconnect();
    sock =  m_serv->get_socket();
    m_serv->set_socket( NULL );
    delete sock;
    delete m_serv;
  }
  
  // Create new Server object
  m_serv = new TASServer();
  sock = new Socket();
  m_serv->set_socket( sock );
  m_serv->set_uicontrol( this );
  
  m_serv->set_username( username );
  m_serv->set_password( password );
  
  host = GetServerHost( servername );
  port = GetServerPort( servername );
  
  // Connect
  m_serv->connect( host, port );

}


//! @brief Disconnect from Server currently connected to
//!
//! @todo Implement
void SpringLobbyApp::Disconnect()
{
}

