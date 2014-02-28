/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_BATTLE_H
#define SPRINGLOBBY_HEADERGUARD_BATTLE_H

#include <set>

#include "hosting/autohost.h"
#include "ibattle.h"

class Ui;
class IServer;
class User;
class wxTimerEvent;


/** \brief model of a sp/mp battle
 * \todo DOCME */
class Battle : public IBattle
{
  public:
    Battle( IServer& serv, int id );
    ~Battle();

    //const BattleOptions& opts() { return m_opts; }

    IServer& GetServer() { return m_serv; }
    AutoHost* GetAutoHost() { return &m_ah; }

    void SendHostInfo( HostInfo update );
    void SendHostInfo( const wxString& Tag );

    void Update( const wxString& Tag );

    void Join( const wxString& password = wxEmptyString );
    void Leave();

    void KickPlayer( User& user );

    void RingNotReadyPlayers();
    void RingNotSyncedPlayers();
    void RingNotSyncedAndNotReadyPlayers();
    void RingPlayer( const User& u );

    void Say( const wxString& msg );
    void DoAction( const wxString& msg );

    void SetLocalMap( const LSL::UnitsyncMap& map );

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

    void ForceUnsyncedToSpectate();
    void ForceUnReadyToSpectate();
    void ForceUnsyncedAndUnreadyToSpectate();

    void SetAutoLockOnStart( bool value );
    bool GetAutoLockOnStart();

    void SetLockExternalBalanceChanges( bool value );
    bool GetLockExternalBalanceChanges();

    void FixColours();
    void Autobalance( BalanceType balance_type = balance_divide, bool clans = true, bool strong_clans = true, int allyteamsize = 0 );
    void FixTeamIDs( BalanceType balance_type = balance_divide, bool clans = true, bool strong_clans = true, int controlteamsize = 0 );

    void SendScriptToClients();

    bool CheckBan(User &user);

    void SetImReady( bool ready );

    User& GetMe();
    const User& GetMe() const;

    void UserPositionChanged( const User& user );

	int GetID() const { return m_id; }

    void SaveMapDefaults();
    void LoadMapDefaults( const wxString& mapname );

		void StartHostedBattle();
    void StartSpring();

    void OnTimer( wxTimerEvent& /*event*/ );

	void SetInGame( bool ingame );

	void OnUnitsyncReloaded( wxEvent& data );


	void SetAutoUnspec(bool value);
	bool GetAutoUnspec() { return m_auto_unspec; }
private:
	void ShouldAutoUnspec();

  protected:
    // Battle variables

    ///< quick hotfix for bans
    std::set<wxString> m_banned_users;
    std::set<wxString> m_banned_ips;
    ///>

    IServer& m_serv;
    AutoHost m_ah;
    bool m_autolock_on_start;

    const int m_id;

		DECLARE_EVENT_TABLE()
};

#endif // SPRINGLOBBY_HEADERGUARD_BATTLE_H
