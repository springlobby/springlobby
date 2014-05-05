/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_SOCKET_H
#define SPRINGLOBBY_HEADERGUARD_SOCKET_H


/**
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

DO NOT CHANGE THIS FILE!

this file is deprecated and will be replaced with

lsl/networking/socket.h

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
**/


#include <wx/string.h>
#include <wx/event.h>
#include <string>

class iNetClass;
class wxCriticalSection;
class PingThread;

enum SockState
{
  SS_Closed,
  SS_Connecting,
  SS_Open
};

enum SockError
{
  SE_No_Error,
  SE_NotConnected,
  SE_Resolve_Host_Failed,
  SE_Connect_Host_Failed
};

const int SOCKET_ID = 100;


class SocketEvents;
class wxSocketEvent;
class wxSocketClient;

//! @brief Class that implements a TCP client socket.
class Socket
{
  public:

    Socket( iNetClass& netclass, bool wait_on_connect = false, bool blocking = false );
    ~Socket();

    // Socket interface

    void Connect( const wxString& addr, const int port );
    void Disconnect( );

    bool Send( const wxString& data );
    wxString Receive();
    //! used in plasmaservice, otherwise getting garbeld responses
    wxString ReceiveSpecial();

    wxString GetLocalAddress() const;
    std::string GetHandle() const {return m_handle;}

    SockState State( );
    SockError Error( ) const;

    void SetSendRateLimit( int Bps = -1 );
    int GetSendRateLimit() {return m_rate;}
    void OnTimer( int mselapsed );

    void SetTimeout( const int seconds );

    protected:

  // Socket variables

    wxSocketClient* m_sock;
    SocketEvents* m_events;

    wxCriticalSection m_lock;

    wxString m_ping_msg;
	std::string m_handle;

    bool m_connecting;
    bool m_wait_on_connect;
    bool m_blocking;
    iNetClass& m_net_class;

    unsigned int m_udp_private_port;
    int m_rate;
    int m_sent;
    std::string m_buffer;

    wxSocketClient* _CreateSocket();

    bool _Send( const wxString& data );
};

class SocketEvents: public wxEvtHandler
{
  public:
    SocketEvents( iNetClass& netclass ): wxEvtHandler(), m_net_class(netclass) {}
    void OnSocketEvent(wxSocketEvent& event);
  protected:
    iNetClass& m_net_class;
  DECLARE_EVENT_TABLE()
};

typedef void (*socket_callback)(Socket*);

#endif // SPRINGLOBBY_HEADERGUARD_SOCKET_H
