/* This file is part of the Springlobby (GPL v2 or later), see COPYING */


/**
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

TODO: remove all wx stuff & merge / move to

lsl/battle/ibattle.cpp

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
**/
#include "ibattle.h"

#include <lslunitsync/unitsync.h>
#include <lslutils/globalsmanager.h>
#include <wx/filename.h>
#include <wx/log.h>
#include <wx/string.h>
#include <wx/tokenzr.h>
#include <algorithm>

#include "gui/ui.h"
#include "gui/uiutils.h"
#include "iserver.h"
#include "log.h"
#include "serverselector.h"
#include "settings.h"
#include "spring.h"
#include "springlobbyapp.h"
#include "utils/conversion.h"
#include "utils/lslconversion.h"
#include "utils/slpaths.h"

#define MAX_TEAMS 255


IBattle::IBattle()
    : m_autohost_manager(NULL)
    , m_generating_script(false)
    , m_is_self_in(false)
    , m_ingame(false)
    , m_map_loaded(false)
    , m_game_loaded(false)
    , m_players_ready(0)
    , m_players_sync(0)
    , m_players_ok(0)
    , m_start_time(0)
{
}


IBattle::~IBattle()
{
	try {
		if (m_is_self_in)
			LSL::usync().UnSetCurrentArchive();
	} catch (const LSL::Util::GlobalDestroyedError& err) {
		/* If this is called during TAServer::~TAServer, then unitsync might be unavailable. */
	}
	SetGUIListActiv(false);
}

bool IBattle::IsSynced()
{
	LoadGame();
	LoadMap();
	if (!m_host_game.name.empty() && m_local_game.name != m_host_game.name) {
		wxLogWarning("Not synced: game name doesn't match host:'%s' local:'%s'", m_host_game.name.c_str(), m_local_game.name.c_str());
		return false;
	}
	if (!m_host_map.name.empty() && m_local_map.name != m_host_map.name) {
		wxLogWarning("Not synced: map name doesn't match host:'%s' local:'%s'", m_host_map.name.c_str(), m_local_map.name.c_str());
		return false;
	}
	if (!m_host_game.hash.empty() && m_host_game.hash != "0" && m_host_game.hash != m_local_game.hash) {
		wxLogWarning("Not synced: game hash doesn't match host:'%s' local:'%s'", m_host_game.hash.c_str(), m_local_game.hash.c_str());
		return false;
	}
	if (!m_host_map.hash.empty() && m_host_map.hash != "0" && m_host_map.hash != m_local_map.hash) {
		wxLogWarning("Not synced: map hash doesn't match host:'%s' local:'%s'", m_host_map.hash.c_str(), m_local_map.hash.c_str());
		return false;
	}
	if (!GameExists()) {
		wxLogWarning("Not synced: game doesn't exist: %s %s", m_host_game.name.c_str());
		return false;
	}
	if (!MapExists()) {
		wxLogWarning("Not synced: map doesn't exist: %s %s", m_host_map.name.c_str());
		return false;
	}
	return true;
}

const std::vector<LSL::lslColor>& IBattle::GetFixColoursPalette(int numteams) const
{
	return GetBigFixColoursPalette(numteams);
}

LSL::lslColor IBattle::GetFixColour(int i) const
{
	int size = m_teams_sizes.size();
	const std::vector<LSL::lslColor>& palette = GetFixColoursPalette(size);
	return palette[i];
}

class DismissColor
{
private:
	typedef std::vector<LSL::lslColor>
	    ColorVec;
	const ColorVec& m_other;

public:
	DismissColor(const ColorVec& other)
	    : m_other(other)
	{
	}

	bool operator()(LSL::lslColor to_check)
	{
		return std::find(m_other.begin(), m_other.end(), to_check) != m_other.end();
	}
};

class AreColoursSimilarProxy
{
	int m_mindiff;

public:
	AreColoursSimilarProxy(int mindiff)
	    : m_mindiff(mindiff)
	{
	}

	bool operator()(LSL::lslColor a, LSL::lslColor b)
	{
		return AreColoursSimilar(a, b, m_mindiff);
	}
};

LSL::lslColor IBattle::GetFreeColour(User*) const
{
	typedef std::vector<LSL::lslColor>
	    ColorVec;

	ColorVec current_used_colors;
	for (user_map_t::size_type i = 0; i < GetNumUsers(); ++i) {
		UserBattleStatus& bs = GetUser(i).BattleStatus();
		current_used_colors.push_back(bs.colour);
	}

	int inc = 1;
	while (true) {
		ColorVec fixcolourspalette(GetFixColoursPalette(m_teams_sizes.size() + inc++));

		ColorVec::iterator fixcolourspalette_new_end = std::unique(fixcolourspalette.begin(), fixcolourspalette.end(), AreColoursSimilarProxy(20));

		fixcolourspalette_new_end = std::remove_if(fixcolourspalette.begin(), fixcolourspalette.end(), DismissColor(current_used_colors));

		if (fixcolourspalette_new_end != fixcolourspalette.begin())
			return (*fixcolourspalette.begin());
	}
}

LSL::lslColor IBattle::GetFreeColour(User& for_whom) const
{
	return GetFreeColour(&for_whom);
}


LSL::lslColor IBattle::GetNewColour() const
{
	return GetFreeColour();
}

