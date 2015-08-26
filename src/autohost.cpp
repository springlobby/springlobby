/* This file is part of the Springlobby (GPL v2 or later), see COPYING */


#include "autohost.h"
#include "ibattle.h"
#include "iserver.h"
#include "settings.h"
#include "user.h"
#include "utils/conversion.h"
#include "utils/lslconversion.h"
#include "utils/misc.h"
#include "utils/version.h"
#include <lslunitsync/unitsync.h>

#include <wx/tokenzr.h>


AutoHost::AutoHost(IBattle& battle)
    : m_battle(battle)
    , m_enabled(false)
    , m_lastActionTime(0)
{
}


void AutoHost::SetEnabled(const bool enabled)
{
	m_enabled = enabled;
}

bool AutoHost::GetEnabled() const
{
	return m_enabled;
}
void AutoHost::DoAction(const wxString& str)
{
	m_battle.DoAction(STD_STRING(str));
}

void AutoHost::OnSaidBattle(const wxString& /*unused*/, const wxString& msg)
{
	// do nothing if autohost functionality is disabled

	if (!m_enabled)
		return;

	// protect against command spam
	time_t currentTime = time(NULL);
	if ((currentTime - m_lastActionTime) < 5)
		return;
	m_lastActionTime = currentTime;


	// check for autohost commands
	wxString command = msg.BeforeFirst(_T(' '));
	wxString params = msg.AfterFirst(_T(' '));
	if (command == _T( "!start" )) {
		StartBattle();
	} else if (command == _T( "!balance" )) {
		unsigned int num = FromwxString(params);
		m_battle.Autobalance(IBattle::balance_random, false, false, num);
		DoAction(_T("is auto-balancing alliances ..."));
	} else if (command == _T( "!cbalance" )) {
		unsigned int num = FromwxString(params);
		m_battle.Autobalance(IBattle::balance_random, true, false, num);
		DoAction(_T( "is auto-balancing alliances ..." ));
	} else if (command == _T( "!help" )) {
		DoAction(_T( "The following commands are available ( <> = mandatory value, {} = optional value ):" ));
		DoAction(_T( "!addbox <topx> <topy> <bottomx> <bottomy> {allynumber}: adds a <allynumber> start restriction to the given coordinates, coordinates range from 0 to 200." ));
		DoAction(_T( "!balance {number}: tries to put players into allyteams by how many start boxes there are, uses {number} allyteams if present." ));
		DoAction(_T( "!cbalance {number}: see !balance but tries to put clanmates together first." ));
		DoAction(_T( "!fixcolors: changes players duplicate colours so they are unique." ));
		DoAction(_T( "!fixids {number}: tries to put players into control teams by number, if number is omitted it assignes as different controlteam per player." ));
		DoAction(_T( "!cfixids {number}: see !fixids but tries to put clanmates together first." ));
		DoAction(_T( "!help: this guide." ));
		DoAction(_T( "!listprofiles: lists the available battle profiles." ));
		DoAction(_T( "!loadprofile profilename: loads an available battle profile." ));
		DoAction(_T( "!lock: prevents more people to join." ));
		DoAction(_T( "!map <name>: switches to <name>." ));
		DoAction(_T( "!removebox <allynumber>: deletes <allynumber> start restriction's box." ));
		DoAction(_T( "!ring {name}: rings players that are not ready or {name} if specified." ));
		DoAction(_T( "!set <optionname> <value>: sets battle option <optionname> to <value>" ));
		DoAction(_T( "!spectunsynced: sets all players with unsynced status to be spectators." ));
		DoAction(_T( "!start: starts the battle." ));
		DoAction(_T( "!unlock: opens the battle again." ));
	} else if (command == _T( "!ring" )) {
		if (!params.IsEmpty()) {
			wxString user = GetBestMatch(m_userlist, params);
			try {
				User& u = m_battle.GetUser(STD_STRING(user));
				m_battle.RingPlayer(u);
				DoAction(_T( "is ringing " ) + user);
			} catch (...) {
				DoAction(_T( "cannot ring " ) + user);
			}
		} else {
			m_battle.RingNotReadyPlayers();
			DoAction(_T( "is ringing players not ready ..." ));
		}
	} else if (command == _T( "!listprofiles" )) {
		LSL::StringVector profilelist = m_battle.GetPresetList();
		unsigned int count = profilelist.size();
		if (count == 0) {
			DoAction(_T( "There are no presets available." ));
		} else {
			DoAction(_T( "The following presets are available:" ));
			for (unsigned int i = 0; i < count; i++) {
				DoAction(TowxString(profilelist[i]));
			}
		}
	} else if (command == _T( "!loadprofile" )) {
		const std::string profilename = GetBestMatch(m_battle.GetPresetList(), STD_STRING(params));
		if (!m_battle.LoadOptionsPreset(profilename))
			DoAction(_T( "Profile not found, use !listprofiles for a list of available profiles." ));
		else
			DoAction(_T( "has loaded profile: " ) + TowxString(profilename));
	} else if (command == _T( "!fixcolors" )) {
		m_battle.FixColours();
		DoAction(_T( "is fixing colors." ));
	} else if (command == _T( "!lock" )) {
		m_battle.SetIsLocked(true);
		DoAction(_T( "has locked the battle." ));
		m_battle.SendHostInfo(IBattle::HI_Locked);
	} else if (command == _T( "!unlock" )) {
		m_battle.SetIsLocked(false);
		DoAction(_T( "has unlocked the battle." ));
		m_battle.SendHostInfo(IBattle::HI_Locked);
	} else if (command == _T( "!fixids" )) {
		unsigned int num = FromwxString(params);
		m_battle.FixTeamIDs(IBattle::balance_divide, false, false, num);
		DoAction(_T( "is auto-balancing control teams ..." ));
	} else if (command == _T( "!cfixids" )) {
		unsigned int num = FromwxString(params);
		m_battle.FixTeamIDs(IBattle::balance_divide, true, true, num);
		DoAction(_T( "is auto-balancing control teams ..." ));
	} else if (command == _T( "!spectunsynced" )) {
		m_battle.ForceUnsyncedToSpectate();
		DoAction(_T( "is forcing unsynced players to be spectators." ));
	} else if (command == _T( "!map" )) {
		if (params.IsEmpty())
			DoAction(_T( "cannot switch to void mapname" ));
		else {
			const wxString mapname = GetBestMatch(lslTowxArrayString(LSL::usync().GetMapList()), params);
			try {
				m_battle.SetLocalMap(STD_STRING(mapname));
				DoAction(_T( "is switching to map " ) + mapname);
				m_battle.SendHostInfo(IBattle::HI_Map);
			} catch (...) {
				DoAction(_T( "cannot switch to map " ) + mapname);
			}
		}
	} else if (command == _T( "!set" )) {
		const std::string key = STD_STRING(params.BeforeFirst(_T(' ')));
		const std::string value = STD_STRING(params.AfterFirst(_T(' ')));
		const bool exists = m_battle.CustomBattleOptions().keyExists(key);
		if (exists) {
			bool result = m_battle.CustomBattleOptions().setSingleOption(key, value);
			if (result) {
				auto section = m_battle.CustomBattleOptions().GetSection(key);
				m_battle.SendHostInfo(stdprintf("%d_%s", section, key.c_str()));
				DoAction(TowxString("has set option " + key + " to value " + value));
			} else
				DoAction(TowxString("cannot set option " + key + " to value " + value + ", reason: invalid value."));
		} else {
			DoAction(TowxString("cannot find option entry " + key));
		}
	} else if (command == _T( "!addbox" )) {
		long var[5]; // 0-3 = positions, 4 = ally
		wxArrayString values = wxStringTokenize(params, _T( " " ));
		const size_t numvalues = values.GetCount();
		if (numvalues < 3 || numvalues > 4) {
			DoAction(_T( "has received an invalid number of params for !addbox" ));
			return;
		}
		bool valueok = true;
		for (size_t i=0; i<numvalues; i++) {
			valueok &= values[i].ToLong(&var[i]);
		}
		for (size_t i=0; i<4; i++) { // check if vars are in reasonable range
			valueok &= (var[i] >= 0) && (var[i] <= 200);
		}
		if (numvalues == 5) {
			valueok &= var[4] >= 0 && var[4] <= 255;
		} else {
			var[4] = m_battle.GetNextFreeRectIdx();
		}
		if (!valueok) {
			DoAction(_T( "has received an invalid param for !addbox" ));
			return;
		}
		BattleStartRect rect = m_battle.GetStartRect(var[4]);
		if (rect.IsOk()) {
			DoAction(wxString::Format(_T("cannot add a startbox for allyteam %d because one is already present."), var[4]));
			return;
		}
		m_battle.AddStartRect(var[4], var[0], var[1], var[2], var[3]);
		m_battle.SendHostInfo(IBattle::HI_StartRects);
		DoAction(_T( "has added start box for allyteam " ) + TowxString(var[4]));
	} else if (command == _T( "!removebox" )) {
		long boxnumber;
		bool numberok = params.ToLong(&boxnumber);
		if (numberok) {
			BattleStartRect rect = m_battle.GetStartRect(boxnumber);
			if (rect.IsOk()) {
				m_battle.RemoveStartRect(boxnumber);
				m_battle.SendHostInfo(IBattle::HI_StartRects);
				DoAction(_T( "has removed the start box for allyteam " ) + TowxString(boxnumber));
			} else {
				DoAction(_T( "cannot find start box " ) + params);
			}
		} else {
			DoAction(_T( "has recieved an invalid param to !removebox command" ));
		}
	} else
		return;
}


