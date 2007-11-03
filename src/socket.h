#ifndef SPRINGLOBBY_HEADERGUARD_SOCKET_H
#define SPRINGLOBBY_HEADERGUARD_SOCKET_H

#include <string>

#include <wx/event.h>

class Server;
class Socket;
class wxSocketEvent;
class wxSocketClient;

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

    void Connect( const std::string& addr, const int port );
    void Disconnect( );

    bool Send( const std::string& data );
    bool Recive( std::string& data );

    Sockstate State( );
    Sockerror Error( );

    void SetSendRateLimit( int Bps = -1 );
    void OnTimer( int mselapsed );

  protected:
  // Socket variables
    wxSocketClient* m_sock;
    SocketEvents* m_events;

    bool m_connecting;
    bool m_block;
    Server& m_serv;

    int m_rate;
    int m_sent;
    std::string m_buffer;

    wxSocketClient* _CreateSocket();
};

#endif // SPRINGLOBBY_HEADERGUARD_SOCKET_H
