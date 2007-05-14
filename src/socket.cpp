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
// Class: Socket
//
// Created on: Fri Apr 27 16:26:26 2007
//

#include "socket.h"

BEGIN_EVENT_TABLE(SocketEvents, wxEvtHandler)

EVT_SOCKET(SOCKET_ID, SocketEvents::OnSocketEvent)

END_EVENT_TABLE()



void SocketEvents::OnSocketEvent(wxSocketEvent& event)
{
  Socket* sock = (Socket*)event.GetClientData();
  socket_callback callback;
  
  assert( sock != NULL );
  
  if ( event.GetSocketEvent() == wxSOCKET_INPUT ) {
    callback = sock->GetDataRecivedCallback();
    assert( callback != NULL );
    callback( sock );
  } else if ( event.GetSocketEvent() == wxSOCKET_LOST ) {
    callback = sock->GetDisconnectedCallback();
    assert( callback != NULL );
    callback( sock );
  } else if ( event.GetSocketEvent() == wxSOCKET_CONNECTION ) {
    callback = sock->GetConnectedCallback();
    assert( callback != NULL );
    callback( sock );
  } else {
    assert(false);
  }
}


//! @brief Constructor
Socket::Socket()
{
  m_connecting = false;
  
  m_on_con = NULL;
  m_on_discon = NULL;
  m_on_data = NULL;
  
  m_udata = NULL;
  
  m_sock = new wxSocketClient();
  m_events = new SocketEvents;
  m_sock->SetFlags( wxSOCKET_NOWAIT );
  
  m_sock->SetEventHandler(*m_events, SOCKET_ID);
  m_sock->SetClientData( (void*)this );
  m_sock->SetNotify( wxSOCKET_CONNECTION_FLAG | wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG );
  m_sock->Notify(true);
}


//! @brief Destructor
Socket::~Socket()
{
  m_sock->Destroy();
}


//! @brief Connect to remote host
void Socket::Connect( const string addr, const int port )
{
  wxIPV4address wxaddr;
  m_connecting = true;

  wxaddr.Hostname( WX_STRING( addr ) );
  wxaddr.Service( port );

  m_sock->Connect( wxaddr, false );
  
}


//! @brief Disconnect from current connection
void Socket::Disconnect( )
{
  m_connecting = false;
}


//! @brief Send data over connection
bool Socket::Send( string data )
{
  if ( data.length() <= 0) return true;
  m_sock->Write( (void*)data.c_str(), data.length() );
  return !m_sock->Error();
}


//! @brief Recive data from connection
bool Socket::Recive( string& data )
{
  char buff[2];
  int readnum;
  int readbytes = 0;
  
  buff[1] = '\0';
  
  do {
    m_sock->Read( (void*)&buff[0], 1 );
    readnum = m_sock->LastCount();
  
    if ( readnum > 0 ) {
      data += &buff[0];
      readbytes++;
    }
  } while ( (readnum > 0) && (buff[0] != '\n') );
  
  if ( readbytes > 0 ) {
    //cout << "** Socket::Recive(): recived: \"" << data.c_str() << "\"" << endl;  
    return true;
  } else {
    return false;
  }
}


//! @brief Get curent socket state
Sockstate Socket::State( )
{
  if ( m_sock->IsConnected() ) {
    m_connecting = false;
    return SS_OPEN;
  } else {
    if ( m_connecting ) {
      return SS_CONNECTING;
    } else {
      return SS_CLOSED;
    }
  }
}


//! @brief Get socket error code
Sockerror Socket::Error( )
{
  
}


void Socket::SetUserdata( void* data )
{
  m_udata = data;
}


void* Socket::GetUserdata()
{
  return m_udata;
}


void Socket::SetConnectedCallback( socket_callback callback )
{
  m_on_con = callback;
}


socket_callback Socket::GetConnectedCallback()
{
  return m_on_con;
}


void Socket::SetDisconnectedCallback( socket_callback callback )
{
  m_on_discon = callback;
}


socket_callback Socket::GetDisconnectedCallback()
{
  return m_on_discon;
}


void Socket::SetDataRecivedCallback( socket_callback callback )
{
  m_on_data = callback;
}


socket_callback Socket::GetDataRecivedCallback()
{
  return m_on_data;
}
