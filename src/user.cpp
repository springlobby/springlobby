/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

/**
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

TODO: remove all wx stuff & merge / move to

lsl/user/user.cpp

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
**/

//
// Class: User
//

#include "ibattle.h"
#include "user.h"
#include "iserver.h"
#include "gui/chatpanel.h"
#include "iconimagelist.h"
#include "utils/conversion.h"

#include <wx/intl.h>

User::User(IServer& serv)
    : CommonUser("", "", 0)
    , m_serv(&serv)
    , m_battle(0)
    , m_flagicon_idx(icons().GetFlagIcon(""))
    , m_rankicon_idx(icons().GetRankIcon(0))
    , m_statusicon_idx(icons().GetUserListStateIcon(GetStatus(), false, false))
    , m_sideicon_idx(icons().ICON_NONE)
{
}

User::User(const std::string& nick, IServer& serv)
    : CommonUser(nick, "", 0)
    , m_serv(&serv)
    , m_battle(0)
    , m_flagicon_idx(icons().GetFlagIcon(""))
    , m_rankicon_idx(icons().GetRankIcon(0))
    , m_statusicon_idx(icons().GetUserListStateIcon(GetStatus(), false, false))
    , m_sideicon_idx(icons().ICON_NONE)
{
}

User::User(const std::string& nick, const std::string& country, const int& cpu, IServer& serv)
    : CommonUser(nick, country, cpu)
    , m_serv(&serv)
    , m_battle(0)
    , m_flagicon_idx(icons().GetFlagIcon(country))
    , m_rankicon_idx(icons().GetRankIcon(0))
    , m_statusicon_idx(icons().GetUserListStateIcon(GetStatus(), false, false))
    , m_sideicon_idx(icons().ICON_NONE)
{
}

User::User(const std::string& nick)
    : CommonUser(nick, "", 0)
    , m_serv(0)
    , m_battle(0)
    , m_flagicon_idx(icons().GetFlagIcon(""))
    , m_rankicon_idx(icons().GetRankIcon(0))
    , m_statusicon_idx(icons().GetUserListStateIcon(GetStatus(), false, false))
    , m_sideicon_idx(icons().ICON_NONE)
{
}

User::User(const std::string& nick, const std::string& country, const int& cpu)
    : CommonUser(nick, country, cpu)
    , m_serv(0)
    , m_battle(0)
    , m_flagicon_idx(icons().GetFlagIcon(country))
    , m_rankicon_idx(icons().GetRankIcon(0))
    , m_statusicon_idx(icons().GetUserListStateIcon(GetStatus(), false, false))
    , m_sideicon_idx(icons().ICON_NONE)
{
}

User::User()
    : CommonUser("", "", 0)
    , m_serv(0)
    , m_battle(0)
    , m_flagicon_idx(icons().GetFlagIcon(""))
    , m_rankicon_idx(icons().GetRankIcon(0))
    , m_statusicon_idx(icons().GetUserListStateIcon(GetStatus(), false, false))
    , m_sideicon_idx(icons().ICON_NONE)
{
}

User::~User()
{
	if (uidata.panel)
		uidata.panel->SetUser(0);
}

std::string UserStatus::GetDiffString(const UserStatus& old) const
{
	if (old.away != away)
		return (away ? "away" : "back");
	if (old.in_game != in_game)
		return (in_game ? "ingame" : "back from game");
	return "";
}

void User::Said(const std::string& /*message*/) const
{
}


void User::Say(const std::string& message) const
{
	GetServer().SayPrivate(GetNick(), message);
}


void User::DoAction(const std::string& message) const
{
	GetServer().DoActionPrivate(GetNick(), message);
}


IBattle* User::GetBattle() const
{
	return m_battle;
}


void User::SetBattle(IBattle* battle)
{
	m_battle = battle;
	m_statusicon_idx = icons().GetUserListStateIcon(GetStatus(), false, m_battle != 0);
}

