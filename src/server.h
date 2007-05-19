//
// Class: Server
//

#ifndef _SERVER_H_
#define _SERVER_H_

#include <string>
#include "user.h"

#define PING_TIMEOUT 30

class ServerEvents;
class Socket;


typedef int ServerError;

#define PE_NONE 0


//! @brief Abstract baseclass that is used to implement a server protocol.
class Server
{
  public:
    Server(): m_sock(NULL), m_ui(NULL), m_keepalive(15) { }
    ~Server() {}
  
    // Server interface
  
    virtual void SetSocket( Socket* sock ) { assert( (!IsConnected()) || (sock == NULL) ); m_sock = sock; }
    virtual Socket* GetSocket( ) { return m_sock; }
    virtual void SetServerEvents( ServerEvents* ui ) { m_ui = ui; }
    virtual ServerEvents* GetServerEvents( ) { return m_ui; }
  
    virtual void Connect( const std::string& addr, const int port ) = 0;
    virtual void Disconnect() = 0;
    virtual bool IsConnected() = 0;
  
    virtual void Login() = 0;
    virtual void Logout() = 0;
    virtual bool IsOnline() = 0;
  
    virtual void Update() = 0;
  
    virtual void JoinChannel( const std::string& channel, const std::string& key ) = 0;
    virtual void PartChannel( const std::string& channel ) = 0;
  
    virtual void SayChannel( const std::string& channel, const std::string& msg ) = 0;
    virtual void SayPrivate( const std::string& nick, const std::string& msg ) = 0;
  
    virtual void SetKeepaliveInterval( int seconds ) { m_keepalive = seconds; }
    virtual int GetKeepaliveInterval() { return m_keepalive; }
  
    virtual void SetUsername( const std::string& username ) { m_user = username; }
    virtual void SetPassword( const std::string& password ) { m_pass = password; }
    
    virtual void Ping() = 0;

  protected:
    // Server variables
  
    Socket* m_sock;
    ServerEvents* m_ui;
    int m_keepalive;
    std::string m_user;
    std::string m_pass;

};


#endif  //_SERVER_H_

