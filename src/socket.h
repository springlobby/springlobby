#ifndef SPRINGLOBBY_HEADERGUARD_SOCKET_H
#define SPRINGLOBBY_HEADERGUARD_SOCKET_H

#include <wx/string.h>

#include <wx/event.h>

class Server;
class Socket;
class wxSocketEvent;
class wxSocketClient;
class wxCriticalSection;

class PingThread;

typedef int Sockstate;

#define SS_CLOSED 0
#define SS_CONNECTING 1
#define SS_OPEN 2

typedef int Sockerror;

#define SE_NO_ERROR 0
#define SE_NOT_CONNECTED 1
#define SE_RESOLVE_HOST_FAILED 2
#define SE_CONNECT_TO_HOST_FAILED 3

#define SOCKET_ID 100

enum PacketType
{
  Tcp,
  Udp
};

class SocketEvents: public wxEvtHandler
{
  public:
    SocketEvents( Server& serv ): m_serv(serv) {}
    void OnSocketEvent(wxSocketEvent& event);
  protected:
    Server& m_serv;
  DECLARE_EVENT_TABLE()
};

typedef void (*socket_callback)(Socket*);


//! @brief Class that implements a TCP client socket.
class Socket
{
  public:

    Socket( Server& serv, bool blocking = false );
    ~Socket();

    // Socket interface

    void Connect( const wxString& addr, const int port );
    void Disconnect( );

    bool Send( const wxString& data );
    bool Receive( wxString& data );


    void Ping();
    void UDPPing();
    void SetPingInfo( const wxString& msg = wxEmptyString, unsigned int interval = 10000 );
    void SetUdpPingInfo( const wxString& addr = wxEmptyString, unsigned int port = 0, unsigned int interval = 10000, wxString msg = _T("") );
    unsigned int GetUDPPingPort() { return m_udp_ping_port; }
    unsigned int GetPingInterval() { return m_ping_int; }
    unsigned int GetUDPPingInterval() { return m_udp_ping_int; }
    unsigned int GetPrivateUDPPort() { return m_udp_private_port; }
    bool GetPingEnabled() { return m_ping_msg != wxEmptyString; }
    bool GetUDPPingEnabled() { return m_udp_ping_adr != wxEmptyString; }

    bool TestOpenPort( PacketType type, unsigned int port );

    Sockstate State( );
    Sockerror Error( );

    void SetSendRateLimit( int Bps = -1 );
    void OnTimer( int mselapsed );

    void OnPingThreadStarted();
    void OnPingThreadStopped();

  protected:

  // Socket variables

    wxSocketClient* m_sock;
    SocketEvents* m_events;

    wxCriticalSection m_lock;
    wxCriticalSection m_ping_thread_wait;

    wxString m_ping_msg;
    wxString m_udp_ping_adr;
    unsigned int m_ping_int;
    unsigned int m_udp_ping_port;
    unsigned int m_udp_ping_int;

    wxString m_udp_msg;
    PingThread* m_ping_t;

    bool m_connecting;
    bool m_block;
    Server& m_serv;

    unsigned int m_udp_private_port;
    int m_rate;
    int m_sent;
    wxString m_buffer;
    wxString m_rcv_buffer;

    wxSocketClient* _CreateSocket();

    bool _Send( const wxString& data );
    void _EnablePingThread( bool enable = true );
    bool _ShouldEnablePingThread();
};



class PingThread: public wxThread
{
  public:
    PingThread( Socket& sock );
    void Init();
    void* Entry();
    void OnExit();
  private:
    Socket& m_sock;
    int m_next_ping;
    int m_next_udp_ping;

};


#endif // SPRINGLOBBY_HEADERGUARD_SOCKET_H