int IBattle::ColourDifference(const LSL::lslColor& a, const LSL::lslColor& b) const // returns max difference of r,g,b.
{
	return std::max(abs(a.Red() - b.Red()), std::max(abs(a.Green() - b.Green()), abs(a.Blue() - b.Blue())));
}

int IBattle::GetFreeTeam(bool excludeme) const
{
	int lowest = 0;
	bool changed = true;
	while (changed) {
		changed = false;
		for (user_map_t::size_type i = 0; i < GetNumUsers(); i++) {
			User& user = GetUser(i);
			if ((&user == &GetMe()) && excludeme)
				continue;
			if (user.BattleStatus().spectator)
				continue;
			if (user.BattleStatus().team == lowest) {
				lowest++;
				changed = true;
			}
		}
	}
	return lowest;
}

int IBattle::GetClosestFixColour(const LSL::lslColor& col, const std::vector<int>& excludes, int difference) const
{
	const std::vector<LSL::lslColor>& palette = GetFixColoursPalette(m_teams_sizes.size() + 1);
	int result = 0;
	int t1 = palette.size();
	int t2 = excludes.size();
	wxLogMessage(_T("GetClosestFixColour %d %d"), t1, t2);
	for (size_t i = 0; i < palette.size(); ++i) {
		if ((i >= excludes.size()) || (!excludes[i])) {
			if (AreColoursSimilar(palette[i], col, difference)) {
				return i;
			}
		}
	}
	return result;
}

User& IBattle::OnUserAdded(User& user)
{
	UserList::AddUser(user);
	UserBattleStatus& bs = user.BattleStatus();
	bs.spectator = false;
	bs.ready = false;
	bs.sync = SYNC_UNKNOWN;
	if (!bs.IsBot() && IsFounderMe() && GetBattleType() == BT_Played) {
		bs.team = GetFreeTeam(&user == &GetMe());
		bs.ally = GetFreeAlly(&user == &GetMe());
		bs.colour = GetFreeColour(user);
	}
	if (IsFounderMe() && ((bs.pos.x < 0) || (bs.pos.y < 0))) {
		UserPosition& pos = bs.pos;
		pos = GetFreePosition();
		UserPositionChanged(user);
	}
	if (!bs.spectator) {
		PlayerJoinedAlly(bs.ally);
		PlayerJoinedTeam(bs.team);
	}
	if (bs.spectator && IsFounderMe())
		m_opts.spectators++;
	if (!bs.spectator && !bs.IsBot()) {
		if (bs.ready)
			m_players_ready++;
		if (bs.sync)
			m_players_sync++;
		if (!bs.ready || !bs.sync)
			m_ready_up_map[user.GetNick()] = time(0);
		if (bs.ready && bs.sync)
			m_players_ok++;
	}
	return user;
}

User& IBattle::OnBotAdded(const std::string& nick, const UserBattleStatus& bs)
{
	m_internal_bot_list[nick] = User(nick);
	User& user = m_internal_bot_list[nick];
	user.UpdateBattleStatus(bs);
	User& usr = OnUserAdded(user);
	return usr;
}

unsigned int IBattle::GetNumBots() const
{
	return m_internal_bot_list.size();
}

unsigned int IBattle::GetNumPlayers() const
{
	return GetNumUsers() - GetNumBots();
}

unsigned int IBattle::GetNumActivePlayers() const
{
	return GetNumPlayers() - m_opts.spectators;
}

void IBattle::OnUserBattleStatusUpdated(User& user, UserBattleStatus status)
{

	UserBattleStatus previousstatus = user.BattleStatus();

	user.UpdateBattleStatus(status);
	unsigned int oldspeccount = m_opts.spectators;
	m_opts.spectators = 0;
	m_players_sync = 0;
	m_players_ready = 0;
	m_players_ok = 0;
	m_teams_sizes.clear();
	m_ally_sizes.clear();
	for (unsigned int i = 0; i < GetNumUsers(); i++) {
		User& loopuser = GetUser(i);
		UserBattleStatus& loopstatus = loopuser.BattleStatus();
		if (loopstatus.spectator)
			m_opts.spectators++;
		else {
			PlayerJoinedTeam(loopstatus.team);
			PlayerJoinedAlly(loopstatus.ally);
			if (!loopstatus.IsBot()) {
				if (loopstatus.ready && loopstatus.spectator)
					m_players_ready++;
				if (loopstatus.sync)
					m_players_sync++;
				if (loopstatus.ready && loopstatus.sync)
					m_players_ok++;
			}
		}
	}
	if (oldspeccount != m_opts.spectators) {
		if (IsFounderMe())
			SendHostInfo(HI_Spectators);
	}
	if (!status.IsBot()) {
		if ((status.ready && status.sync) || status.spectator) {
			std::map<std::string, time_t>::iterator itor = m_ready_up_map.find(user.GetNick());
			if (itor != m_ready_up_map.end()) {
				m_ready_up_map.erase(itor);
			}
		}
		if ((!status.ready || !status.sync) && !status.spectator) {
			std::map<std::string, time_t>::iterator itor = m_ready_up_map.find(user.GetNick());
			if (itor == m_ready_up_map.end()) {
				m_ready_up_map[user.GetNick()] = time(0);
			}
		}
	}
}

