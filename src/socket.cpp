/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */

#ifdef _MSC_VER
#ifndef NOMINMAX
    #define NOMINMAX
#endif // NOMINMAX
#include <winsock2.h>
#endif // _MSC_VER

#include <wx/socket.h>
#include <wx/string.h>
#include <wx/log.h>
#include <stdexcept>
#include <algorithm>

#include "socket.h"
#include "server.h"
#include "utils/debug.h"
#include "utils/conversion.h"

#ifdef __WXMSW__
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <wx/msw/winundef.h>
#include <iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")
#elif defined(linux)
#include <sys/ioctl.h>
#include <net/if.h>
#endif

#define LOCK_SOCKET wxCriticalSectionLocker criticalsection_lock(m_lock)

wxString _GetHandle()
{
        wxString handle;
        #ifdef __WXMSW__

    IP_ADAPTER_INFO AdapterInfo[16];       // Allocate information for 16 cards
    DWORD dwBufLen = sizeof(AdapterInfo);  // Save memory size of buffer

    DWORD dwStatus = GetAdaptersInfo ( AdapterInfo, &dwBufLen); // Get info
                if (dwStatus != NO_ERROR) return _T(""); // Check status
    for (unsigned int i=0; i<std::min( (unsigned int)6, (unsigned int)AdapterInfo[0].AddressLength); i++)
    {
        handle += TowxString(((unsigned int)AdapterInfo[0].Address[i])&255);
        if (i != 5) handle += _T(':');
    }
        #elif defined(__WXGTK__) && defined(linux)
        int sock = socket (AF_INET, SOCK_DGRAM, 0);
        if (sock < 0)
        {
                return _T(""); //not a valid socket
        }
        struct ifreq dev; //container for the hw data
        struct if_nameindex *NameList = if_nameindex(); //container for the interfaces list
        if (NameList == NULL)
        {
                close(sock);
                return _T(""); //cannot list the interfaces
        }

        int pos = 0;
        std::string InterfaceName;
        do
        {
                if (NameList[pos].if_index == 0)
                {
                        close(sock);
                        if_freenameindex(NameList);
                        return _T(""); // no valid interfaces found
                }
                InterfaceName = NameList[pos].if_name;
                pos++;
        } while (InterfaceName.substr(0,2) == "lo" || InterfaceName.substr(0,3) == "sit");

        if_freenameindex(NameList); //free the memory

        strcpy (dev.ifr_name, InterfaceName.c_str()); //select from the name
        if (ioctl(sock, SIOCGIFHWADDR, &dev) < 0) //get the interface data
        {
                close(sock);
                return _T(""); //cannot list the interfaces
        }

    for (int i=0; i<6; i++)
    {
        handle += TowxString(((unsigned int)dev.ifr_hwaddr.sa_data[i])&255);
        if (i != 5) handle += _T(':');
    }
        close(sock);
        #endif
        return handle;
}


BEGIN_EVENT_TABLE(SocketEvents, wxEvtHandler)

EVT_SOCKET(SOCKET_ID, SocketEvents::OnSocketEvent)

END_EVENT_TABLE()


void SocketEvents::OnSocketEvent(wxSocketEvent& event)
{
  Socket* sock = (Socket*)event.GetClientData();
  try
  {
  ASSERT_LOGIC( sock != 0, _T("sock = 0") );
  } catch (...) { return; }

  if ( event.GetSocketEvent() == wxSOCKET_INPUT ) {
    m_net_class.OnDataReceived( sock );
  } else if ( event.GetSocketEvent() == wxSOCKET_LOST ) {
    m_net_class.OnDisconnected( sock );
  } else if ( event.GetSocketEvent() == wxSOCKET_CONNECTION ) {
    m_net_class.OnConnected( sock );
  } else {
    try
    {
    ASSERT_LOGIC( false, _T("Unknown socket event."));
    } catch (...) { return; };
  }
}


//! @brief Constructor
Socket::Socket( iNetClass& netclass, bool wait_on_connect, bool blocking  ) :
    m_sock( NULL ),
    m_events( NULL ),
    m_handle( _GetHandle() ),
    m_connecting( false ),
    m_wait_on_connect( wait_on_connect ),
    m_blocking(blocking),
    m_net_class(netclass),
	m_udp_private_port(0),
    m_rate(-1),
    m_sent(0)
{
}


//! @brief Destructor
Socket::~Socket()
{
  LOCK_SOCKET;
	if ( m_sock ) m_sock->Destroy();
  delete m_events;
}


