/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_USER_H
#define SPRINGLOBBY_HEADERGUARD_USER_H


/**
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

DO NOT CHANGE THIS FILE!

this file is deprecated and will be replaced with

lsl/user/user.h

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
**/


#include "utils/mixins.h"
#include <lslutils/misc.h>
#include <string>

class IServer;

const unsigned int SYNC_UNKNOWN = 0;
const unsigned int SYNC_SYNCED = 1;
const unsigned int SYNC_UNSYNCED = 2;

//! @brief Struct used to store a client's status.
struct UserStatus
{
	enum RankContainer {
		RANK_1,
		RANK_2,
		RANK_3,
		RANK_4,
		RANK_5,
		RANK_6,
		RANK_7,
		RANK_8
	};

	bool in_game;
	bool away;
	RankContainer rank;
	bool moderator;
	bool bot;
	UserStatus()
	    : in_game(false)
	    , away(false)
	    , rank(RANK_1)
	    , moderator(false)
	    , bot(false)
	{
	}
	std::string GetDiffString(const UserStatus& other) const;
};

struct UserPosition
{
	int x;
	int y;
	UserPosition()
	    : x(-1)
	    , y(-1)
	{
	}
};

struct UserBattleStatus
{
	//!!! when adding something to this struct, also modify User::UpdateBattleStatus() !!
	// total 17 members here
	int team;
	int ally;
	LSL::lslColor colour;
	int color_index;
	int handicap;
	int side;
	unsigned int sync;
	bool spectator;
	bool ready;
	bool isfromdemo;
	UserPosition pos; // for startpos = 4
	// bot-only stuff
	std::string owner;
	std::string aishortname;
	std::string airawname;
	std::string aiversion;
	int aitype;
	// for nat holepunching
	std::string ip;
	unsigned int udpport;
	std::string scriptPassword;
	bool IsBot() const
	{
		return !aishortname.empty();
	}
	UserBattleStatus()
	    : team(0)
	    , ally(0)
	    , colour(0, 0, 0)
	    , color_index(-1)
	    , handicap(0)
	    , side(0)
	    , sync(SYNC_UNKNOWN)
	    , spectator(false)
	    , ready(false)
	    , isfromdemo(false)
	    , aitype(-1)
	    , udpport(0)
	{
	}
	bool operator==(const UserBattleStatus& s) const
	{
		return ((team == s.team) && (colour == s.colour) && (handicap == s.handicap) && (side == s.side) && (sync == s.sync) && (spectator == s.spectator) && (ready == s.ready) && (owner == s.owner) && (aishortname == s.aishortname) && (isfromdemo == s.isfromdemo) && (aitype == s.aitype));
	}
	bool operator!=(const UserBattleStatus& s) const
	{
		return ((team != s.team) || (colour != s.colour) || (handicap != s.handicap) || (side != s.side) || (sync != s.sync) || (spectator != s.spectator) || (ready != s.ready) || (owner != s.owner) || (aishortname != s.aishortname) || (isfromdemo != s.isfromdemo) || (aitype != s.aitype));
	}
};

class ChatPanel;
class IBattle;

struct UiUserData
{
	UiUserData()
	    : panel(0)
	{
	}
	ChatPanel* panel;
};

//! parent class leaving out server related functionality
class CommonUser
{
public:
	CommonUser(const std::string& nick, const std::string& country, const int& cpu)
	    : m_nick(std::string(nick))
	    , m_country(std::string(country))
	    , m_cpu(cpu)
	{
	}

	virtual ~CommonUser()
	{
	}

	const std::string& GetNick() const
	{
		return m_nick;
	}
	virtual void SetNick(const std::string& nick)
	{
		m_nick = nick;
	}

	const std::string& GetCountry() const
	{
		return m_country;
	}
	virtual void SetCountry(const std::string& country)
	{
		m_country = country;
	}

	int GetCpu() const
	{
		return m_cpu;
	}
	void SetCpu(const int& cpu)
	{
		m_cpu = cpu;
	}

	int GetID() const
	{
		return m_id;
	}
	void SetID(int id)
	{
		m_id = id;
	}

	UserStatus& Status()
	{
		return m_status;
	}

	UserStatus GetStatus() const
	{
		return m_status;
	}
	virtual void SetStatus(const UserStatus& status);

	UserBattleStatus& BattleStatus()
	{
		return m_bstatus;
	}

	UserBattleStatus GetBattleStatus() const
	{
		return m_bstatus;
	}

	/** Read-only variant of BattleStatus() above.
	     */
	const UserBattleStatus&
	BattleStatus() const
	{
		return m_bstatus;
	}

	//void SetBattleStatus( const UserBattleStatus& status );/// dont use this to avoid overwriting data like ip and port, use following method.
	void UpdateBattleStatus(const UserBattleStatus& status);

	/*    void SetUserData( void* userdata ) { m_data = userdata; }
        void* GetUserData() { return m_data; }*/

	bool Equals(const CommonUser& other) const
	{
		return (m_nick == other.GetNick());
	}


private:
	std::string m_nick;
	std::string m_country;
	int m_id;
	int m_cpu;
	UserStatus m_status;
	UserBattleStatus m_bstatus;

	//void* m_data;
};

//! Class containing all the information about a user
class User : public CommonUser
{
public:
	mutable UiUserData uidata;

	User(IServer& serv);
	User(const std::string& nick, IServer& serv);
	User(const std::string& nick, const std::string& country, const int& cpu, IServer& serv);
	User(const std::string& nick);
	User(const std::string& nick, const std::string& country, const int& cpu);
	User();

	virtual ~User();

	// User interface

	IServer& GetServer() const
	{
		return *m_serv;
	}

	void Said(const std::string& message) const;
	void Say(const std::string& message) const;
	void DoAction(const std::string& message) const;

	IBattle* GetBattle() const;
	void SetBattle(IBattle* battle);

	void SendMyUserStatus() const;
	void SetStatus(const UserStatus& status);
	void SetCountry(const std::string& country);

	bool ExecuteSayCommand(const std::string& cmd) const;

	static std::string GetRankName(UserStatus::RankContainer rank);

	float GetBalanceRank();
	UserStatus::RankContainer GetRank();
	std::string GetClan();

	int GetFlagIconIndex() const
	{
		return m_flagicon_idx;
	}
	int GetRankIconIndex() const
	{
		return m_rankicon_idx;
	}
	int GetStatusIconIndex() const
	{
		return m_statusicon_idx;
	}
	//bool operator< ( const User& other ) const { return m_nick < other.GetNick() ; }
	//User& operator= ( const User& other );

	int GetSideiconIndex() const
	{
		return m_sideicon_idx;
	}
	void SetSideiconIndex(const int idx)
	{
		m_sideicon_idx = idx;
	}

private:
	// User variables

	IServer* m_serv;
	IBattle* m_battle;
	int m_flagicon_idx;
	int m_rankicon_idx;
	int m_statusicon_idx;
	int m_sideicon_idx;

	//! copy-semantics?
};

#endif // SPRINGLOBBY_HEADERGUARD_USER_H