/// Should only be called if user isn't immediately kicked (ban / rank limit)
void AutoHost::OnUserAdded(User& user)
{
	m_userlist.Add(TowxString(user.GetNick()));
	// do nothing if autohost functionality is disabled
	if (!m_enabled)
		return;
	m_battle.DoAction(stdprintf("Hi %s, this battle is in %s autohost mode. For help say !help", user.GetNick().c_str(), getSpringlobbyVersion().c_str()));
}


void AutoHost::OnUserRemoved(User& user)
{
	if (m_userlist.Index(TowxString(user.GetNick())) != wxNOT_FOUND) //triggers assertion in arraystring otherwise
		m_userlist.Remove(TowxString(user.GetNick()));
	// do nothing if autohost functionality is disabled
	if (!m_enabled)
		return;

	if (m_battle.GetNumUsers() == 1 && m_battle.IsLocked()) {
		m_battle.SetIsLocked(false);
		DoAction(_T( "has auto-unlocked the battle." ));
		m_battle.SendHostInfo(IBattle::HI_Locked);
	}
}


void AutoHost::StartBattle()
{
	// todo: the logic here is copied from BattleRoomTab::OnStart, may wish to refactor this sometime.
	// note: the strings here must remain untranslated because they're visible to everyone in the battle!

	m_battle.GetMe().BattleStatus().ready = true;

	if (!m_battle.IsEveryoneReady()) {
		DoAction(_T( "Some players are not ready yet." ));
		return;
	}

	DoAction(_T( "is starting game ..." ));
	m_battle.StartHostedBattle();

	m_battle.SaveMapDefaults(); // save map preset
}
