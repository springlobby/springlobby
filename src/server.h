//
// Class: Server
//

#ifndef _SERVER_H_
#define _SERVER_H_

#include <string>
#include "channellist.h"
#include "userlist.h"
#include "battlelist.h"

#define PING_TIMEOUT 30

class ServerEvents;
class Socket;
class Channel;
class Ui;
class Battle;


typedef int ServerError;

#define PE_NONE 0

struct UiServerData {
  UiServerData(): panel(NULL) {}
  ChatPanel* panel;
};



//! @brief Abstract baseclass that is used to implement a server protocol.
class Server
{
  public:
    friend class ServerEvents;

      
    UiServerData uidata;

    
    Server( Ui& ui): m_sock(NULL), m_ui(ui), m_keepalive(15) { }
    ~Server() {}
  
    // Server interface
  
    virtual void SetSocket( Socket* sock ) { assert( (!IsConnected()) || (sock == NULL) ); m_sock = sock; }
    virtual Socket* GetSocket( ) { return m_sock; }
  
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
  
    virtual void JoinBattle( const int& battleid, const std::string& password = "" ) = 0;
    virtual void LeaveBattle( const int& battleid ) = 0;
    
    virtual void SetKeepaliveInterval( int seconds ) { m_keepalive = seconds; }
    virtual int GetKeepaliveInterval() { return m_keepalive; }
  
    virtual void SetUsername( const std::string& username ) { m_user = username; }
    virtual void SetPassword( const std::string& password ) { m_pass = password; }
    virtual bool IsPasswordHash( const std::string& pass ) = 0;
    virtual std::string GetPasswordHash( const std::string& pass ) = 0;
    
    virtual void Ping() = 0;

    virtual void OnConnected( Socket* sock ) = 0;
    virtual void OnDisconnected( Socket* sock ) = 0;
    virtual void OnDataRecived( Socket* sock ) = 0;
    
    virtual User& GetMe() = 0;
    User& GetUser( const std::string& nickname );
    bool UserExists( const std::string& nickname );

    Channel& GetChannel( const std::string& name );
    int GetNumChannels();
    Channel& GetChannel( const int& index );
    bool ChannelExists( const std::string& name );

    Battle& GetBattle( const int& battleid );
    bool BattleExists( const int& battleid );

  protected:
    // Server variables
  
    Socket* m_sock;
    Ui& m_ui;
    int m_keepalive;
    std::string m_user;
    std::string m_pass;
    bool m_pass_hash;

    ChannelList m_channels;
    UserList m_users;
    BattleList m_battles;
  
    User& _AddUser( const std::string& user );
    void _RemoveUser( const std::string& nickname );
  
    Channel& _AddChannel( const std::string& chan );
    void _RemoveChannel( const std::string& name );    

    Battle& _AddBattle( const int& id );
    void _RemoveBattle( const int& id );    

};


#endif  //_SERVER_H_

