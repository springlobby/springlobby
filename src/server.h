#ifndef SPRINGLOBBY_HEADERGUARD_SERVER_H
#define SPRINGLOBBY_HEADERGUARD_SERVER_H

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
class User;
struct UserBattleStatus;
class ChatPanel;
struct BattleOptions;

typedef int ServerError;

#define PE_NONE 0


#define HI_None 0
#define HI_Map 1
#define HI_Locked 2
#define HI_Spectators 4
#define HI_StartResources 8
#define HI_MaxUnits 16
#define HI_StartType 32
#define HI_GameType 64
#define HI_Options 128
#define HI_StartRects 256

typedef int HostInfo;

struct UiServerData {
  UiServerData(): panel(0) {}
  ChatPanel* panel;
};

//! @brief Abstract baseclass that is used to implement a server protocol.
class Server
{
  public:
    friend class ServerEvents;


    UiServerData uidata;


    Server( Ui& ui): m_sock(0), m_ui(ui), m_keepalive(15) { }
    virtual ~Server();

    // Server interface

    virtual void SetSocket( Socket* sock );
    virtual Socket* GetSocket( ) { return m_sock; }

    virtual bool Register( const std::string& addr, const int port, const std::string& nick, const std::string& password ) = 0;
    virtual void AcceptAgreement() = 0;

    virtual void Connect( const std::string& addr, const int port ) = 0;
    virtual void Disconnect() = 0;
    virtual bool IsConnected() = 0;

    virtual void Login() = 0;
    virtual void Logout() = 0;
    virtual bool IsOnline() = 0;

    virtual void Update() = 0;

    virtual void JoinChannel( const std::string& channel, const std::string& key ) = 0;
    virtual void PartChannel( const std::string& channel ) = 0;

    virtual void DoActionChannel( const std::string& channel, const std::string& msg ) = 0;
    virtual void SayChannel( const std::string& channel, const std::string& msg ) = 0;
    virtual void SayPrivate( const std::string& nick, const std::string& msg ) = 0;

    virtual void SayBattle( int battleid, const std::string& msg ) = 0;
    virtual void DoActionBattle( int battleid, const std::string& msg ) = 0;

    virtual void Ring( const std::string& nick ) = 0;

    virtual void HostBattle( BattleOptions bo, const std::string& password = "" ) = 0;
    virtual void JoinBattle( const int& battleid, const std::string& password = "" ) = 0;
    virtual void LeaveBattle( const int& battleid ) = 0;
    virtual void StartHostedBattle() = 0;

    virtual void ForceSide( int battleid, const std::string& nick, int side ) = 0;
    virtual void ForceTeam( int battleid, const std::string& nick, int team ) = 0;
    virtual void ForceAlly( int battleid, const std::string& nick, int ally ) = 0;
    virtual void ForceColour( int battleid, const std::string& nick, int r, int g, int b ) = 0;
    virtual void ForceSpectator( int battleid, const std::string& nick, bool spectator ) = 0;
    virtual void BattleKickPlayer( int battleid, const std::string& nick ) = 0;


    virtual void AddBot( int battleid, const std::string& nick, const std::string& owner, UserBattleStatus status, const std::string& aidll ) = 0;
    virtual void RemoveBot( int battleid, const std::string& nick ) = 0;
    virtual void UpdateBot( int battleid, const std::string& nick, UserBattleStatus status ) = 0;

    virtual void SendHostInfo( HostInfo update ) = 0;
    virtual void SendRaw( const std::string& raw ) = 0;

  virtual void RequestInGameTime() = 0;

    virtual Battle* GetCurrentBattle() = 0;

    virtual void RequestChannels() = 0;

    virtual void SendMyBattleStatus( UserBattleStatus& bs ) = 0;
    virtual void SendMyUserStatus() = 0;

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

#endif // SPRINGLOBBY_HEADERGUARD_SERVER_H
