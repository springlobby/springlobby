/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */

#include <wx/socket.h>
#include <wx/thread.h>
#include <stdexcept>

#include "socket.h"
#include "server.h"
#include "utils.h"


#define LOCK_SOCKET wxCriticalSectionLocker criticalsection_lock(m_lock)


BEGIN_EVENT_TABLE(SocketEvents, wxEvtHandler)

EVT_SOCKET(SOCKET_ID, SocketEvents::OnSocketEvent)

END_EVENT_TABLE()


void SocketEvents::OnSocketEvent(wxSocketEvent& event)
{
  Socket* sock = (Socket*)event.GetClientData();

  ASSERT_LOGIC( sock != 0, _T("sock = 0") );

  if ( event.GetSocketEvent() == wxSOCKET_INPUT ) {
    m_serv.OnDataReceived( sock );
  } else if ( event.GetSocketEvent() == wxSOCKET_LOST ) {
    m_serv.OnDisconnected( sock );
  } else if ( event.GetSocketEvent() == wxSOCKET_CONNECTION ) {
    m_serv.OnConnected( sock );
  } else {
    ASSERT_LOGIC( false, _T("Unknown socket event."));
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

  m_sock = 0;
  m_events = 0;
  m_ping_msg = wxEmptyString;
  m_ping_int = -1;
  m_ping_t = 0;

}


//! @brief Destructor
Socket::~Socket()
{
  LOCK_SOCKET;
  if ( m_sock != 0 ) m_sock->Destroy();
  delete m_events;
  if ( m_ping_t != 0 ) {
    m_ping_t->Delete();
    m_ping_thread_wait.Enter();
  }
}


wxSocketClient* Socket::_CreateSocket()
{
  wxSocketClient* sock = new wxSocketClient();

  sock->SetClientData( (void*)this );
  if ( !m_block ) {
    if ( m_events == 0 ) m_events = new SocketEvents( m_serv );
    sock->SetFlags( wxSOCKET_NOWAIT );

    sock->SetEventHandler(*m_events, SOCKET_ID);
    sock->SetNotify( wxSOCKET_CONNECTION_FLAG | wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG );
    sock->Notify(true);
  } else {
    if ( m_events != 0 ) {
      delete m_events;
      m_events = 0;
    }
  }
  return sock;
}

//! @brief Connect to remote host
void Socket::Connect( const std::string& addr, const int port )
{
  LOCK_SOCKET;
  _SetPingInfo( wxEmptyString ); // Turn off ping thread.

  wxIPV4address wxaddr;
  m_connecting = true;

  wxaddr.Hostname( WX_STRING( addr ) );
  wxaddr.Service( port );

  if ( m_sock != 0 ) m_sock->Destroy();
  m_sock = _CreateSocket();
  m_sock->Connect( wxaddr, m_block );
}

void Socket::Disconnect( )
{
  if ( m_sock == 0 ) return;
  m_serv.OnDisconnected( this );
  LOCK_SOCKET;
  m_sock->Destroy();
  m_sock = 0;
  _SetPingInfo( wxEmptyString );
}

//! @brief Send data over connection
bool Socket::Send( const std::string& data )
{
  LOCK_SOCKET;
  return _Send( data );
}

bool Socket::_Send( const std::string& data )
{
  if ( m_sock == 0 ) {
    wxLogError( _T("Socket NULL") );
    return false;
  }

  if ( m_rate > 0 ) {
    m_buffer += data;
    int max = m_rate - m_sent;
    if ( max > 0 ) {
      std::string send = m_buffer.substr( 0, max );
      m_buffer.erase( 0, max );
      //wxLogMessage( _T("send: %d  sent: %d  max: %d   :  buff: %d"), send.length() , m_sent, max, m_buffer.length() );
      m_sock->Write( (void*)send.c_str(), send.length() );
      m_sent += send.length();
    }
  } else {
    if ( data.length() <= 0) return true;
    m_sock->Write( (void*)data.c_str(), data.length() );
  }
  return !m_sock->Error();
}


//! @brief Receive data from connection
bool Socket::Receive( std::string& data )
{
  if ( m_sock == 0 ) {
    wxLogError( _T("Socket NULL") );
    return false;
  }

  LOCK_SOCKET;

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
  if ( m_sock == 0 ) return SS_CLOSED;

  LOCK_SOCKET;
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


void Socket::SetPingInfo( const wxString& msg, unsigned int interval )
{
  LOCK_SOCKET;
  _SetPingInfo( msg, interval );
}


//! @brief Set ping info to be used by the ping thread.
//!
//! @note Set msg to an empty string to turn off the ping thread.
//! @note This has to be set every time the socket connects.
void Socket::_SetPingInfo( const wxString& msg, unsigned int interval )
{
  m_ping_msg = msg;
  m_ping_int = interval;
  if ( msg == wxEmptyString ) {
    if ( m_ping_t != 0 ) {
      m_ping_t->Delete();
      m_ping_thread_wait.Enter();
      m_ping_t = 0;
    }
  } else {
    if ( m_ping_t == 0 ) {
      m_ping_t = new PingThread( *this );
      m_ping_t->Init();
    }
  }
}


//! @brief Set the maximum upload ratio.
void Socket::SetSendRateLimit( int Bps )
{
  m_rate = Bps;
}


//! @brief Ping remote host with custom protocol message.
void Socket::Ping()
{
  wxLogMessage( _T("Sent ping.") );
  if ( m_ping_msg != wxEmptyString ) Send( STD_STRING(m_ping_msg) );
}


void Socket::OnTimer( int mselapsed )
{
  LOCK_SOCKET;

  if ( m_rate > 0 ) {
    m_sent -= int( ( mselapsed / 1000.0 ) * m_rate );
    if ( m_sent < 0 ) m_sent = 0;
    if ( m_buffer.length() > 0 ) _Send("");
  } else {
    m_sent = 0;
  }
}


void Socket::OnPingThreadStarted()
{
  m_ping_thread_wait.Enter();
}


void Socket::OnPingThreadStopped()
{
  m_ping_thread_wait.Leave();
}


PingThread::PingThread( Socket& sock ):
  m_sock(sock)
{
}


void PingThread::Init()
{
  Create();
  SetPriority( WXTHREAD_MAX_PRIORITY );
  Run();
}


void* PingThread::Entry()
{
  m_sock.OnPingThreadStarted();
  while ( !TestDestroy() ) {
    Sleep( m_sock.GetPingInterval() );
    m_sock.Ping();
  }
  return 0;
}


void PingThread::OnExit()
{
  m_sock.OnPingThreadStopped();
}

