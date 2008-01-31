/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */

#include <wx/socket.h>
#include <wx/thread.h>
#include <wx/protocol/http.h>
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
  m_udp_ping_adr = wxEmptyString;
  m_udp_ping_int = 0;
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
bool Socket::Receive( wxString& data )
{
  if ( m_sock == 0 ) {
    wxLogError( _T("Socket NULL") );
    return false;
  }

  LOCK_SOCKET;

  wxChar buff[2];
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


//! @brief Set udp ping info.
//! @see Socket::SetPingInfo
void Socket::SetUdpPingInfo( const wxString& addr, unsigned int port, unsigned int interval )
{
  LOCK_SOCKET;
  m_udp_ping_adr = addr;
  m_udp_ping_int = interval;
  m_udp_ping_port = port;
  _EnablePingThread( _ShouldEnablePingThread() );
}


void Socket::_EnablePingThread( bool enable )
{
  if ( !enable ) {
    if ( m_ping_t != 0 ) {

      // Reset values to be sure.
      m_ping_int = 0;
      m_ping_msg = wxEmptyString;
      m_udp_ping_port = 0;
      m_udp_ping_int = 0;
      m_udp_ping_adr = wxEmptyString;

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
  return ( (m_ping_msg != wxEmptyString) || (m_udp_ping_adr != wxEmptyString) );
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
  if ( m_ping_msg != wxEmptyString ) Send( m_ping_msg );
}


/*
ling m_ui.OnUserStatusChanged( user )
20.01.04: updating battles
20.01.04: Sent udp ping.
20.01.04: UDPPing address taspringmaster.clan-sy.com port 12345
20.01.04: socket's IsOk() is false, no UDP ping done.
20.01.05: Sent ping.
20.01.05: OnBattleOpened (  )
20.01.05: MapExists (  )
*/

//! @brief Send udp ping.
//! @note used for nat travelsal.
//! @todo Use m_udp_ping_msg variable as message.
void Socket::UDPPing(){
#if(NAT_TRAVERSAL_SUPPORT)
  if ( m_ping_msg == wxEmptyString ) return;

  wxLogMessage( _T("Sent udp ping.") );

  wxLogMessage( _T("UDPPing address %s port %d"), m_udp_ping_adr.c_str(), m_udp_ping_port );

  wxIPV4address local_addr;
  local_addr.AnyAddress(); // <--- THATS ESSENTIAL!
  local_addr.Service(12345);

  wxDatagramSocket udp_socket(local_addr,/* wxSOCKET_WAITALL*/wxSOCKET_NONE);

  wxIPV4address wxaddr;
  wxaddr.Hostname(m_udp_ping_adr);
  wxaddr.Service(m_udp_ping_port);

  char *message="ipv4 sux!";
  if(udp_socket.IsOk()){
    udp_socket.SendTo(wxaddr,message,strlen(message)+1);
  }else{
    wxLogMessage(_T("socket's IsOk() is false, no UDP ping done."));
  }
  if(udp_socket.Error())wxLogMessage(_T("Error=%d"),udp_socket.LastError());
#endif
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
  m_next_udp_ping = 0;
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
  m_next_ping = -1;
  m_next_udp_ping = -1;

  while ( !TestDestroy() ) {

    if ( (m_next_ping < 0) && (m_sock.GetPingEnabled()) ) m_next_ping = m_sock.GetPingInterval();
    if ( (m_next_udp_ping < 0) && (m_sock.GetUDPPingEnabled()) ) m_next_udp_ping = m_sock.GetUDPPingInterval();

    if ( (m_next_ping >= 0) && (m_next_udp_ping >= 0) ) {
      // Both normal and udp ping enabled.

      if ( m_next_ping < m_next_udp_ping ) {
        // Normal ping next in line
        Sleep( m_next_ping );
        m_next_udp_ping -= m_next_ping;
        m_next_ping = -1;
        m_sock.Ping();
      } else {
        // UDP ping next in line.
        Sleep( m_next_udp_ping );
        m_next_ping -= m_next_udp_ping;
        m_next_udp_ping = -1;
        m_sock.UDPPing();
      }

    } else if (m_next_ping >= 0) {
      // Normal ping.
      Sleep( m_next_ping );
      m_sock.Ping();
    } else if (m_next_udp_ping >= 0) {
      // UDP ping.
      Sleep( m_next_udp_ping );
      m_sock.UDPPing();
    } else {
      // No ping enabled.
      Sleep( 100 );
    }

  }
  m_sock.OnPingThreadStopped();
  return 0;
}


void PingThread::OnExit()
{

}


//! @brief used to check if the NAT is done properly when hosting
bool Socket::TestOpenPort( PacketType type, unsigned int port )
{
  if ( type == Udp )
  {
    #ifndef HAVE_WX26
    wxIPV4address local_addr;
    local_addr.AnyAddress(); // <--- THATS ESSENTIAL!
    local_addr.Service(port);

    wxSocketServer udp_socket(local_addr, wxSOCKET_NONE);

    wxHTTP connect_to_server;
    connect_to_server.SetTimeout( 10 );

    if ( !connect_to_server.Connect( _T("zjt3.com") ) ) return false;
    connect_to_server.GetInputStream(wxString::Format( _T("/porttest.php?port=%d"), port));

    if(udp_socket.IsOk()){
      if ( !udp_socket.WaitForAccept( 10 ) ) return false;
    }else{
      wxLogMessage(_T("socket's IsOk() is false, no UDP packets can be checked"));
      return false;
    }
    if(udp_socket.Error())
    {
      wxLogMessage(_T("Error=%d"),udp_socket.LastError());
      return false;
    }
    return true;
    #endif
  }
  return false;
}
