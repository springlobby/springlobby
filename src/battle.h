#ifndef SPRINGLOBBY_HEADERGUARD_BATTLE_H
#define SPRINGLOBBY_HEADERGUARD_BATTLE_H

#include <set>

#include "autohost.h"
#include "ibattle.h"

class Ui;
class Server;
class User;


/** \brief model of a sp/mp battle
 * \todo DOCME */
class Battle : public IBattle
{
  public:
    Battle( Server& serv, int id );
    ~Battle();

    //const BattleOptions& opts() { return m_opts; }

    Server& GetServer() { return m_serv; }
    AutoHost& GetAutoHost() { return m_ah; }

    void SendHostInfo( HostInfo update );
    void SendHostInfo( const wxString& Tag );

    int GetMyPlayerNum() const;

    void FixColours( );

    void Update();
    void Update( const wxString& Tag );

    void Join( const wxString& password = _T("") );
    void Leave();

    void KickPlayer( User& user );

    void RingNotReadyPlayers();

    void Say( const wxString& msg );
    void DoAction( const wxString& msg );


    void OnRequestBattleStatus();
    void SendMyBattleStatus();

    bool ExecuteSayCommand( const wxString& cmd );

    void AddBot( const wxString& nick, UserBattleStatus status );

    void ForceSide( User& user, int side );
    void ForceTeam( User& user, int team );
    void ForceAlly( User& user, int ally );
    void ForceColour( User& user, const wxColour& col );
    void ForceSpectator( User& user, bool spectator );
    void BattleKickPlayer( User& user );
    void SetHandicap( User& user, int handicap);

    User& OnUserAdded( User& user );
    void OnUserBattleStatusUpdated( User &user, UserBattleStatus status );
    void OnUserRemoved( User& user );

    bool IsFounderMe();

    int GetMyPlayerNum();

    void Autobalance( BalanceType balance_type = balance_divide, bool clans = true, bool strong_clans = true, int allyteamsize = 0 );
    void FixTeamIDs( BalanceType balance_type = balance_divide, bool clans = true, bool strong_clans = true, int controlteamsize = 0 );
    void ForceUnsyncedToSpectate();

    void SetAutoLockOnStart( bool value );
    bool GetAutoLockOnStart();

    void SetIsProxy( bool value );
    bool IsProxy();

    void SetLockExternalBalanceChanges( bool value );
    bool GetLockExternalBalanceChanges();

    ///< quick hotfix for bans
    bool CheckBan(User &user);
    ///>

    void SetImReady( bool ready );

    void DisableHostStatusInProxyMode( bool value ) { m_generating_script = value; }

    User& GetMe();
    bool IsFounderMe() const;

    void UserPositionChanged( const User& user );

  protected:
    // Battle variables

    ///< quick hotfix for bans
    std::set<wxString> m_banned_users;
    std::set<wxString> m_banned_ips;
    ///>

    Server& m_serv;
    AutoHost m_ah;
    bool m_autolock_on_start;

    const int m_id;
};

#endif // SPRINGLOBBY_HEADERGUARD_BATTLE_H