bool IBattle::ShouldAutoStart() const
{
	if (GetInGame())
		return false;
	if (!IsLocked() && (GetNumActivePlayers() < m_opts.maxplayers))
		return false; // proceed checking for ready & sync players only if the battle is full or locked
	if (!IsEveryoneReady())
		return false;
	return true;
}

void IBattle::OnUserRemoved(User& user)
{
	UserBattleStatus& bs = user.BattleStatus();
	if (!bs.spectator) {
		PlayerLeftTeam(bs.team);
		PlayerLeftAlly(bs.ally);
	}
	if (!bs.spectator && !bs.IsBot()) {
		if (bs.ready)
			m_players_ready--;
		if (bs.sync)
			m_players_sync--;
		if (bs.ready && bs.sync)
			m_players_ok--;
	}
	if (IsFounderMe() && bs.spectator) {
		m_opts.spectators--;
		SendHostInfo(HI_Spectators);
	}
	if (&user == &GetMe()) {
		OnSelfLeftBattle();
	}
	UserList::RemoveUser(user.GetNick());
	if (!bs.IsBot())
		user.SetBattle(0);
	else {
		UserVecIter itor = m_internal_bot_list.find(user.GetNick());
		if (itor != m_internal_bot_list.end()) {
			m_internal_bot_list.erase(itor);
		}
	}
}


bool IBattle::IsEveryoneReady() const
{
	for (unsigned int i = 0; i < GetNumPlayers(); i++) {
		User& usr = GetUser(i);
		UserBattleStatus& status = usr.BattleStatus();
		if (status.IsBot())
			continue;
		if (status.spectator)
			continue;
		if (&usr == &GetMe())
			continue;
		if (!status.ready)
			return false;
		if (!status.sync)
			return false;
	}
	return true;
}


void IBattle::AddStartRect(unsigned int allyno, unsigned int left, unsigned int top, unsigned int right, unsigned int bottom)
{
	assert(allyno < MAX_TEAMS);

	BattleStartRect sr;

	sr.ally = allyno;
	sr.left = left;
	sr.top = top;
	sr.right = right;
	sr.bottom = bottom;
	sr.toadd = true;
	sr.todelete = false;
	sr.toresize = false;
	sr.exist = true;

	m_rects[allyno] = sr;
}


void IBattle::RemoveStartRect(unsigned int allyno)
{
	assert(allyno < MAX_TEAMS);
	std::map<unsigned int, BattleStartRect>::iterator rect_it = m_rects.find(allyno);
	if (rect_it == m_rects.end())
		return;

	rect_it->second.todelete = true;
}


void IBattle::ResizeStartRect(unsigned int allyno)
{
	assert(allyno < MAX_TEAMS);
	std::map<unsigned int, BattleStartRect>::iterator rect_it = m_rects.find(allyno);
	if (rect_it == m_rects.end())
		return;

	rect_it->second.toresize = true;
}


void IBattle::StartRectRemoved(unsigned int allyno)
{
	assert(allyno < MAX_TEAMS);
	std::map<unsigned int, BattleStartRect>::const_iterator rect_it = m_rects.find(allyno);
	if (rect_it == m_rects.end())
		return;

	if (rect_it->second.todelete)
		m_rects.erase(allyno);
}


void IBattle::StartRectResized(unsigned int allyno)
{
	assert(allyno < MAX_TEAMS);
	std::map<unsigned int, BattleStartRect>::iterator rect_it = m_rects.find(allyno);
	if (rect_it == m_rects.end())
		return;

	rect_it->second.toresize = false;
}


void IBattle::StartRectAdded(unsigned int allyno)
{
	assert(allyno < MAX_TEAMS);
	std::map<unsigned int, BattleStartRect>::iterator rect_it = m_rects.find(allyno);
	if (rect_it == m_rects.end())
		return;

	rect_it->second.toadd = false;
}


BattleStartRect IBattle::GetStartRect(unsigned int allyno) const
{
	assert(allyno < MAX_TEAMS);
	std::map<unsigned int, BattleStartRect>::const_iterator rect_it = m_rects.find(allyno);
	if (rect_it != m_rects.end())
		return (*rect_it).second;
	return BattleStartRect();
}

//total number of start rects
unsigned int IBattle::GetNumRects() const
{
	return m_rects.size();
}

//key of last start rect in the map
unsigned int IBattle::GetLastRectIdx() const
{
	if (GetNumRects() > 0) {
		const unsigned int res = m_rects.rbegin()->first;
		assert(res < MAX_TEAMS);
		return res;
	}

	return 0;
}

//return  the lowest currently unused key in the map of rects.
unsigned int IBattle::GetNextFreeRectIdx() const
{
	//check for unused allyno keys
	for (unsigned int i = 0; i <= GetNumRects(); i++) {
		if (!GetStartRect(i).IsOk()) {
			assert(i < MAX_TEAMS);
			return i;
		}
	}
	return 1; //rects start at 1
}

void IBattle::ClearStartRects()
{
	m_rects.clear();
}

void IBattle::ForceSide(User& user, int side)
{
	if (IsFounderMe() || user.BattleStatus().IsBot()) {
		user.BattleStatus().side = side;
	}
}

void IBattle::ForceTeam(User& user, int team)
{
	if (IsFounderMe() || user.BattleStatus().IsBot()) {
		if (!user.BattleStatus().spectator) {
			PlayerLeftTeam(user.BattleStatus().team);
			PlayerJoinedTeam(team);
		}
		user.BattleStatus().team = team;
	}
}


