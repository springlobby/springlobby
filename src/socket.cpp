/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */

#include <wx/socket.h>
#include <stdexcept>

#include "socket.h"
#include "server.h"
#include "utils.h"

BEGIN_EVENT_TABLE(SocketEvents, wxEvtHandler)

EVT_SOCKET(SOCKET_ID, SocketEvents::OnSocketEvent)

END_EVENT_TABLE()


void SocketEvents::OnSocketEvent(wxSocketEvent& event)
{
  Socket* sock = (Socket*)event.GetClientData();

  ASSERT_LOGIC( sock != 0, "sock = 0" );

  if ( event.GetSocketEvent() == wxSOCKET_INPUT ) {
    m_serv.OnDataRecived( sock );
  } else if ( event.GetSocketEvent() == wxSOCKET_LOST ) {
    m_serv.OnDisconnected( sock );
  } else if ( event.GetSocketEvent() == wxSOCKET_CONNECTION ) {
    m_serv.OnConnected( sock );
  } else {
    ASSERT_LOGIC( false, "Unknown socket event.");
  }
}


//! @brief Constructor
Socket::Socket( Server& serv, bool blocking ):
  m_block(blocking),
  m_serv(serv),
  m_rate(-1),
  m_sent(0)
{
  m_connecting = false;

  m_sock = new wxSocketClient();

  m_sock->SetClientData( (void*)this );
  if ( !blocking ) {
    m_events = new SocketEvents( serv );
    m_sock->SetFlags( wxSOCKET_NOWAIT );

    m_sock->SetEventHandler(*m_events, SOCKET_ID);
    m_sock->SetNotify( wxSOCKET_CONNECTION_FLAG | wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG );
    m_sock->Notify(true);
  } else {
    m_events = 0;
  }
}


//! @brief Destructor
Socket::~Socket()
{
  m_sock->Destroy();
  delete m_events;
}



//! @brief Connect to remote host
void Socket::Connect( const std::string& addr, const int port )
{
  wxIPV4address wxaddr;
  m_connecting = true;

  wxaddr.Hostname( WX_STRING( addr ) );
  wxaddr.Service( port );

  m_sock->Connect( wxaddr, m_block );

}

void Socket::Disconnect( )
{
  m_serv.OnDisconnected( this );
  m_sock->Close();
}

//! @brief Send data over connection
bool Socket::Send( const std::string& data )
{
  if ( m_rate > 0 ) {
    m_buffer += data;
    int max = m_rate - m_sent;
    if ( max > 0 ) {
      std::string send = m_buffer.substr( 0, max );
      m_buffer.erase( 0, max );
      debug( "send: " + i2s(send.length()) + " sent: " + i2s(m_sent) + " max: " + i2s(max) + " buff: " +i2s(m_buffer.length()) );
      m_sock->Write( (void*)send.c_str(), send.length() );
      m_sent += send.length();
    }
  } else {
    if ( data.length() <= 0) return true;
    m_sock->Write( (void*)data.c_str(), data.length() );
  }
  return !m_sock->Error();
}


//! @brief Recive data from connection
bool Socket::Recive( std::string& data )
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
//!
//! @todo Implement
Sockerror Socket::Error( )
{
  return -1;
}


void Socket::SetSendRateLimit( int Bps )
{
  m_rate = Bps;
}


void Socket::OnTimer( int mselapsed )
{
  if ( m_rate > 0 ) {
    m_sent -= int( ( mselapsed / 1000.0 ) * m_rate );
    if ( m_sent < 0 ) m_sent = 0;
    if ( m_buffer.length() > 0 ) Send("");
  } else {
    m_sent = 0;
  }
}

