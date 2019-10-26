/* This file is part of the Springlobby (GPL v2 or later), see COPYING */


/**
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

TODO: remove all wx stuff & merge / move to

lsl/spring/spring.cpp

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
**/


//
// Class: Spring
//
#include "spring.h"

#include <lsl/battle/tdfcontainer.h>
#include <lslunitsync/unitsync.h>
#include <lslutils/conversion.h>
#include <lslutils/globalsmanager.h>
#include <wx/filename.h>
#include <wx/log.h>

#include <algorithm>
#include <clocale>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <random>
#include <algorithm>
#include <cerrno>

#include "gui/crashreporterdialog.h"
#include "gui/customdialogs.h"
#include "gui/mainwindow.h"
#include "gui/ui.h"
#include "ibattle.h"
#include "log.h"
#include "settings.h"
#include "springprocess.h"
#include "utils/conversion.h"
#include "utils/globalevents.h"
#include "utils/slconfig.h"
#include "utils/slpaths.h"

SLCONFIG("/Spring/Safemode", false, "launch spring in safemode");

BEGIN_EVENT_TABLE(Spring, wxEvtHandler)

EVT_COMMAND(PROC_SPRING, wxEVT_SPRING_EXIT, Spring::OnTerminated)

END_EVENT_TABLE()

#define FIRST_UDP_SOURCEPORT 8300

Spring& spring()
{
	static LSL::Util::LineInfo<Spring> m(AT);
	static LSL::Util::GlobalObjectHolder<Spring, LSL::Util::LineInfo<Spring> > m_spring(m);
	return m_spring;
}

Spring::Spring()
    : wxEvtHandler()
    , crash_count(0)
    , m_process(NULL)
    , m_running(false)
{
}


Spring::~Spring()
{
	m_process = NULL;
}


bool Spring::IsRunning() const
{
	return m_running;
}

bool Spring::Run(IBattle& battle)
{
	// Short-circuit before we "damage" internal and on-disk structures
	if (m_running) {
		wxLogError(_("An engine/game is already running!"));
		return false;
	}

	// Fresh start.
	crash_count = 0;
	engine_path = SlPaths::GetSpringBinary(battle.GetEngineVersion());
	engine_params.clear();

	if (!wxFile::Exists(TowxString(engine_path))) {
		customMessageBoxModal(SL_MAIN_ICON, wxString::Format(
		  _("The engine executable version '%s' was not found at the set location '%s', please re-check."),
		  battle.GetEngineVersion().c_str(), engine_path.c_str()), _("Executable not found"));
		ui().mw().ShowConfigure(MainWindow::OPT_PAGE_SPRING);
		return false;
	}
	wxLogMessage(_T("Going to start engine %s with executable at %s"),
	             battle.GetEngineVersion().c_str(), engine_path.c_str());

	const std::string& demopath = battle.GetPlayBackFilePath();
	if (!demopath.empty()) {
		engine_params.push_back(TowxString(demopath));
		return LaunchEngine();
	}

//	if (cfg().ReadBool(_T( "/Spring/Safemode" ))) {

	const wxString scripttxt = TowxString(SlPaths::GetLobbyWriteDir()) + _T("script.txt");
	try {
		wxFile f(scripttxt, wxFile::write);
		battle.DisableHostStatusInProxyMode(true);
		f.Write(WriteScriptTxt(battle));
		battle.DisableHostStatusInProxyMode(false);
		f.Close();
	} catch (const std::exception& e) {
		wxLogError(_T("Couldn't write %s, exception caught:\n %s"), scripttxt, e.what());
		return false;
	}

	engine_params.push_back(scripttxt);
	return LaunchEngine();
}

bool Spring::LaunchEngine()
{
	// Print out command line
	std::string stdparams;
	for (const wxString param: engine_params) {
		if (!stdparams.empty())
			stdparams += " ";
		stdparams += STD_STRING(param);
	}

	const std::string datadir = SlPaths::GetDataDir();
	wxLogMessage(_T("Engine call arguments (datadir: %s): %s %s"),
	             datadir.c_str(), TowxString(engine_path).c_str(), stdparams.c_str());

	// Actually run
	wxSetWorkingDirectory(TowxString(datadir));
	if (m_process == 0) {
		m_process = new SpringProcess(*this);
	}
	m_process->Create();
	m_process->SetCommand(TowxString(engine_path), engine_params);
	m_process->Run();
	m_running = true;
	if (0 == crash_count) // Still trying to start...
		GlobalEventManager::Instance()->Send(GlobalEventManager::OnSpringStarted);
	return true;
}