void User::SetStatus(const UserStatus& status)
{
	CommonUser::SetStatus(status);
	// If user is host of a game, then his in_game status tells if the game is on!
	if (m_battle != 0) {
		try {
			User& user = m_battle->GetFounder();
			if (user.GetNick() == GetNick()) {
				m_battle->Update("");
			}
		} catch (...) {
		}
	}

	m_statusicon_idx = icons().GetUserListStateIcon(GetStatus(), false, m_battle != 0);
	m_rankicon_idx = icons().GetRankIcon(GetStatus().rank);
}

void User::SetCountry(const std::string& country)
{
	CommonUser::SetCountry(country);
	m_flagicon_idx = icons().GetFlagIcon(country);
}

void CommonUser::UpdateBattleStatus(const UserBattleStatus& status)
{

	// total 17 members to update.

	m_bstatus.team = status.team;
	m_bstatus.ally = status.ally;
	m_bstatus.colour = status.colour;
	m_bstatus.color_index = status.color_index;
	m_bstatus.handicap = status.handicap;
	m_bstatus.side = status.side;
	m_bstatus.sync = status.sync;
	m_bstatus.spectator = status.spectator;
	m_bstatus.ready = status.ready;
	if (!status.aishortname.empty())
		m_bstatus.aishortname = status.aishortname;
	if (!status.airawname.empty())
		m_bstatus.airawname = status.airawname;
	if (!status.aiversion.empty())
		m_bstatus.aiversion = status.aiversion;
	if (!(status.aitype > 0))
		m_bstatus.aitype = status.aitype;
	if (!status.owner.empty())
		m_bstatus.owner = status.owner;
	if (status.pos.x > 0)
		m_bstatus.pos.x = status.pos.x;
	if (status.pos.y > 0)
		m_bstatus.pos.y = status.pos.y;

	// update ip and port if those were set.
	if (!status.ip.empty())
		m_bstatus.ip = status.ip;
	if (status.udpport != 0)
		m_bstatus.udpport = status.udpport; // 15
}


void User::SendMyUserStatus() const
{
	GetServer().SendMyUserStatus(GetStatus());
}


bool User::ExecuteSayCommand(const std::string& cmd) const
{
	if (TowxString(cmd).BeforeFirst(' ').Lower() == _T("/me")) {
		GetServer().DoActionPrivate(GetNick(), LSL::Util::AfterFirst(cmd, " "));
		return true;
	}
	return false;
}

UserStatus::RankContainer User::GetRank()
{
	return GetStatus().rank;
}

std::string User::GetRankName(UserStatus::RankContainer rank)
{
	//TODO: better interface to ranks?
	switch (rank) {
		case UserStatus::RANK_1:
			return "Newbie";
		case UserStatus::RANK_2:
			return "Beginner";
		case UserStatus::RANK_3:
			return "Average";
		case UserStatus::RANK_4:
			return "Above average";
		case UserStatus::RANK_5:
			return "Experienced";
		case UserStatus::RANK_6:
			return "Highly experienced";
		case UserStatus::RANK_7:
			return "Veteran";
		case UserStatus::RANK_8:
			return "Badly needs to get laid";
		default:
			return "Unknown";
	}
}

float User::GetBalanceRank()
{
	return 1.0 + 0.1 * float(GetStatus().rank - UserStatus::RANK_1) / float(UserStatus::RANK_8 - UserStatus::RANK_1);
}

std::string User::GetClan()
{
	const std::string tmp = LSL::Util::AfterFirst(GetNick(), "[");
	if (tmp != GetNick()) {
		const std::string clan = LSL::Util::BeforeFirst(tmp, "]");
		if (clan != tmp)
			return clan;
	}
	return "";
}

void CommonUser::SetStatus(const UserStatus& status)
{
	m_status = status;
}

//User& User::operator= ( const User& other )
//{
//    if( this != &other ) {
//        //m_serv = (other.GetServer());
//        m_status = other.GetStatus();
//        m_battle = other.GetBattle();
//        m_nick = other.GetNick();
//        m_cpu = other.GetCpu();
//        m_country = other.GetCountry();
//        m_bstatus = other.GetBattleStatus();
//        uidata = other.uidata;
//
//
//    }
//    return *this;
//}