void IBattle::ForceAlly(User& user, int ally)
{

	if (IsFounderMe() || user.BattleStatus().IsBot()) {
		if (!user.BattleStatus().spectator) {
			PlayerLeftAlly(user.BattleStatus().ally);
			PlayerJoinedAlly(ally);
		}
		user.BattleStatus().ally = ally;
	}
}


void IBattle::ForceColour(User& user, const LSL::lslColor& col)
{
	if (IsFounderMe() || user.BattleStatus().IsBot()) {
		user.BattleStatus().colour = col;
	}
}

void IBattle::PlayerJoinedTeam(int team)
{
	std::map<int, int>::const_iterator itor = m_teams_sizes.find(team);
	if (itor == m_teams_sizes.end())
		m_teams_sizes[team] = 1;
	else
		m_teams_sizes[team] = m_teams_sizes[team] + 1;
}

void IBattle::PlayerJoinedAlly(int ally)
{
	std::map<int, int>::const_iterator iter = m_ally_sizes.find(ally);
	if (iter == m_ally_sizes.end())
		m_ally_sizes[ally] = 1;
	else
		m_ally_sizes[ally] = m_ally_sizes[ally] + 1;
}

void IBattle::PlayerLeftTeam(int team)
{
	std::map<int, int>::iterator itor = m_teams_sizes.find(team);
	if (itor != m_teams_sizes.end()) {
		itor->second = itor->second - 1;
		if (itor->second == 0) {
			m_teams_sizes.erase(itor);
		}
	}
}

void IBattle::PlayerLeftAlly(int ally)
{
	std::map<int, int>::iterator iter = m_ally_sizes.find(ally);
	if (iter != m_ally_sizes.end()) {
		iter->second = iter->second - 1;
		if (iter->second == 0) {
			m_ally_sizes.erase(iter);
		}
	}
}

void IBattle::ForceSpectator(User& user, bool spectator)
{
	if (IsFounderMe() || user.BattleStatus().IsBot()) {
		UserBattleStatus& status = user.BattleStatus();

		if (!status.spectator) { // leaving spectator status
			PlayerJoinedTeam(status.team);
			PlayerJoinedAlly(status.ally);
			if (status.ready && !status.IsBot())
				m_players_ready++;
		}

		if (spectator) { // entering spectator status
			PlayerLeftTeam(status.team);
			PlayerLeftAlly(status.ally);
			if (status.ready && !status.IsBot())
				m_players_ready--;
		}

		if (IsFounderMe()) {
			if (status.spectator != spectator) {
				if (spectator) {
					m_opts.spectators++;
				} else {
					m_opts.spectators--;
				}
				SendHostInfo(HI_Spectators);
			}
		}
		user.BattleStatus().spectator = spectator;
	}
}

void IBattle::SetHandicap(User& user, int handicap)
{
	if (IsFounderMe() || user.BattleStatus().IsBot()) {
		user.BattleStatus().handicap = handicap;
	}
}


void IBattle::KickPlayer(User& user)
{
	if (IsFounderMe() || user.BattleStatus().IsBot()) {
		OnUserRemoved(user);
	}
}

int IBattle::GetFreeAlly(bool excludeme) const
{
	int lowest = 0;
	bool changed = true;
	while (changed) {
		changed = false;
		for (unsigned int i = 0; i < GetNumUsers(); i++) {
			User& user = GetUser(i);
			if ((&user == &GetMe()) && excludeme)
				continue;
			if (user.BattleStatus().spectator)
				continue;
			if (user.BattleStatus().ally == lowest) {
				lowest++;
				changed = true;
			}
		}
	}
	return lowest;
}

UserPosition IBattle::GetFreePosition()
{
	UserPosition ret;
	LSL::UnitsyncMap map = LoadMap();
	for (int i = 0; i < int(map.info.positions.size()); i++) {
		bool taken = false;
		for (unsigned int bi = 0; bi < GetNumUsers(); bi++) {
			User& user = GetUser(bi);
			UserBattleStatus& status = user.BattleStatus();
			if (status.spectator)
				continue;
			if ((map.info.positions[i].x == status.pos.x) && (map.info.positions[i].y == status.pos.y)) {
				taken = true;
				break;
			}
		}
		if (!taken) {
			ret.x = LSL::Util::Clamp(map.info.positions[i].x, 0, map.info.width);
			ret.y = LSL::Util::Clamp(map.info.positions[i].y, 0, map.info.height);
			return ret;
		}
	}
	ret.x = map.info.width / 2;
	ret.y = map.info.height / 2;
	return ret;
}


void IBattle::SetHostMap(const std::string& mapname, const std::string& hash)
{
	assert(hash.empty() || LSL::Util::MakeHashUnsigned(hash) == hash);
	m_map_loaded = mapname.empty();
	m_host_map.name = mapname;
	m_host_map.hash = hash;
}


void IBattle::SetLocalMap(const std::string& mapname)
{
	if (mapname.empty()) {
		wxLogWarning("Battle with empty map name!");
		return;
	}

	LSL::UnitsyncMap map = LSL::usync().GetMap(mapname);
	if (map.name != m_local_map.name || map.hash != m_local_map.hash) {
		m_local_map = map;
		m_map_loaded = true;
		if (IsFounderMe()) { // save all rects infos
				     //TODO
		}
	}
}


