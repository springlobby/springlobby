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
void AutoHost::DoAction(const std::string& str)
{
	m_battle.DoAction(str);
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
		DoAction("is auto-balancing alliances ...");
	} else if (command == _T( "!cbalance" )) {
		unsigned int num = FromwxString(params);
		m_battle.Autobalance(IBattle::balance_random, true, false, num);
		DoAction("is auto-balancing alliances ..." );
	} else if (command == _T( "!help" )) {
		DoAction("The following commands are available ( <> = mandatory value, {} = optional value ):" );
		DoAction("!addbox <topx> <topy> <bottomx> <bottomy> {allynumber}: adds a <allynumber> start restriction to the given coordinates, coordinates range from 0 to 200." );
		DoAction("!balance {number}: tries to put players into allyteams by how many start boxes there are, uses {number} allyteams if present." );
		DoAction("!cbalance {number}: see !balance but tries to put clanmates together first." );
		DoAction("!fixcolors: changes players duplicate colours so they are unique." );
		DoAction("!fixids {number}: tries to put players into control teams by number, if number is omitted it assignes as different controlteam per player." );
		DoAction("!cfixids {number}: see !fixids but tries to put clanmates together first." );
		DoAction("!help: this guide." );
		DoAction("!listprofiles: lists the available battle profiles." );
		DoAction("!loadprofile profilename: loads an available battle profile." );
		DoAction("!lock: prevents more people to join." );
		DoAction("!map <name>: switches to <name>." );
		DoAction("!removebox <allynumber>: deletes <allynumber> start restriction's box." );
		DoAction("!ring {name}: rings players that are not ready or {name} if specified." );
		DoAction("!set <optionname> <value>: sets battle option <optionname> to <value>" );
		DoAction("!spectunsynced: sets all players with unsynced status to be spectators." );
		DoAction("!start: starts the battle." );
		DoAction("!unlock: opens the battle again." );
	} else if (command == _T( "!ring" )) {
		if (!params.IsEmpty()) {
			const std::string user = STD_STRING(GetBestMatch(m_userlist, params));
			try {
				User& u = m_battle.GetUser(user);
				m_battle.RingPlayer(u);
				DoAction("is ringing " + user);
			} catch (...) {
				DoAction("cannot ring " + user);
			}
		} else {
			m_battle.RingNotReadyPlayers();
			DoAction("is ringing players not ready ...");
		}
	} else if (command == _T( "!listprofiles" )) {
		LSL::StringVector profilelist = m_battle.GetPresetList();
		unsigned int count = profilelist.size();
		if (count == 0) {
			DoAction("There are no presets available.");
		} else {
			DoAction("The following presets are available:");
			for (unsigned int i = 0; i < count; i++) {
				DoAction(profilelist[i]);
			}
		}
	} else if (command == _T( "!loadprofile" )) {
		const std::string profilename = GetBestMatch(m_battle.GetPresetList(), STD_STRING(params));
		if (!m_battle.LoadOptionsPreset(profilename))
			DoAction("Profile not found, use !listprofiles for a list of available profiles.");
		else
			DoAction("has loaded profile: " + profilename);
	} else if (command == _T( "!fixcolors" )) {
		m_battle.FixColours();
		DoAction("is fixing colors." );
	} else if (command == _T( "!lock" )) {
		m_battle.SetIsLocked(true);
		DoAction("has locked the battle.");
		m_battle.SendHostInfo(IBattle::HI_Locked);
	} else if (command == _T( "!unlock" )) {
		m_battle.SetIsLocked(false);
		DoAction("has unlocked the battle." );
		m_battle.SendHostInfo(IBattle::HI_Locked);
	} else if (command == _T( "!fixids" )) {
		unsigned int num = FromwxString(params);
		m_battle.FixTeamIDs(IBattle::balance_divide, false, false, num);
		DoAction("is auto-balancing control teams ..." );
	} else if (command == _T( "!cfixids" )) {
		unsigned int num = FromwxString(params);
		m_battle.FixTeamIDs(IBattle::balance_divide, true, true, num);
		DoAction("is auto-balancing control teams ...");
	} else if (command == _T( "!spectunsynced" )) {
		m_battle.ForceUnsyncedToSpectate();
		DoAction("is forcing unsynced players to be spectators.");
	} else if (command == _T( "!map" )) {
		if (params.IsEmpty())
			DoAction("cannot switch to void mapname");
		else {
			const std::string mapname = STD_STRING(GetBestMatch(lslTowxArrayString(LSL::usync().GetMapList()), params));
			try {
				m_battle.SetLocalMap(mapname);
				DoAction("is switching to map " + mapname);
				m_battle.SendHostInfo(IBattle::HI_Map);
			} catch (...) {
				DoAction("cannot switch to map " + mapname);
			}
		}
	} else if (command == _T( "!set" )) {
		const std::string key = STD_STRING(params.BeforeFirst(_T(' ')));
		const std::string value = STD_STRING(params.AfterFirst(_T(' ')));
		const bool exists = m_battle.CustomBattleOptions().keyExists(key);
		if (!exists) {
			DoAction("cannot find option entry " + key);
			return;
		}
		const LSL::Enum::GameOption section = m_battle.CustomBattleOptions().GetSection(key);
		const bool result = m_battle.CustomBattleOptions().setSingleOption(key, value, section);
		if (!result) {
			DoAction("cannot set option " + key + " to value " + value + ", reason: invalid value.");
			return;
		}

		m_battle.SendHostInfo(stdprintf("%d_%s", section, key.c_str()));
		DoAction("has set option " + key + " to value " + value);
	} else if (command == _T( "!addbox" )) {
		long var[5]; // 0-3 = positions, 4 = ally
		wxArrayString values = wxStringTokenize(params, _T( " " ));
		const size_t numvalues = values.GetCount();
		if (numvalues < 3 || numvalues > 4) {
			DoAction("has received an invalid number of params for !addbox");
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
			DoAction("has received an invalid param for !addbox" );
			return;
		}
		BattleStartRect rect = m_battle.GetStartRect(var[4]);
		if (rect.IsOk()) {
			DoAction(stdprintf("cannot add a startbox for allyteam %d because one is already present.", var[4]));
			return;
		}
		m_battle.AddStartRect(var[4], var[0], var[1], var[2], var[3]);
		m_battle.SendHostInfo(IBattle::HI_StartRects);
		DoAction(stdprintf("has added start box for allyteam %d", var[4]));
	} else if (command == _T( "!removebox" )) {
		long boxnumber;
		bool numberok = params.ToLong(&boxnumber);
		if (numberok) {
			BattleStartRect rect = m_battle.GetStartRect(boxnumber);
			if (rect.IsOk()) {
				m_battle.RemoveStartRect(boxnumber);
				m_battle.SendHostInfo(IBattle::HI_StartRects);
				DoAction(stdprintf("has removed the start box for allyteam ", boxnumber));
			} else {
				DoAction("cannot find start box " + STD_STRING(params));
			}
		} else {
			DoAction("has recieved an invalid param to !removebox command" );
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
		DoAction("has auto-unlocked the battle.");
		m_battle.SendHostInfo(IBattle::HI_Locked);
	}
}


void AutoHost::StartBattle()
{
	// todo: the logic here is copied from BattleRoomTab::OnStart, may wish to refactor this sometime.
	// note: the strings here must remain untranslated because they're visible to everyone in the battle!

	m_battle.GetMe().BattleStatus().ready = true;

	if (!m_battle.IsEveryoneReady()) {
		DoAction("Some players are not ready yet.");
		return;
	}

	DoAction("is starting game ...");
	m_battle.StartHostedBattle();

	m_battle.SaveMapDefaults(); // save map preset
}
