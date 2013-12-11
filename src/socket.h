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
    wxString GetHandle() const {return m_handle;}

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

    wxString m_ping_msg,m_handle;

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

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2011

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    SpringLobby is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