const LSL::UnitsyncMap& IBattle::LoadMap()
{
	if ((m_map_loaded) || (m_host_map.name.empty()) || !MapExists(true))
		return m_local_map;

	m_local_map = LSL::usync().GetMap(m_host_map.name);
	assert(LSL::Util::MakeHashUnsigned(m_local_map.hash) == m_local_map.hash);
	const bool options_loaded = CustomBattleOptions().loadOptions(LSL::Enum::MapOption, m_host_map.name);
	if (!options_loaded) {
		wxLogWarning(_T("couldn't load the map options"));
	}
	m_map_loaded = true;
	return m_local_map;
}


const std::string& IBattle::GetHostMapName() const
{
	return m_host_map.name;
}


const std::string& IBattle::GetHostMapHash() const
{
	return m_host_map.hash;
}


void IBattle::SetHostGame(const std::string& gamename, const std::string& hash)
{
	assert(hash.empty() || LSL::Util::MakeHashUnsigned(hash) == hash);
	if ((m_host_game.name == gamename) && (m_host_game.hash == hash)) {
		return;
	}
	m_game_loaded = gamename.empty();
	m_host_game.name = gamename;
	m_host_game.hash = hash;
	
	gameNameWithoutVersion = gamename.substr(0,gamename.find_last_of(" "));
	// generate game-specific colour
	int sum = 0;
	for (char& c : gameNameWithoutVersion) {
		sum += c;
	}
	int sumR = 255 - ((sum % 7)*10);
	int sumG = 255 - ((sum % 7)*10);
	int sumB = 255 - ((sum % 4)*10);
	char buff[11];
	sprintf(buff, "#%2x%2x%2x",sumR,sumG,sumB);
	gameBackgroundColour = wxColour(buff);
}


void IBattle::SetLocalGame(const LSL::UnitsyncGame& mod)
{
	assert(mod.hash.empty() || LSL::Util::MakeHashUnsigned(mod.hash) == mod.hash); // hash has to be in unsigned format
	if (mod.hash.empty()) {
		wxLogWarning("empty hash: %s", mod.name);
	}
	if ((mod.name == m_local_game.name) && (mod.hash == m_local_game.hash)) {
		return;
	}
	m_previous_local_game_name = m_local_game.name;
	m_local_game = mod;
	m_game_loaded = true;
}


const LSL::UnitsyncGame& IBattle::LoadGame()
{
	if (m_game_loaded || m_host_game.name.empty()) {
		return m_local_game;
	}
	if (!GameExists(true)) {
		wxLogWarning("Game doesn't exist");
		return m_local_game;
	}
	SetLocalGame(LSL::usync().GetGame(m_host_game.name));
	const bool options_loaded = CustomBattleOptions().loadOptions(LSL::Enum::ModOption, m_host_game.name);

	if (!options_loaded) {
		wxLogWarning("couldn't load the game options");
	}

	m_game_loaded = true;
	return m_local_game;
}


const std::string& IBattle::GetHostGameName() const
{
	return m_host_game.name;
}


const std::string& IBattle::GetHostGameNameWithoutVersion() const
{
	return gameNameWithoutVersion;
}

const wxColour& IBattle::GetHostGameBackgroundColour() const
{
	return gameBackgroundColour;
}

const std::string& IBattle::GetHostGameHash() const
{
	return m_host_game.hash;
}


bool IBattle::MapExists(bool comparehash) const
{
	if (m_host_map.name.empty()) {
		return false;
	}
	if (comparehash) {
		return LSL::usync().MapExists(m_host_map.name, m_host_map.hash);
	}
	return LSL::usync().MapExists(m_host_map.name, "");
}


bool IBattle::GameExists(bool comparehash) const
{
	if (comparehash)
		return LSL::usync().GameExists(m_host_game.name, m_host_game.hash);
	return LSL::usync().GameExists(m_host_game.name, "");
}

void IBattle::RestrictUnit(const std::string& unitname, int count)
{
	m_restricted_units[unitname] = count;
}


void IBattle::UnrestrictUnit(const std::string& unitname)
{
	std::map<std::string, int>::iterator pos = m_restricted_units.find(unitname);
	if (pos == m_restricted_units.end())
		return;
	m_restricted_units.erase(pos);
}


void IBattle::UnrestrictAllUnits()
{
	m_restricted_units.clear();
}


const std::map<std::string, int>& IBattle::GetRestrictedUnits() const
{
	return m_restricted_units;
}

void IBattle::OnSelfLeftBattle()
{
	GetMe().BattleStatus().spectator = false; // always reset back yourself to player when rejoining
	m_is_self_in = false;
	for (size_t j = 0; j < GetNumUsers(); ++j) {
		User& u = GetUser(j);
		if (u.GetBattleStatus().IsBot()) {
			ui().OnUserLeftBattle(*this, u, true);
			OnUserRemoved(u);
			j--;
		}
	}
	ClearStartRects();
	m_teams_sizes.clear();
	m_ally_sizes.clear();
	m_players_ready = 0;
	m_players_sync = 0;
	m_players_ok = 0;
	LSL::usync().UnSetCurrentArchive(); //left battle
}

