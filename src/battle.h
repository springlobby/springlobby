#ifndef SPRINGLOBBY_HEADERGUARD_BATTLE_H
#define SPRINGLOBBY_HEADERGUARD_BATTLE_H

#include <set>

#include "autohost.h"
#include "ibattle.h"

class Ui;
class Server;
class User;
class wxTimerEvent;


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

    void Update();
    void Update( const wxString& Tag );

    void Join( const wxString& password = _T("") );
    void Leave();

    void KickPlayer( User& user );

    void RingNotReadyPlayers();
    void RingNotSyncedPlayers();
    void RingNotSyncedAndNotReadyPlayers();
    void RingPlayer( const User& u );

    void Say( const wxString& msg );
    void DoAction( const wxString& msg );

    void SetLocalMap( const UnitSyncMap& map );

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

    ///< quick hotfix for bans
    bool CheckBan(User &user);
    ///>

    void SetImReady( bool ready );

    User& GetMe();
    const User& GetMe() const;

    void UserPositionChanged( const User& user );

    int GetID() { return m_id; }

    void SaveMapDefaults();
    void LoadMapDefaults( const wxString& mapname );

		void StartHostedBattle();
    void StartSpring();

    void OnTimer( wxTimerEvent& /*event*/ );

	void SetInGame( bool ingame );

	void OnUnitsyncReloaded( GlobalEvents::GlobalEventData data );

	void SetAutoUnspec(bool value);
	bool GetAutoUnspec() { return m_auto_unspec; }

  void ShouldAutoUnspec();

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

		DECLARE_EVENT_TABLE()
};

#endif // SPRINGLOBBY_HEADERGUARD_BATTLE_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2011

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    SpringLobby is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