//! @brief Creates an TCP socket and sets it up.
wxSocketClient* Socket::_CreateSocket()
{
    wxSocketClient* sock = new wxSocketClient();

    sock->SetClientData( (void*)this );
    if ( !m_blocking )
    {
        if ( m_events == 0 )
            m_events = new SocketEvents( m_net_class );
        sock->SetFlags( wxSOCKET_NOWAIT );

        sock->SetEventHandler(*m_events, SOCKET_ID);
        sock->SetNotify( wxSOCKET_CONNECTION_FLAG | wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG );
        sock->Notify(true);
    }
    else
    {
		// blocking mode _must_ block, and end blocking as soon as data arrives otherwise other blocking but no gui block
		// mode will wait for timeout before unlocking
		sock->SetFlags( wxSOCKET_BLOCK );
		delete m_events;
		m_events = 0;
    }
    return sock;
}

//! @brief Connect to remote host.
void Socket::Connect( const wxString& addr, const int port )
{
  LOCK_SOCKET;

  wxIPV4address wxaddr;
  m_connecting = true;
  m_buffer = "";

  wxaddr.Hostname( addr );
  wxaddr.Service( port );

  if ( m_sock != 0 ) m_sock->Destroy();
  m_sock = _CreateSocket();
  m_sock->Connect( wxaddr, m_wait_on_connect );
  m_sock->SetTimeout( 40 );
}

void Socket::SetTimeout( const int seconds )
{
    if ( m_sock != 0 )
        m_sock->SetTimeout( seconds );
}

//! @brief Disconnect from remote host if connected.
void Socket::Disconnect( )
{
  if ( m_sock ) m_sock->SetTimeout( 0 );
  m_buffer = "";

  if ( m_sock )
  {
    m_sock->Destroy();
    m_sock = 0;
  }
  m_net_class.OnDisconnected( this );
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
  if ( !m_sock )
  {
    wxLogError( _T("Socket NULL") );
    return false;
  }

	m_buffer += (const char*)data.mb_str(wxConvUTF8);
	int crop = m_buffer.length();
  if ( m_rate > 0 )
  {
  	 int max = m_rate - m_sent;
  	 if ( crop > 0 ) crop = max;
  }
  std::string send = m_buffer.substr( 0, crop );
	//wxLogMessage( _T("send: %d  sent: %d  max: %d   :  buff: %d"), send.length() , m_sent, max, m_buffer.length() );
	m_sock->Write( send.c_str(), send.length() );
	if ( !m_sock->Error() )
	{
		wxUint32 sentdata = m_sock->LastCount();
		m_buffer.erase( 0, sentdata );
		m_sent += sentdata;
	}
  return !m_sock->Error();
}


//! @brief Receive data from connection
wxString Socket::Receive()
{
	wxString ret;
  if ( m_sock == 0 )
  {
    wxLogError( _T("Socket NULL") );
    return ret;
  }

  LOCK_SOCKET;

  const int chunk_size = 1337;

  std::vector<char> buff;
  int readnum = 0;
  int totalbytes = 0;

  do
  {
  	buff.resize( totalbytes + chunk_size ); // increase buffer capacity to fit incoming chunk
    m_sock->Read( &buff[totalbytes], chunk_size );
    readnum = m_sock->LastCount();
    totalbytes += readnum;
  } while ( readnum >= chunk_size );

	if ( totalbytes > 0 )
	{
		ret = wxString( &buff[0], wxConvUTF8, totalbytes );
		if ( ret.IsEmpty() )
		{
			ret = wxString( &buff[0], wxConvLocal, totalbytes );
			if ( ret.IsEmpty() )
			{
				 ret = wxString( &buff[0], wxCSConv(_T("latin-1")), totalbytes );
			}
		}
	}

	return ret;
}

//! @brief Get curent socket state
SockState Socket::State( )
{
  if ( m_sock == 0 ) return SS_Closed;

  LOCK_SOCKET;
  if ( m_sock->IsConnected() ) {
    m_connecting = false;
    return SS_Open;
  } else {
    if ( m_connecting ) {
      return SS_Connecting;
    } else {
      return SS_Closed;
    }
  }
}


//! @brief Get socket error code
//! @todo Implement
SockError Socket::Error( ) const
{
  return (SockError)-1;
}


//! @brief used to retrieve local ip address behind NAT to communicate to the server on login
wxString Socket::GetLocalAddress() const
{
  if ( !m_sock || !m_sock->IsConnected() )
    return wxEmptyString;

  wxIPV4address localaddr;
  m_sock->GetLocal( localaddr );

  return localaddr.IPAddress();
}


//! @brief Set the maximum upload ratio.
void Socket::SetSendRateLimit( int Bps )
{
  m_rate = Bps;
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