static std::string FixPresetName(const std::string& name)
{
	// look name up case-insensitively
	const wxArrayString& presetList = sett().GetPresetList();
	int index = presetList.Index(TowxString(name), false /*case insensitive*/);
	if (index == -1)
		return "";

	// set preset to the actual name, with correct case
	return STD_STRING(presetList[index]);
}


bool IBattle::LoadOptionsPreset(const std::string& name)
{
	const std::string preset = FixPresetName(name);
	if (preset.empty())
		return false; //preset not found
	m_preset = preset;

	for (unsigned int i = 0; i < LSL::Enum::LastOption; i++) {
		std::map<wxString, wxString> options = sett().GetHostingPreset(TowxString(m_preset), i);
		if ((LSL::Enum::GameOption)i != LSL::Enum::PrivateOptions) {
			for (std::map<wxString, wxString>::const_iterator itor = options.begin(); itor != options.end(); ++itor) {
				wxLogWarning(itor->first + _T(" ::: ") + itor->second);
				CustomBattleOptions().setSingleOption(STD_STRING(itor->first),
								      STD_STRING(itor->second),
								      (LSL::Enum::GameOption)i);
			}
		} else {
			if (!options[_T("mapname")].IsEmpty()) {
				if (!ui().NeedsDownload(this)) {
					SetLocalMap(STD_STRING(options[_T("mapname")]));
					SendHostInfo(HI_Map);
				}
			}

			for (unsigned int j = 0; j <= GetLastRectIdx(); ++j) {
				if (GetStartRect(j).IsOk())
					RemoveStartRect(j); // remove all rects that might come from map presets
			}
			SendHostInfo(IBattle::HI_StartRects);

			const unsigned int rectcount = FromwxString(options[_T("numrects")]);
			for (unsigned int loadrect = 0; loadrect < rectcount; loadrect++) {
				int ally = FromwxString(options[_T("rect_") + TowxString(loadrect) + _T("_ally")]);
				if (ally == 0)
					continue;
				AddStartRect(ally - 1, FromwxString(options[_T("rect_") + TowxString(loadrect) + _T("_left")]), FromwxString(options[_T("rect_") + TowxString(loadrect) + _T("_top")]), FromwxString(options[_T("rect_") + TowxString(loadrect) + _T("_right")]), FromwxString(options[_T("rect_") + TowxString(loadrect) + _T("_bottom")]));
			}
			SendHostInfo(HI_StartRects);

			wxStringTokenizer tkr(options[_T("restrictions")], _T('\t'));
			m_restricted_units.clear();
			while (tkr.HasMoreTokens()) {
				wxString unitinfo = tkr.GetNextToken();
				RestrictUnit(STD_STRING(unitinfo.BeforeLast(_T('='))), FromwxString(unitinfo.AfterLast(_T('='))));
			}
			SendHostInfo(HI_Restrictions);
		}
	}
	SendHostInfo(HI_Send_All_opts);
	ui().ReloadPresetList();
	return true;
}


void IBattle::SaveOptionsPreset(const std::string& name)
{
	m_preset = FixPresetName(name);
	if (m_preset == "")
		m_preset = name; //new preset

	for (int i = 0; i < (int)LSL::Enum::LastOption; i++) {
		if ((LSL::Enum::GameOption)i != LSL::Enum::PrivateOptions) {
			const auto opts = CustomBattleOptions().getOptionsMap((LSL::Enum::GameOption)i);
			std::map<wxString, wxString> wopts;
			for (const auto pair : opts)
				wopts.insert(std::make_pair(TowxString(pair.first), TowxString(pair.second)));
			sett().SetHostingPreset(TowxString(m_preset), (LSL::Enum::GameOption)i, wopts);
		} else {
			std::map<wxString, wxString> opts;
			opts[_T("mapname")] = TowxString(GetHostMapName());
			unsigned int validrectcount = 0;
			if (LSL::Util::FromIntString(
				CustomBattleOptions().getSingleValue("startpostype", LSL::Enum::EngineOption)) == ST_Choose) {
				unsigned int boxcount = GetLastRectIdx();
				for (unsigned int boxnum = 0; boxnum <= boxcount; boxnum++) {
					BattleStartRect rect = GetStartRect(boxnum);
					if (rect.IsOk()) {
						opts[_T("rect_") + TowxString(validrectcount) + _T("_ally")] = TowxString(rect.ally + 1);
						opts[_T("rect_") + TowxString(validrectcount) + _T("_left")] = TowxString(rect.left);
						opts[_T("rect_") + TowxString(validrectcount) + _T("_top")] = TowxString(rect.top);
						opts[_T("rect_") + TowxString(validrectcount) + _T("_bottom")] = TowxString(rect.bottom);
						opts[_T("rect_") + TowxString(validrectcount) + _T("_right")] = TowxString(rect.right);
						validrectcount++;
					}
				}
			}
			opts[_T("numrects")] = TowxString(validrectcount);

			wxString restrictionsstring;
			for (std::map<std::string, int>::const_iterator itor = m_restricted_units.begin(); itor != m_restricted_units.end(); ++itor) {
				restrictionsstring << TowxString(itor->first) << _T('=') << TowxString(itor->second) << _T('\t');
			}
			opts[_T("restrictions")] = restrictionsstring;

			sett().SetHostingPreset(TowxString(m_preset), (LSL::Enum::GameOption)i, opts);
		}
	}
	sett().SaveSettings();
	ui().ReloadPresetList();
}


