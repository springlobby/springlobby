//
// Class: Socket
//

#include <wx/socket.h>

#include "socket.h"
#include "server.h"
#include "utils.h"

BEGIN_EVENT_TABLE(SocketEvents, wxEvtHandler)

EVT_SOCKET(SOCKET_ID, SocketEvents::OnSocketEvent)

END_EVENT_TABLE()


void SocketEvents::OnSocketEvent(wxSocketEvent& event)
{
  Socket* sock = (Socket*)event.GetClientData();
  //socket_callback callback;
  
  assert( sock != NULL );
  
  if ( event.GetSocketEvent() == wxSOCKET_INPUT ) {
/*    callback = sock->GetDataRecivedCallback();
    assert( callback != NULL );
    callback( sock );*/
    m_serv.OnDataRecived( sock );
  } else if ( event.GetSocketEvent() == wxSOCKET_LOST ) {
/*    callback = sock->GetDisconnectedCallback();
    assert( callback != NULL );
    callback( sock );*/
    m_serv.OnDisconnected( sock );
  } else if ( event.GetSocketEvent() == wxSOCKET_CONNECTION ) {
    m_serv.OnConnected( sock );
/*    callback = sock->GetConnectedCallback();
    assert( callback != NULL );
    callback( sock );*/
  } else {
    assert(false);
  }
}


//! @brief Constructor
Socket::Socket( Server& serv ) : m_serv(serv)
{
  m_connecting = false;
  
/*  m_on_con = NULL;
  m_on_discon = NULL;
  m_on_data = NULL;
  
  m_udata = NULL;*/
  
  m_sock = new wxSocketClient();
  m_events = new SocketEvents( serv );
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
void Socket::Connect( const std::string& addr, const int port )
{
  wxIPV4address wxaddr;
  m_connecting = true;

  wxaddr.Hostname( WX_STRING( addr ) );
  wxaddr.Service( port );

  m_sock->Connect( wxaddr, false );
  
}

void Socket::Disconnect( )
{
  m_serv.OnDisconnected( this );
  m_sock->Destroy();
}

//! @brief Send data over connection
bool Socket::Send( const std::string& data )
{
  if ( data.length() <= 0) return true;
  m_sock->Write( (void*)data.c_str(), data.length() );
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

