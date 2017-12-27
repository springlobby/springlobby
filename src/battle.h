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

	IServer& GetServer() override
	{
		return m_serv;
	}
	AutoHost* GetAutoHost() override
	{
		return &m_ah;
	}

	virtual void OnPlayerTrueskillChanged(const std::string& NickName, double TrueSkill) override;

	virtual void SendHostInfo(HostInfo update) override;
	virtual void SendHostInfo(const std::string& Tag) override;

	virtual void Join(const std::string& password = "") override;
	virtual void Leave() override;

	virtual void KickPlayer(User& user) override;

	virtual void RingNotReadyPlayers() override;
	virtual void RingNotSyncedPlayers() override;
	virtual void RingNotSyncedAndNotReadyPlayers() override;
	virtual void RingPlayer(const User& u) override;

	virtual void Say(const std::string& msg) override;
	virtual void DoAction(const std::string& msg) override;

	virtual void SetLocalMap(const std::string& mapname) override;

	virtual void OnRequestBattleStatus() override;
	virtual void SendMyBattleStatus() override;

	virtual bool ExecuteSayCommand(const std::string& cmd) override;

	virtual void AddBot(const std::string& nick, UserBattleStatus status);

	virtual void ForceSide(User& user, int side) override;
	virtual void ForceTeam(User& user, int team) override;
	virtual void ForceAlly(User& user, int ally) override;
	virtual void ForceColour(User& user, const LSL::lslColor& col) override;
	virtual void ForceSpectator(User& user, bool spectator) override;
	//    virtual void BattleKickPlayer( User& user );
	virtual void SetHandicap(User& user, int handicap) override;

	virtual User& OnUserAdded(User& user) override;
	virtual void OnUserBattleStatusUpdated(User& user, UserBattleStatus status) override;
	virtual void OnUserRemoved(User& user) override;

	virtual void ForceUnsyncedToSpectate() override;
	virtual void ForceUnReadyToSpectate() override;
	virtual void ForceUnsyncedAndUnreadyToSpectate() override;

	virtual void SetAutoLockOnStart(bool value) override;
	virtual bool GetAutoLockOnStart() override;

	virtual void SetLockExternalBalanceChanges(bool value) override;
	virtual bool GetLockExternalBalanceChanges();

	virtual void FixColours() override;
	virtual void Autobalance(BalanceType balance_type = balance_divide, bool clans = true, bool strong_clans = true, int allyteamsize = 0) override;
	virtual void FixTeamIDs(BalanceType balance_type = balance_divide, bool clans = true, bool strong_clans = true, int controlteamsize = 0) override;

	//	virtual void SendScriptToClients();

	virtual bool CheckBan(User& user) override;

	virtual void SetImReady(bool ready) override;
	virtual void SetAutolaunchGame(bool autolaunch) override;

	virtual User& GetMe() override;
	virtual const User& GetMe() const override;

	virtual void UserPositionChanged(const User& user) override;

	virtual int GetID() const override
	{
		return m_id;
	}

	virtual void SaveMapDefaults() override;
	virtual void LoadMapDefaults(const std::string& mapname) override;

	virtual void StartHostedBattle() override;
	virtual void StartSpring() override;

	virtual void OnTimer(wxTimerEvent& /*event*/);

	virtual void SetInGame(bool ingame) override;

	virtual void OnUnitsyncReloaded(wxEvent& data);


	virtual void SetAutoUnspec(bool value) override;
	virtual bool GetAutoUnspec() override
	{
		return m_auto_unspec;
	}

private:
	void ShouldAutoUnspec();
	bool m_auto_unspec;			// unspec as soon as there's a free slot
	unsigned int m_auto_unspec_num_players; // number of players in the battle when m_auto_unspec was set to true

	bool m_autolaunch_game; // whether to automatically launch game (when spectating)
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