const std::string& IBattle::GetCurrentPreset() const
{
	return m_preset;
}


void IBattle::DeletePreset(const std::string& name)
{
	const std::string preset = FixPresetName(name);
	if (m_preset == preset)
		m_preset = "";
	sett().DeletePreset(TowxString(preset));
	ui().ReloadPresetList();
}

LSL::StringVector IBattle::GetPresetList()
{
	return wxArrayStringToLSL(sett().GetPresetList());
}

void IBattle::UserPositionChanged(const User& /*unused*/)
{
}

void IBattle::SetPlayBackFilePath(const std::string& path)
{
	m_playback_file_path = path;
	// I don't see any easier way of dealing with unicode.
	m_playback_file_name = TowxString(path).AfterLast(wxFileName::GetPathSeparator());
}

void IBattle::AddUserFromDemo(User& user)
{
	user.BattleStatus().isfromdemo = true;
	m_internal_user_list[user.GetNick()] = user;
	UserList::AddUser(m_internal_user_list[user.GetNick()]);
}

void IBattle::SetProxy(const std::string& value)
{
	m_opts.proxyhost = value;
}

bool IBattle::IsProxy() const
{
	return !m_opts.proxyhost.empty();
}

const std::string& IBattle::GetProxy() const
{
	return m_opts.proxyhost;
}

bool IBattle::IsFounderMe() const
{
	return ((m_opts.founder == GetMe().GetNick()) || (IsProxy() && !m_generating_script));
}

bool IBattle::IsFounder(const User& user) const
{
	if (UserExists(m_opts.founder)) {
		try {
			return &GetFounder() == &user;
		} catch (...) {
			return false;
		}
	} else
		return false;
}

void IBattle::LoadScriptMMOpts(const std::string& sectionname, const LSL::TDF::PDataList& node)
{
	if (!node.ok())
		return;
	LSL::TDF::PDataList section(node->Find(sectionname));
	if (!section.ok())
		return;
	LSL::OptionsWrapper& opts = CustomBattleOptions();
	for (LSL::TDF::PNode n = section->First(); n != section->Last(); n = section->Next(n)) {
		if (!n.ok())
			continue;
		opts.setSingleOption(n->Name(), section->GetString(n->Name()));
	}
}

void IBattle::LoadScriptMMOpts(const LSL::TDF::PDataList& node)
{
	if (!node.ok())
		return;
	LSL::OptionsWrapper& opts = CustomBattleOptions();
	auto options = opts.getOptionsMap(LSL::Enum::EngineOption);
	for (const auto i : options) {
		opts.setSingleOption(i.first, node->GetString(i.first, i.second));
	}
}

