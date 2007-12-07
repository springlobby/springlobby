#ifndef SPRINGLOBBY_HEADERGUARD_TASSERVER_H
#define SPRINGLOBBY_HEADERGUARD_TASSERVER_H

#include <string>
#include <list>

#include "server.h"

//! @brief Struct used internally by the TASServer class to calculate ping roundtimes.
struct TASPingListItem {
  int id;
  time_t t;
};

class Ui;
class Socket;
class User;
struct UserBattleStatus;
class ServerEvents;

//! @brief TASServer protocol implementation.
class TASServer : public Server
{
  public:
    TASServer( Ui& ui );
    ~TASServer();

    // Overloaded functions from Server
    bool ExecuteSayCommand( const wxString& cmd );

    void SetSocket( Socket* sock );

    bool Register( const std::string& addr, const int port, const std::string& nick, const std::string& password );
    void AcceptAgreement();

    void Connect( const std::string& addr, const int port );
    void Disconnect();
    bool IsConnected();

    void Login();
    void Logout();
    bool IsOnline();

    void Update( int mselapsed );

    void Ping();

    User& GetMe();

    void JoinChannel( const std::string& channel, const std::string& key );
    void PartChannel( const std::string& channel );

    void DoActionChannel( const std::string& channel, const std::string& msg );
    void SayChannel( const std::string& channel, const std::string& msg );

    void DoActionPrivate( const std::string& nick, const std::string& msg );
    void SayPrivate( const std::string& nick, const std::string& msg );

    void SayBattle( int battleid, const std::string& msg );
    void DoActionBattle( int battleid, const std::string& msg );

    void Ring( const std::string& nick );

    void ModeratorSetTopic( const std::string& channel, const std::string& topic );
    void ModeratorSetChannelKey( const std::string& channel, const std::string& key );
    void ModeratorMute( const std::string& channel, const std::string& nick, int duration, bool byip );
    void ModeratorUnmute( const std::string& channel, const std::string& nick );
    void ModeratorKick( const std::string& channel, const std::string& reason );
    void ModeratorBan( const std::string& nick, bool byip );
    void ModeratorUnban( const std::string& nick );
    void ModeratorGetIP( const std::string& nick );
    void ModeratorGetLastLogin( const std::string& nick );
    void ModeratorGetLastIP( const std::string& nick );
    void ModeratorFindByIP( const std::string& ipadress );

    void AdminGetAccountAccess( const std::string& nick );
    void AdminChangeAccountAccess( const std::string& nick, const std::string& accesscode );
    void AdminSetBotMode( const std::string& nick, bool isbot );

    void HostBattle( BattleOptions bo, const std::string& password = "" );
    void JoinBattle( const int& battleid, const std::string& password = "" );
    void LeaveBattle( const int& battleid );
    void SendMyBattleStatus( UserBattleStatus& bs );
    void SendMyUserStatus();

    void ForceSide( int battleid, const std::string& nick, int side );
    void ForceTeam( int battleid, const std::string& nick, int team );
    void ForceAlly( int battleid, const std::string& nick, int ally );
    void ForceColour( int battleid, const std::string& nick, int r, int g, int b );
    void ForceSpectator( int battleid, const std::string& nick, bool spectator );
    void BattleKickPlayer( int battleid, const std::string& nick );
    void SetHandicap( int battleid, const std::string& nick, int handicap);

    void AddBot( int battleid, const std::string& nick, const std::string& owner, UserBattleStatus status, const std::string& aidll );
    void RemoveBot( int battleid, const std::string& nick );
    void UpdateBot( int battleid, const std::string& nick, UserBattleStatus status );

    void StartHostedBattle();
    void SendHostInfo( HostInfo update );
    void SendRaw( const std::string& raw );

    void RequestInGameTime( const std::string& nick );

    Battle* GetCurrentBattle();

    void RequestChannels();
    // TASServer specific functions
    void ExecuteCommand( const std::string& in );
    void ExecuteCommand( const std::string& cmd, const std::string& inparams, int replyid = -1 );

    void HandlePong( int replyid );
    void HandlePinglist();

    void OnConnected( Socket* sock );
    void OnDisconnected( Socket* sock );
    void OnDataReceived( Socket* sock );

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

    std::string m_agreement;

    void _ReceiveAndExecute();
};

#endif // SPRINGLOBBY_HEADERGUARD_TASSERVER_H
