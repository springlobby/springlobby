/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_BATTLE_H
#define SPRINGLOBBY_HEADERGUARD_BATTLE_H

#include <set>
#include <wx/event.h>

#include "autohost.h"
#include "ibattle.h"

class IServer;
class User;
class wxTimerEvent;
class wxEvtHandler;
class wxTimer;


/** \brief model of a sp/mp battle
 * \todo DOCME */
class Battle : public IBattle, public wxEvtHandler
{
public:
	Battle(IServer& serv, int id);
	~Battle();

	//const BattleOptions& opts() { return m_opts; }

	IServer& GetServer()
	{
		return m_serv;
	}
	AutoHost* GetAutoHost()
	{
		return &m_ah;
	}

	virtual void OnPlayerTrueskillChanged(const std::string& NickName, double TrueSkill) override;

	virtual void SendHostInfo(HostInfo update);
	virtual void SendHostInfo(const std::string& Tag);

	virtual void Update(const std::string& Tag);

	virtual void Join(const std::string& password = "");
	virtual void Leave();

	virtual void KickPlayer(User& user);

	virtual void RingNotReadyPlayers();
	virtual void RingNotSyncedPlayers();
	virtual void RingNotSyncedAndNotReadyPlayers();
	virtual void RingPlayer(const User& u);

	virtual void Say(const std::string& msg);
	virtual void DoAction(const std::string& msg);

	virtual void SetLocalMap(const std::string& mapname);

	virtual void OnRequestBattleStatus();
	virtual void SendMyBattleStatus();

	virtual bool ExecuteSayCommand(const std::string& cmd);

	virtual void AddBot(const std::string& nick, UserBattleStatus status);

	virtual void ForceSide(User& user, int side);
	virtual void ForceTeam(User& user, int team);
	virtual void ForceAlly(User& user, int ally);
	virtual void ForceColour(User& user, const LSL::lslColor& col);
	virtual void ForceSpectator(User& user, bool spectator);
	//    virtual void BattleKickPlayer( User& user );
	virtual void SetHandicap(User& user, int handicap);

	virtual User& OnUserAdded(User& user);
	virtual void OnUserBattleStatusUpdated(User& user, UserBattleStatus status);
	virtual void OnUserRemoved(User& user);

	virtual void ForceUnsyncedToSpectate();
	virtual void ForceUnReadyToSpectate();
	virtual void ForceUnsyncedAndUnreadyToSpectate();

	virtual void SetAutoLockOnStart(bool value);
	virtual bool GetAutoLockOnStart();

	virtual void SetLockExternalBalanceChanges(bool value);
	virtual bool GetLockExternalBalanceChanges();

	virtual void FixColours();
	virtual void Autobalance(BalanceType balance_type = balance_divide, bool clans = true, bool strong_clans = true, int allyteamsize = 0);
	virtual void FixTeamIDs(BalanceType balance_type = balance_divide, bool clans = true, bool strong_clans = true, int controlteamsize = 0);

	//	virtual void SendScriptToClients();

	virtual bool CheckBan(User& user);

	virtual void SetImReady(bool ready);

	virtual User& GetMe();
	virtual const User& GetMe() const;

	virtual void UserPositionChanged(const User& user);

	virtual int GetID() const
	{
		return m_id;
	}

	virtual void SaveMapDefaults();
	virtual void LoadMapDefaults(const std::string& mapname);

	virtual void StartHostedBattle();
	virtual void StartSpring();

	virtual void OnTimer(wxTimerEvent& /*event*/);

	virtual void SetInGame(bool ingame);

	virtual void OnUnitsyncReloaded(wxEvent& data);


	virtual void SetAutoUnspec(bool value);
	virtual bool GetAutoUnspec()
	{
		return m_auto_unspec;
	}

private:
	void ShouldAutoUnspec();
	bool m_auto_unspec;			// unspec as soon as there's a free slot
	unsigned int m_auto_unspec_num_players; // number of players in the battle when m_auto_unspec was set to true

	// Battle variables

	///< quick hotfix for bans
	std::set<std::string> m_banned_users;
	std::set<std::string> m_banned_ips;
	///>

	IServer& m_serv;
	AutoHost m_ah;
	bool m_autolock_on_start;

	const int m_id;
	wxTimer* m_timer;

	DECLARE_EVENT_TABLE()
};

#endif // SPRINGLOBBY_HEADERGUARD_BATTLE_H
