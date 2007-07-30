#ifndef SPRINGLOBBY_HEADERGUARD_SERVEREVENTS_H
#define SPRINGLOBBY_HEADERGUARD_SERVEREVENTS_H

#include "battle.h"

class Ui;
struct UserStatus;
struct UserBattleStatus;
class Server;

// FIXME this is defined elsewhere, should use a different kind of type so we could use forward decl
typedef int Sockerror;

typedef int Protocolerror;

//! @brief Class that implements server event behaviour.
class ServerEvents
{
  public:
    ServerEvents( Server& serv, Ui& ui ) : m_serv(serv),m_ui(ui) {}
    ~ServerEvents() {}

  // Uicontrol interface

    void OnConnected( const std::string& server_name, const std::string& server_ver, bool supported );
    void OnDisconnected();

    void OnLogin();
    void OnLoginInfoComplete();
    void OnLogout();

    void OnUnknownCommand( const std::string& command, const std::string& params );
    void OnSocketError( const Sockerror& error );
    void OnProtocolError( const Protocolerror error );
    void OnMotd( const std::string& msg );
    void OnPong( int ping_time );

    void OnNewUser( const std::string& nick, const std::string& conutry, int cpu );
    void OnUserStatus( const std::string& nick, UserStatus status );
    void OnUserQuit( const std::string& nick );

    void OnBattleOpened( int id, bool replay, NatType nat, const std::string& nick,
                         const std::string& host, int port, int maxplayers,
                         bool haspass, int rank, std::string maphash, const std::string& map,
                         const std::string& title, const std::string& mod );

    void OnUserJoinedBattle( int battleid, const std::string& nick );
    void OnUserLeftBattle( int battleid, const std::string& nick );
    void OnBattleInfoUpdated( int battleid, int metal, int energy, int units, StartType start, bool comm, bool dgun, bool dim, bool ghost, std::string hash );
    void OnBattleInfoUpdated( int battleid, int spectators, bool locked, std::string maphash, const std::string& map );
    void OnBattleClosed( int battleid );
    void OnJoinedBattle( int battleid );
    void OnClientBattleStatus( int battleid, const std::string& nick, UserBattleStatus status );
    void OnBattleStartRectAdd( int battleid, int allyno, int left, int top, int right, int bottom );
    void OnBattleStartRectRemove( int battleid, int allyno );

    void OnBattleDisableUnit( int battleid, const std::string& unitname );
    void OnBattleEnableUnit( int battleid, const std::string& unitname );
    void OnBattleEnableAllUnits( int battleid );

    void OnJoinChannelResult( bool success, const std::string& channel, const std::string& reason );
    void OnChannelSaid( const std::string& channel, const std::string& who, const std::string& message );
    void OnChannelJoin( const std::string& channel, const std::string& who );
    void OnChannelPart( const std::string& channel, const std::string& who, const std::string& message );
    void OnChannelTopic( const std::string& channel, const std::string& who, const std::string& message, int when );
    void OnChannelAction( const std::string& channel, const std::string& who, const std::string& action );
    void OnChannelList( const std::string& channel, const int& numusers );
    void OnPrivateMessage( const std::string& user, const std::string& message, bool fromme = false );

    void OnRequestBattleStatus( int battleid );
    void OnSaidBattle( int battleid, const std::string& nick, const std::string& msg );
    void OnBattleAction( int battleid, const std::string& nick, const std::string& msg );

    void OnAcceptAgreement( const std::string& agreement );

  protected:
    Server& m_serv;
    Ui& m_ui;
};

#endif // SPRINGLOBBY_HEADERGUARD_SERVEREVENTS_H
