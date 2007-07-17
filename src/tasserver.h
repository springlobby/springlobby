//
// Class: TASServer
//

#ifndef _TASSERVER_H_
#define _TASSERVER_H_

#include <string>
#include <list>

#include "server.h"

//#include <time.h>
//#include "ui.h"
//

//! @brief Struct used internally by the TASServer class to calculate ping roundtimes.
struct TASPingListItem {
  int id;
  time_t t;
};

class Ui;
class Socket;
class User;
class UserBattleStatus;
class ServerEvents;

//! @brief TASServer protocol implementation.
class TASServer : public Server
{
  public:
    TASServer( Ui& ui );
    ~TASServer();

    // Overloaded functions from Server
    void SetSocket( Socket* sock );
    void Connect( const std::string& addr, const int port );
    void Disconnect();
    bool IsConnected();

    void Login();
    void Logout();
    bool IsOnline();

    void Update();

    void Ping();

    User& GetMe();

    void JoinChannel( const std::string& channel, const std::string& key );
    void PartChannel( const std::string& channel );

    void SayChannel( const std::string& channel, const std::string& msg );
    void SayPrivate( const std::string& nick, const std::string& msg );

    void SayBattle( int battleid, const std::string& msg );

    void JoinBattle( const int& battleid, const std::string& password = "" );
    void LeaveBattle( const int& battleid );
    void SendMyBattleStatus( UserBattleStatus& bs );

    void RequestChannels();
    // TASServer specific functions
    void ExecuteCommand( const std::string& in );
    void ExecuteCommand( const std::string& cmd, const std::string& inparams, int replyid = -1 );

    std::string GetWordParam( std::string& params );
    std::string GetSentenceParam( std::string& params );
    int GetIntParam( std::string& params );

    void HandlePong( int replyid );
    void HandlePinglist();

    void OnConnected( Socket* sock );
    void OnDisconnected( Socket* sock );
    void OnDataRecived( Socket* sock );

    bool IsPasswordHash( const std::string& pass );
    std::string GetPasswordHash( const std::string& pass );

  protected:
    Ui& m_ui;
    ServerEvents* m_se;
    int m_ser_ver;

    bool m_connected;
    bool m_online;
    std::string m_buffer;
    time_t m_last_ping;
    int m_ping_id;
    std::list<TASPingListItem> m_pinglist;

    int m_battle_id;

    void _ReciveAndExecute();
};

#endif  //_TASSERVER_H_

