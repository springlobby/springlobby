//
// Class: Socket
// Created on: Fri Apr 27 16:26:26 2007
//

#ifndef _SOCKET_H_
#define _SOCKET_H_

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
    
    Socket( Server& serv );
    ~Socket();
  
    // Socket interface
  
    void Connect( const std::string& addr, const int port );
    void Disconnect( );
  
    bool Send( const std::string& data );
    bool Recive( std::string& data );
  
    Sockstate State( );
    Sockerror Error( );
  
/*    void SetUserdata( void* data ) { m_udata = data; }
    void* GetUserdata() { return m_udata; }
  
    void SetConnectedCallback( socket_callback callback ) { m_on_con = callback; }
    socket_callback GetConnectedCallback() { return m_on_con; }

    void SetDisconnectedCallback( socket_callback callback ) { m_on_discon = callback; }
    socket_callback GetDisconnectedCallback() { return m_on_discon; }

    void SetDataRecivedCallback( socket_callback callback ) { m_on_data = callback; }
    socket_callback GetDataRecivedCallback() { return m_on_data; }
*/
  protected:
  // Socket variables
    wxSocketClient* m_sock;
    SocketEvents* m_events;
/*    socket_callback m_on_con;
    socket_callback m_on_discon;
    socket_callback m_on_data;*/
    bool m_connecting;
    //void* m_udata;
    Server& m_serv;
};


#endif  //_SOCKET_H_