void Spring::OnTerminated(wxCommandEvent& event)
{
	const int exit_code = event.GetInt();
	wxLogInfo(_T("Engine exited with code %d"), exit_code);

	m_running = false;
	m_process = NULL;

	// First crash, try safe mode
	if (0 != exit_code) {
		++crash_count;
		wxString heading(_("Engine crashed?"));
		wxString message = wxString::Format(_("Engine exited with nonzero code %d\n\n"), exit_code);

		if (1 == crash_count) {
			message += _("Possible reasons include:\n"
			             "a) Timeout when connecting.\n"
						   " Make sure that the host you are connecting to has their firewall(s) and router(s)"
						   " configured correctly. The default port is 8452.\n"
						 "b) Long connection loss during gameplay.\n"
			               " In this case simply restart normally.\n"
			             "c) Graphics-related crash\n"
						   " The engine or game requires more than what your graphics hardware is capable of."
						   " You can try restarting in safe mode."
						   " If that worked, then adjust engine settings to match your hardware capabilities."
						   " If the problems persist, then ask in the newbies channel for help.\n"
			             "d) Choosing incompatible content for the selected game\n"
						   " Sometimes, choosing incompatible content for the selected game will either crash the"
						   " engine or the match will stop immidiately after loading finishes."
			               " Are you using the right engine version for the selected game?"
			               " Please consult the documentation of the selected game.\n"
			             "\n"
			             "We can run the engine again in safe mode. This will disable all graphics"
			             " features known to cause problems, but will drastically reduce the visual"
			             " quality of the game and some of the effects may not be rendered at all!"
			  );

			const wxString infologPath = SlPaths::GetDataDir() + wxFileName::GetPathSeparator() + "infolog.txt";
			int code = ui().AskCrashReporter(infologPath, message, infologPath);
			if (code == CrashReporterDialog::CANCEL) {
				wxLogMessage("Re-run cancelled");
				crash_count = 0;
			} else if (code == CrashReporterDialog::RERUN_NORMAL) {
				wxLogMessage(_T("Will re-run engine in normal-mode"));
				LaunchEngine();
				return; // We have not terminated yet!
			} else if (code == CrashReporterDialog::RERUN_SAFE) {
				wxLogMessage(_T("Will re-run engine in safe-mode"));
				engine_params.push_back(_T("--safemode"));
				LaunchEngine();
				return; // We have not terminated yet!
			}
		} else if (2 == crash_count) {
			message += _("Ooops, the engine crashed again.\n\n"
			             "This time I do not know what to do anymore. Please contact my developers!\n"
			             ":(");

			customMessageBoxModal(SL_MAIN_ICON, message);
		}
	} else if (0 != crash_count) { // there was a crash, but we are cool now
		customMessageBoxModal(SL_MAIN_ICON, _("Please report the crash to the game's developers\n"));
	}

	// Forward notification to other listeners
	event.SetEventType(GlobalEventManager::OnSpringTerminated);
	GlobalEventManager::Instance()->Send(event);
}