//! (koshi) don't delete commented things please, they might be need in the future and i'm lazy
void IBattle::GetBattleFromScript(bool loadmapmod)
{

	BattleOptions opts;
	std::stringstream ss(GetScript());
	LSL::TDF::PDataList script(LSL::TDF::ParseTDF(ss));

	LSL::TDF::PDataList replayNode(script->Find("GAME"));
	if (replayNode.ok()) {

		std::string gamename = replayNode->GetString("GameType");
		std::string gamehash = replayNode->GetString("ModHash");
		if (!gamehash.empty())
			gamehash = LSL::Util::MakeHashUnsigned(gamehash);
		SetHostGame(gamename, gamehash);

		//don't have the maphash, what to do?
		//ui download function works with mapname if hash is empty, so works for now
		std::string mapname = replayNode->GetString("MapName");
		std::string maphash = replayNode->GetString("MapHash");
		if (!gamehash.empty())
			gamehash = LSL::Util::MakeHashUnsigned(maphash);
		SetHostMap(mapname, maphash);

		//        opts.ip         = replayNode->GetString( _T("HostIP") );
		//        opts.port       = replayNode->GetInt  ( _T("HostPort"), DEFAULT_EXTERNAL_UDP_SOURCE_PORT );
		opts.spectators = 0;

		int playernum = replayNode->GetInt("NumPlayers", 0);
		int usersnum = replayNode->GetInt("NumUsers", 0);
		if (usersnum > 0)
			playernum = usersnum;
		//        int allynum = replayNode->GetInt  ( _T("NumAllyTeams"), 1);
		//        int teamnum = replayNode->GetInt  ( _T("NumTeams"), 1);


		LSL::StringVector sides;
		if (loadmapmod) {
			sides = LSL::usync().GetSides(gamename);
		}

		IBattle::TeamVec parsed_teams = GetParsedTeamsVec();
		IBattle::AllyVec parsed_allies = GetParsedAlliesVec();

		//[PLAYERX] sections
		for (int i = 0; i < playernum; ++i) {
			LSL::TDF::PDataList player(replayNode->Find(stdprintf("PLAYER%d", i)));
			LSL::TDF::PDataList bot(replayNode->Find(stdprintf("AI%d", i)));
			if (player.ok() || bot.ok()) {
				if (bot.ok())
					player = bot;
				User user(player->GetString("Name"), boost::to_upper_copy(player->GetString("CountryCode")));
				UserBattleStatus& status = user.BattleStatus();
				status.isfromdemo = true;
				status.spectator = player->GetInt("Spectator", 0);
				opts.spectators += user.BattleStatus().spectator;
				status.team = player->GetInt("Team");
				if (!status.spectator) {
					PlayerJoinedTeam(status.team);
				}
				status.sync = true;
				status.ready = true;
				if (status.spectator)
					m_opts.spectators++;
				else {
					if (!bot.ok()) {
						if (status.ready)
							m_players_ready++;
						if (status.sync)
							m_players_sync++;
						if (status.sync && status.ready)
							m_players_ok++;
					}
				}

				//! (koshi) changed this from ServerRankContainer to RankContainer
				user.Status().rank = (UserStatus::RankContainer)player->GetInt("Rank", -1);

				if (bot.ok()) {
					status.aishortname = bot->GetString("ShortName");
					status.aiversion = bot->GetString("Version");
					int ownerindex = bot->GetInt("Host");
					LSL::TDF::PDataList aiowner(replayNode->Find(stdprintf("PLAYER%d", ownerindex)));
					if (aiowner.ok()) {
						status.owner = aiowner->GetString("Name");
					}
				}

				IBattle::TeamInfoContainer teaminfos = parsed_teams[user.BattleStatus().team];
				if (!teaminfos.exist) {
					LSL::TDF::PDataList team(replayNode->Find(stdprintf("TEAM%d", user.BattleStatus().team)));
					if (team.ok()) {
						teaminfos.exist = true;
						teaminfos.TeamLeader = team->GetInt("TeamLeader", 0);
						teaminfos.StartPosX = team->GetInt("StartPosX", -1);
						teaminfos.StartPosY = team->GetInt("StartPosY", -1);
						teaminfos.AllyTeam = team->GetInt("AllyTeam", 0);
						teaminfos.RGBColor = LSL::lslColor::FromFloatString(team->GetString("RGBColor"));
						teaminfos.SideName = team->GetString("Side", "");
						teaminfos.Handicap = team->GetInt("Handicap", 0);
						const int sidepos = LSL::Util::IndexInSequence(sides, teaminfos.SideName);
						teaminfos.SideNum = sidepos;
						parsed_teams[user.BattleStatus().team] = teaminfos;
					}
				}
				if (teaminfos.exist) {
					status.ally = teaminfos.AllyTeam;
					status.pos.x = teaminfos.StartPosX;
					status.pos.y = teaminfos.StartPosY;
					status.colour = teaminfos.RGBColor;
					status.handicap = teaminfos.Handicap;
					if (!status.spectator) {
						PlayerJoinedAlly(status.ally);
					}
					if (teaminfos.SideNum >= 0)
						status.side = teaminfos.SideNum;
					IBattle::AllyInfoContainer allyinfos = parsed_allies[user.BattleStatus().ally];
					if (!allyinfos.exist) {
						LSL::TDF::PDataList ally(replayNode->Find(stdprintf("ALLYTEAM%d", user.BattleStatus().ally)));
						if (ally.ok()) {
							allyinfos.exist = true;
							allyinfos.NumAllies = ally->GetInt("NumAllies", 0);
							allyinfos.StartRectLeft = ally->GetInt("StartRectLeft", 0);
							allyinfos.StartRectTop = ally->GetInt("StartRectTop", 0);
							allyinfos.StartRectRight = ally->GetInt("StartRectRight", 0);
							allyinfos.StartRectBottom = ally->GetInt("StartRectBottom", 0);
							parsed_allies[user.BattleStatus().ally] = allyinfos;
							AddStartRect(user.BattleStatus().ally, allyinfos.StartRectTop, allyinfos.StartRectTop, allyinfos.StartRectRight, allyinfos.StartRectBottom);
						}
					}
				}

				AddUserFromDemo(user);
			}
		}
		SetParsedTeamsVec(parsed_teams);
		SetParsedAlliesVec(parsed_allies);

		//MMoptions, this'll fail unless loading map/mod into wrapper first
		if (loadmapmod) {
			LoadScriptMMOpts("mapoptions", replayNode);
			LoadScriptMMOpts("modoptions", replayNode);
		}

		opts.maxplayers = playernum;
	}
	SetBattleOptions(opts);
}

void IBattle::SetInGame(bool ingame)
{
	m_ingame = ingame;
	if (m_ingame)
		m_start_time = wxGetUTCTime();
	else
		m_start_time = 0;
}

long IBattle::GetBattleRunningTime() const
{
	if (!GetInGame())
		return 0;
	if (m_start_time == 0)
		return 0;
	return wxGetUTCTime() - m_start_time;
}


IServer& IBattle::GetServer()
{
	return serverSelector().GetServer();
}

User& IBattle::GetFounder() const
{
	return GetUser(m_opts.founder);
}

void IBattle::RemoveUnfittingBots()
{
	const auto old_ais = LSL::usync().GetAIList(m_previous_local_game_name);
	const auto new_ais = LSL::usync().GetAIList(m_local_game.name);
	LSL::StringVector diff(old_ais.size());
	LSL::StringVector::iterator end = std::set_difference(old_ais.begin(), old_ais.end(), new_ais.begin(), new_ais.end(), diff.begin());
	for (auto it = diff.begin(); it != end; ++it) {
		for (size_t j = 0; j < GetNumUsers(); ++j) {
			User& u = GetUser(j);
			if (u.GetBattleStatus().airawname == *it)
				KickPlayer(u);
		}
	}
}

bool IBattle::EngineExists() const
{
	return !SlPaths::GetCompatibleVersion(GetEngineVersion()).empty();
}
