/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */

#include <wx/socket.h>
#include <wx/thread.h>
#include <wx/string.h>
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

  //resetting the ping state vars.
  m_ping_msg = wxEmptyString;
  m_ping_int = 0;
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


//! @brief Creates an TCP socket and sets it up.
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

//! @brief Connect to remote host.
//! @note This turns off the ping thread.
void Socket::Connect( const wxString& addr, const int port )
{
  LOCK_SOCKET;
  _EnablePingThread( false ); // Turn off ping thread.

  wxIPV4address wxaddr;
  m_connecting = true;

  wxaddr.Hostname( addr );
  wxaddr.Service( port );

  if ( m_sock != 0 ) m_sock->Destroy();
  m_sock = _CreateSocket();
  m_sock->Connect( wxaddr, m_block );
}


//! @brief Disconnect from remote host if connected.
//! @note This turns off the ping thread.
void Socket::Disconnect( )
{
  if ( m_sock == 0 ) return;
  m_serv.OnDisconnected( this );
  LOCK_SOCKET;
  _EnablePingThread( false );
  m_sock->Destroy();
  m_sock = 0;
}


//! @brief Send data over connection.
bool Socket::Send( const wxString& data )
{
  LOCK_SOCKET;
  return _Send( data );
}


//! @brief Internal send function.
//! @note Does not lock the criticalsection.
bool Socket::_Send( const wxString& data )
{
  if ( m_sock == 0 ) {
    wxLogError( _T("Socket NULL") );
    return false;
  }

  if ( m_rate > 0 ) {
    m_buffer += data;
    int max = m_rate - m_sent;
    if ( max > 0 ) {
      wxString send = m_buffer.substr( 0, max );
      m_buffer.erase( 0, max );
      //wxLogMessage( _T("send: %d  sent: %d  max: %d   :  buff: %d"), send.length() , m_sent, max, m_buffer.length() );
      std::string s = (const char*)send.mb_str(wxConvUTF8);
      m_sock->Write( s.c_str(), s.length() );
      m_sent += s.length();
    }
  } else {
    if ( data.length() <= 0) return true;
    std::string s = (const char*)data.mb_str(wxConvUTF8);
    m_sock->Write( s.c_str(), s.length() );
  }
  return !m_sock->Error();
}


//! @brief Receive data from connection
bool Socket::Receive( wxString& data )
{
  if ( m_sock == 0 ) {
    wxLogError( _T("Socket NULL") );
    return false;
  }

  LOCK_SOCKET;

  const int buff_size = 1337;

  char buff[buff_size+2] = { 0 };
  int readnum;

  buff[buff_size+1] = '\0';

  do {
    m_sock->Read( &buff[0], buff_size );
    readnum = m_sock->LastCount();
    buff[readnum] = '\0';

    if ( readnum > 0 ) {
      wxString d = wxString( &buff[0], wxConvUTF8 );
      if ( d.IsEmpty() )
      {
        d = wxString( &buff[0], wxConvLocal );
        #ifndef HAVE_WX26
        if ( d.IsEmpty() ) d = wxString( &buff[0], wxCSConv(_T("latin-1")) );
        #endif
      }
      m_rcv_buffer += d;
    }
  } while ( readnum >= buff_size );

  if ( m_rcv_buffer.Contains(_T("\n")) ) {
    data = m_rcv_buffer.BeforeFirst('\n');
    m_rcv_buffer = m_rcv_buffer.AfterFirst('\n');
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
//! @todo Implement
Sockerror Socket::Error( )
{
  return -1;
}


//! @brief Set ping info to be used by the ping thread.
//! @note Set msg to an empty string to turn off the ping thread.
//! @note This has to be set every time the socket connects.
void Socket::SetPingInfo( const wxString& msg, unsigned int interval )
{
  LOCK_SOCKET;
  m_ping_msg = msg;
  m_ping_int = interval;
  _EnablePingThread( _ShouldEnablePingThread() );
}


void Socket::_EnablePingThread( bool enable )
{

  if ( !enable ) {
    if ( m_ping_t != 0 ) {

      // Reset values to be sure.
      m_ping_int = 0;
      m_ping_msg = wxEmptyString;

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


//! @brief Check if we should enable or dsable the ping htread.
//! @see Socket::_EnablePingThread
bool Socket::_ShouldEnablePingThread()
{
  return ( (m_ping_msg != wxEmptyString) );
}


//! @brief Set the maximum upload ratio.
void Socket::SetSendRateLimit( int Bps )
{
  m_rate = Bps;
}


//! @brief Ping remote host with custom protocol message.
void Socket::Ping()
{
  // wxLogMessage( _T("Sent ping.") );

  if ( m_ping_msg != wxEmptyString ) Send( m_ping_msg );
}



void Socket::OnTimer( int mselapsed )
{
  LOCK_SOCKET;

  if ( m_rate > 0 ) {
    m_sent -= int( ( mselapsed / 1000.0 ) * m_rate );
    if ( m_sent < 0 ) m_sent = 0;
    if ( m_buffer.length() > 0 ) _Send(_T(""));
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
  m_next_ping = 0;
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
  m_next_ping = m_next_ping = m_sock.GetPingInterval();

  while ( !TestDestroy() )
  {

    if ( !m_sock.GetPingEnabled() ) break;

    m_sock.Ping();
    Sleep( m_next_ping );

  }
  m_sock.OnPingThreadStopped();
  return 0;
}


void PingThread::OnExit()
{

}