std::string Spring::WriteScriptTxt(IBattle& battle) const
{
	wxLogMessage(_T("0 WriteScriptTxt called "));

	std::stringstream ret;

	LSL::TDF::TDFWriter tdf(ret);

	// Start generating the script.
	tdf.EnterSection("GAME");

	if (battle.IsFounderMe()) {
		tdf.AppendStr("HostIP", ""); //Listen on all addresses for connections when hosting
		if (battle.GetNatType() == NAT_Hole_punching)
			tdf.AppendInt("HostPort", battle.GetMyInternalUdpSourcePort());
		else
			tdf.AppendInt("HostPort", battle.GetHostPort());
	} else {
		tdf.AppendStr("HostIP", battle.GetHostIp());
		tdf.AppendInt("HostPort", battle.GetHostPort());
		if (battle.GetNatType() == NAT_Hole_punching) {
			tdf.AppendInt("SourcePort", battle.GetMyInternalUdpSourcePort());
		} else if (sett().GetClientPort() != 0) {
			tdf.AppendInt("SourcePort", sett().GetClientPort()); /// this allows to play with broken router by setting SourcePort to some forwarded port.
		}
	}
	tdf.AppendInt("IsHost", battle.IsFounderMe());

	User& me = battle.GetMe();
	tdf.AppendStr("MyPlayerName", me.GetNick());

	if (!me.BattleStatus().scriptPassword.empty()) {
		tdf.AppendStr("MyPasswd", me.BattleStatus().scriptPassword);
	}

	if (!battle.IsFounderMe()) {
		tdf.LeaveSection();
		return ret.str();
	}

	/**********************************************************************************
																Host-only section
	**********************************************************************************/

	tdf.AppendLineBreak();

	tdf.AppendStr("ModHash", battle.LoadGame().hash);
	tdf.AppendStr("MapHash", battle.LoadMap().hash);

	tdf.AppendStr("Mapname", battle.GetHostMapName());
	tdf.AppendStr("GameType", battle.GetHostGameNameAndVersion());

	tdf.AppendLineBreak();

	switch (battle.GetBattleType()) {
		case BT_Played:
			break;
		case BT_Replay: {
			wxString path(battle.GetPlayBackFilePath());
			if (path.Find(_T("/")) != wxNOT_FOUND)
				path.BeforeLast(_T('/'));
			tdf.AppendStr("DemoFile", STD_STRING(path));
			tdf.AppendLineBreak();
			break;
		}
		case BT_Savegame: {
			wxString path(battle.GetPlayBackFilePath());
			if (path.Find(_T("/")) != wxNOT_FOUND)
				path.BeforeLast(_T('/'));
			tdf.AppendStr("Savefile", STD_STRING(path));
			tdf.AppendLineBreak();
			break;
		}
		default:
			slLogDebugFunc("");
			break;
	}

	const long startpostype = LSL::Util::FromIntString(
	    battle.CustomBattleOptions().getSingleValue("startpostype", LSL::Enum::EngineOption));

	std::vector<LSL::StartPos> remap_positions;
	if (battle.IsProxy() && (startpostype != IBattle::ST_Pick) && (startpostype != IBattle::ST_Choose)) {
		std::set<int> parsedteams;
		unsigned int NumUsers = battle.GetNumUsers();
		unsigned int NumTeams = 0;
		for (unsigned int i = 0; i < NumUsers; i++) {
			User& usr = battle.GetUser(i);
			UserBattleStatus& status = usr.BattleStatus();
			if (status.spectator)
				continue;
			if (parsedteams.find(status.team) != parsedteams.end())
				continue; // skip duplicates
			parsedteams.insert(status.team);
			NumTeams++;
		}

		LSL::MapInfo infos = battle.LoadMap().info;
		unsigned int nummapstartpositions = infos.positions.size();
		unsigned int copysize = std::min(nummapstartpositions, NumTeams);
		remap_positions = std::vector<LSL::StartPos>(infos.positions.begin(), infos.positions.begin() + copysize); // only add the first x positions

		if (startpostype == IBattle::ST_Random) {
			std::random_device seed;
			std::mt19937 rng(seed());
			std::shuffle(remap_positions.begin(), remap_positions.end(), rng); // shuffle the positions
		}
	}
	if (battle.IsProxy()) {
		if ((startpostype == IBattle::ST_Random) || (startpostype == IBattle::ST_Fixed)) {
			tdf.AppendInt("startpostype", IBattle::ST_Pick);
		} else
			tdf.AppendInt("startpostype", startpostype);
	} else
		tdf.AppendInt("startpostype", startpostype);

	tdf.EnterSection("mapoptions");
	for (const auto& it : battle.CustomBattleOptions().getOptions(LSL::Enum::MapOption)) {
		tdf.AppendStr(it.first, it.second.second);
	}
	tdf.LeaveSection();


	tdf.EnterSection("modoptions");
	tdf.AppendInt("relayhoststartpostype", startpostype); // also save the original wanted setting
	for (const auto& it : battle.CustomBattleOptions().getOptions(LSL::Enum::ModOption)) {
		tdf.AppendStr(it.first, it.second.second);
	}
	tdf.LeaveSection();

	const std::map<std::string, int>& units = battle.GetRestrictedUnits();
	tdf.AppendInt("NumRestrictions", units.size());
	tdf.EnterSection("RESTRICT");
	int restrictcount = 0;
	for (std::map<std::string, int>::const_iterator itor = units.begin(); itor != units.end(); ++itor) {
		tdf.AppendStr(stdprintf("Unit%d", restrictcount), itor->first);
		tdf.AppendInt(stdprintf("Limit%d", restrictcount), itor->second);
		restrictcount++;
	}
	tdf.LeaveSection();


	tdf.AppendLineBreak();

	if (battle.IsProxy()) {
		tdf.AppendInt("NumPlayers", battle.GetNumPlayers() - 1);
		tdf.AppendInt("NumUsers", battle.GetNumUsers() - 1);
	} else {
		tdf.AppendInt("NumPlayers", battle.GetNumPlayers());
		tdf.AppendInt("NumUsers", battle.GetNumUsers());
	}

	tdf.AppendLineBreak();

	unsigned int NumUsers = battle.GetNumUsers();

	typedef std::map<int, int> ProgressiveTeamsVec;
	typedef ProgressiveTeamsVec::iterator ProgressiveTeamsVecIter;
	ProgressiveTeamsVec teams_to_sorted_teams; // original team -> progressive team
	int free_team = 0;
	std::map<User*, int> player_to_number; // player -> ordernumber
	srand(time(NULL));
	for (unsigned int i = 0; i < NumUsers; i++) {
		User& user = battle.GetUser(i);
		UserBattleStatus& status = user.BattleStatus();
		if (!status.spectator) {
			ProgressiveTeamsVecIter itor = teams_to_sorted_teams.find(status.team);
			if (itor == teams_to_sorted_teams.end()) {
				teams_to_sorted_teams[status.team] = free_team;
				free_team++;
			}
		}
		if (battle.IsProxy() && (user.GetNick() == battle.GetFounder().GetNick()))
			continue;
		if (status.IsBot())
			continue;
		tdf.EnterSection(stdprintf("PLAYER%d", i));
		tdf.AppendStr("Name", user.GetNick());
		tdf.AppendStr("CountryCode", STD_STRING(TowxString(user.GetCountry()).Lower()));
		tdf.AppendInt("Spectator", status.spectator);
		tdf.AppendInt("Rank", user.GetRank());
		tdf.AppendInt("IsFromDemo", status.isfromdemo);
		if (!status.scriptPassword.empty()) {
			tdf.AppendStr("Password", status.scriptPassword);
		}
		if (!status.spectator) {
			tdf.AppendInt("Team", teams_to_sorted_teams[status.team]);
		} else {
			int speccteam = 0;
			if (!teams_to_sorted_teams.empty())
				speccteam = rand() % teams_to_sorted_teams.size();
			tdf.AppendInt("Team", speccteam);
		}
		tdf.LeaveSection();
		player_to_number[&user] = i;
	}
	for (unsigned int i = 0; i < NumUsers; i++) {
		User& user = battle.GetUser(i);
		UserBattleStatus& status = user.BattleStatus();
		if (!status.IsBot())
			continue;
		tdf.EnterSection(stdprintf("AI%d", i));
		tdf.AppendStr("Name", user.GetNick());		// AI's nick;
		tdf.AppendStr("ShortName", status.aishortname); // AI libtype
		tdf.AppendStr("Version", status.aiversion);     // AI libtype version
		tdf.AppendInt("Team", teams_to_sorted_teams[status.team]);
		tdf.AppendInt("IsFromDemo", int(status.isfromdemo));
		tdf.AppendInt("Host", player_to_number[&battle.GetUser(status.owner)]);
		tdf.EnterSection("Options");
		int optionmapindex = battle.CustomBattleOptions().GetAIOptionIndex(user.GetNick());
		if (optionmapindex > 0) {
			for (const auto& it : battle.CustomBattleOptions().getOptions((LSL::Enum::GameOption)optionmapindex)) {
				tdf.AppendStr(it.first, it.second.second);
			}
		}
		tdf.LeaveSection();
		tdf.LeaveSection();
		player_to_number[&user] = i;
	}

	tdf.AppendLineBreak();

	std::set<int> parsedteams;
	const auto sides = LSL::usync().GetSides(battle.GetHostGameNameAndVersion());
	for (unsigned int i = 0; i < NumUsers; i++) {
		User& usr = battle.GetUser(i);
		UserBattleStatus& status = usr.BattleStatus();
		if (status.spectator)
			continue;
		if (parsedteams.find(status.team) != parsedteams.end())
			continue; // skip duplicates
		parsedteams.insert(status.team);

		tdf.EnterSection(stdprintf("TEAM%d", teams_to_sorted_teams[status.team]));
		if (status.IsBot()) {
			tdf.AppendInt("TeamLeader", player_to_number[&battle.GetUser(status.owner)]);
		} else {
			tdf.AppendInt("TeamLeader", player_to_number[&usr]);
		}
		if (battle.IsProxy()) {
			if (startpostype == IBattle::ST_Pick) {
				tdf.AppendInt("StartPosX", status.pos.x);
				tdf.AppendInt("StartPosZ", status.pos.y);
			} else if ((startpostype == IBattle::ST_Fixed) || (startpostype == IBattle::ST_Random)) {
				int teamnumber = teams_to_sorted_teams[status.team];
				if (teamnumber < int(remap_positions.size())) { // don't overflow
					LSL::StartPos position = remap_positions[teamnumber];
					tdf.AppendInt("StartPosX", position.x);
					tdf.AppendInt("StartPosZ", position.y);
				}
			}
		} else {
			if (startpostype == IBattle::ST_Pick) {
				tdf.AppendInt("StartPosX", status.pos.x);
				tdf.AppendInt("StartPosZ", status.pos.y);
			}
		}

		tdf.AppendInt("AllyTeam", status.ally);
		tdf.AppendStr("RGBColor", LSL::lslColor::ToFloatString(status.colour));

		unsigned int side = status.side;
		if (side < sides.size())
			tdf.AppendStr("Side", sides[side]);
		tdf.AppendInt("Handicap", status.handicap);
		tdf.LeaveSection();
	}

	tdf.AppendLineBreak();

	unsigned int maxiter = std::max(NumUsers, battle.GetLastRectIdx() + 1);
	std::set<int> parsedallys;
	for (unsigned int i = 0; i < maxiter; i++) {

		User& usr = battle.GetUser(i);
		UserBattleStatus& status = usr.BattleStatus();
		BattleStartRect sr = battle.GetStartRect(i);
		if (status.spectator && !sr.IsOk())
			continue;
		int ally = status.ally;
		if (status.spectator)
			ally = i;
		if (parsedallys.find(ally) != parsedallys.end())
			continue; // skip duplicates
		sr = battle.GetStartRect(ally);
		parsedallys.insert(ally);

		tdf.EnterSection(stdprintf("ALLYTEAM%d", ally));
		tdf.AppendInt("NumAllies", 0);
		if (startpostype == IBattle::ST_Choose) {
			if (sr.IsOk()) {
				const char* old_locale = std::setlocale(LC_NUMERIC, "C");

				tdf.AppendStr("StartRectLeft", stdprintf("%.3f", sr.left / 200.0));
				tdf.AppendStr("StartRectTop", stdprintf("%.3f", sr.top / 200.0));
				tdf.AppendStr("StartRectRight", stdprintf("%.3f", sr.right / 200.0));
				tdf.AppendStr("StartRectBottom", stdprintf("%.3f", sr.bottom / 200.0));

				std::setlocale(LC_NUMERIC, old_locale);
			}
		}
		tdf.LeaveSection();
	}

	tdf.LeaveSection();
	return ret.str();
}
