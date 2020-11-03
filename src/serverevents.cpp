/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

//
// Class: ServerEvents
//

#include "serverevents.h"

#ifdef _MSC_VER
#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX
#include <winsock2.h>
#endif // _MSC_VER

#include <lslutils/globalsmanager.h>
#include <wx/intl.h>
#include <wx/log.h>
#include <exception>
#include <locale>
#include <sstream>
#include <stdexcept>

#include "autohost.h"
#include "autohostmanager.h"
#include "channel.h"
#include "gui/customdialogs.h"
#include "gui/mainwindow.h"
#include "gui/ui.h"
#include "gui/uiutils.h"
#include "iserver.h"
#include "log.h"
#include "servermanager.h"
#include "settings.h"
#include "user.h"
#include "utils/conversion.h"
#include "utils/globalevents.h"
#include "utils/tasutil.h"
#include "utils/uievents.h"

void ServerEvents::OnConnected(const std::string& server_name, const std::string& server_ver, bool supported, const std::string& server_spring_ver, bool /*unused*/)
{
	wxLogInfo("Connected to %s %s %s", m_serv.m_serverinfo.hostname.c_str(), server_ver.c_str(), server_spring_ver.c_str());
	//Server version will include patchlevel from release 89 onwards
	m_serv.SetRequiredSpring(LSL::Util::BeforeFirst(server_spring_ver, "."));
	ui().OnConnected(m_serv, TowxString(server_name), TowxString(server_spring_ver), supported);
	m_serv.Login();
}


void ServerEvents::OnDisconnected(bool wasonline)
{
	wxLogInfo("Disconnected from server: %s %d", m_serv.m_serverinfo.hostname.c_str(), wasonline);
	m_serv.SetRequiredSpring("");
	try {
		ui().OnDisconnected(m_serv, wasonline);
	} catch (LSL::Util::GlobalDestroyedError&) {
		/* At the end of the program, the global reference in ui() might already have been nullified. */
	}
}


void ServerEvents::OnLogin()
{
}


void ServerEvents::OnLoginInfoComplete()
{
	slLogDebugFunc("");
	wxString nick = TowxString(m_serv.GetMe().GetNick());
	wxArrayString highlights = sett().GetHighlightedWords();
	if (highlights.Index(nick) == -1) {
		highlights.Add(nick);
		sett().SetHighlightedWords(highlights);
	}
	//m_serv.RequestChannels();
	GlobalEventManager::Instance()->Send(GlobalEventManager::OnLogin);
	std::vector<ChannelJoinInfo> autojoin = sett().GetChannelsJoin();
	for (std::vector<ChannelJoinInfo>::const_iterator itor = autojoin.begin(); itor != autojoin.end(); ++itor) {
		if (itor->name.IsEmpty())
			continue;
		Channel& chan = m_serv._AddChannel(STD_STRING(itor->name));
		chan.SetPassword(STD_STRING(itor->password));
		ui().OnJoinedChannelSuccessful(chan, itor == autojoin.begin());
	}
	for (std::vector<ChannelJoinInfo>::const_iterator itor = autojoin.begin(); itor != autojoin.end(); ++itor)
		m_serv.JoinChannel(STD_STRING(itor->name), STD_STRING(itor->password));

	ui().OnLoggedIn();
}


void ServerEvents::OnUnknownCommand(const std::string& command, const std::string& params)
{
	slLogDebugFunc("");
	ui().OnUnknownCommand(m_serv, TowxString(command), TowxString(params));
}


void ServerEvents::OnSocketError(const Sockerror& /*unused*/)
{
	//wxLogDebugFunc( wxEmptyString );
}


void ServerEvents::OnProtocolError(const Protocolerror /*unused*/)
{
	//wxLogDebugFunc( wxEmptyString );
}


void ServerEvents::OnMotd(const std::string& msg)
{
	slLogDebugFunc("");
	ui().OnMotd(m_serv, TowxString(msg));
}


void ServerEvents::OnPong(wxLongLong ping_time)
{
	//wxLongLong is non-POD and cannot be passed to wxString::Format as such. use c-string rep instead. converting to long might loose precision
	UiEvents::StatusData data(wxString::Format(_("ping: %s ms"), ping_time.ToString().c_str()), 2);
	UiEvents::GetStatusEventSender(UiEvents::addStatusMessage).SendEvent(data);
}


void ServerEvents::OnNewUser(const std::string& nick, const std::string& country, int id, const std::string& clientAgent)
{
	slLogDebugFunc("");
	if (m_serv.UserExists(nick)) {
		wxLogWarning("New user from server, but already exists: %s", nick.c_str());
		return;
	}
	User& user = m_serv._AddUser(nick);
	user.SetCountry(country);
	user.SetID(id);
	user.SetClientAgent(clientAgent);
	if (!m_serv.IsOnline()) { //login info isn't complete yet
		return;
	}
	ui().OnUserOnline(user);

	if (useractions().DoActionOnUser(UserActions::ActNotifLogin, TowxString(nick))) {
		actNotifBox(SL_MAIN_ICON, TowxString(nick) + _(" just connected"));
	}
}


void ServerEvents::OnUserStatus(const std::string& nick, UserStatus status)
{
	slLogDebugFunc("");
	try {
		User& user = m_serv.GetUser(nick);

		UserStatus oldStatus = user.GetStatus();
		user.SetStatus(status);
		if (useractions().DoActionOnUser(UserActions::ActNotifStatus, TowxString(nick))) {
			wxString diffString = TowxString(status.GetDiffString(oldStatus));
			if (diffString != wxEmptyString)
				actNotifBox(SL_MAIN_ICON, TowxString(nick) + _(" is now ") + diffString);
		}

		if (m_serv.IsOnline()) { //login info isn't complete yet
			ui().OnUserStatusChanged(user);
		}

		if (user.GetBattle() != 0) {
			IBattle& battle = *user.GetBattle();
			try {
				if (battle.GetFounder().GetNick() == user.GetNick()) {
					if (status.in_game != battle.GetInGame()) {
						battle.SetInGame(status.in_game);
						if (m_serv.IsOnline()) {
							if (status.in_game) {
								battle.StartSpring();
							} else {
								ui().OnBattleInfoUpdated(battle, wxEmptyString);
							}
						}
					}
				}
			} catch (const std::exception& e) {
				wxLogWarning(_T("Exception: %s"), e.what());
			}
		}
	} catch (const std::exception& e) {
		wxLogWarning(_T("Exception: %s"), e.what());
	}
}


void ServerEvents::OnUserQuit(const std::string& nick)
{
	slLogDebugFunc("");
	try {
		User& user = m_serv.GetUser(nick);
		IBattle* userbattle = user.GetBattle();
		if (userbattle) {
			int battleid = userbattle->GetID();
			try {
				if (&userbattle->GetFounder() == &user) {
					for (int i = 0; i < int(userbattle->GetNumUsers()); i++) {
						User& battleuser = userbattle->GetUser(i);
						OnUserLeftBattle(battleid, battleuser.GetNick());
					}
					OnBattleClosed(battleid);
				} else
					OnUserLeftBattle(battleid, user.GetNick());
			} catch (const std::exception& e) {
				wxLogWarning(_T("Exception: %s"), e.what());
			}
		}
		ui().OnUserOffline(user);
		m_serv._RemoveUser(nick);
		if (useractions().DoActionOnUser(UserActions::ActNotifLogin, TowxString(nick)))
			actNotifBox(SL_MAIN_ICON, TowxString(nick) + _(" just went offline"));
	} catch (const std::runtime_error& e) {
		wxLogWarning(_T("Exception: %s"), e.what());
	}
}


void ServerEvents::OnBattleOpened(int id, BattleType type, NatType nat, const std::string& nick,
				  const std::string& host, int port, int maxplayers,
				  bool haspass, int rank, const std::string& maphash, const std::string& engineName, const std::string& engineVersion, const std::string& map,
				  const std::string& title, const std::string& mod,
				  const std::string& channelName)
{
	slLogDebugFunc("");
	try {
		ASSERT_EXCEPTION(!m_serv.BattleExists(id), _T("New battle from server, but already exists!"));
		IBattle& battle = m_serv._AddBattle(id, channelName);

		User& user = m_serv.GetUser(nick);
		battle.OnUserAdded(user);

		battle.SetBattleType(type);
		battle.SetNatType(nat);
		battle.SetFounder(nick);
		battle.SetHostIp(host);
		battle.SetHostPort(port);
		battle.SetMaxPlayers(maxplayers);
		battle.SetIsPassworded(haspass);
		battle.SetRankNeeded(rank);
		battle.SetHostMap(map, maphash);
		battle.SetDescription(title);
		battle.SetHostGame(mod, "");
		battle.SetEngineName(engineName);
		battle.SetEngineVersion(engineVersion);
		if (useractions().DoActionOnUser(UserActions::ActNotifBattle, TowxString(user.GetNick()))) {
			actNotifBox(SL_MAIN_ICON, TowxString(user.GetNick()) + _(" opened battle ") + TowxString(title));
		}
		if (!m_serv.IsOnline()) { //login info isn't complete yet
			return;
		}

		ui().OnBattleOpened(battle);
		if (user.Status().in_game) {
			battle.SetInGame(true);
			battle.StartSpring();
		}
	} catch (const std::runtime_error& e) {
		wxLogWarning(_T("Exception: %s"), e.what());
	}
}


void ServerEvents::OnJoinedBattle(int battleid, const std::string& hash)
{
	slLogDebugFunc("");
	try {
		IBattle& battle = m_serv.GetBattle(battleid);

		battle.SetHostGame(battle.GetHostGameNameAndVersion(), hash);
		UserBattleStatus& bs = m_serv.GetMe().BattleStatus();
		bs.spectator = false;

		if (!battle.IsFounderMe() || battle.IsProxy()) {
			battle.CustomBattleOptions().loadOptions(LSL::Enum::MapOption, battle.GetHostMapName());
			battle.CustomBattleOptions().loadOptions(LSL::Enum::ModOption, battle.GetHostGameNameAndVersion());
		}

		ui().OnJoinedBattle(battle);
	} catch (const std::runtime_error& e) {
		wxLogWarning(_T("Exception: %s"), e.what());
	}
}


void ServerEvents::OnHostedBattle(int battleid)
{
	slLogDebugFunc("");
	try {
		IBattle& battle = m_serv.GetBattle(battleid);

		if (battle.GetBattleType() == BT_Played) {
			battle.CustomBattleOptions().loadOptions(LSL::Enum::MapOption, battle.GetHostMapName());
			battle.CustomBattleOptions().loadOptions(LSL::Enum::ModOption, battle.GetHostGameNameAndVersion());
		} else {
			battle.GetBattleFromScript(true);
		}


		const std::string presetname = STD_STRING(sett().GetModDefaultPresetName(TowxString(battle.GetHostGameNameAndVersion())));
		if (!presetname.empty()) {
			battle.LoadOptionsPreset(presetname);
		}

		battle.LoadMapDefaults(battle.GetHostMapName());

		m_serv.SendHostInfo(IBattle::HI_Send_All_opts);

		ui().OnHostedBattle(battle);
	} catch (const assert_exception& e) {
		wxLogWarning(_T("Exception: %s"), e.what());
	}
}


void ServerEvents::OnStartHostedBattle(int battleid)
{
	slLogDebugFunc("");
	IBattle& battle = m_serv.GetBattle(battleid);
	battle.SetInGame(true);
	battle.StartSpring();
}


void ServerEvents::OnClientBattleStatus(int battleid, const std::string& nick, UserBattleStatus status)
{
	try {
		IBattle& battle = m_serv.GetBattle(battleid);
		User& user = battle.GetUser(nick);

		//if ( battle.IsFounderMe() ) AutoCheckCommandSpam( battle, user );

		status.color_index = user.BattleStatus().color_index;
		battle.OnUserBattleStatusUpdated(user, status);
		ui().OnUserBattleStatus(user);
	} catch (const std::runtime_error& e) {
		wxLogWarning(_T("Exception: %s"), e.what());
	}
}


void ServerEvents::OnUserJoinedBattle(int battleid, const std::string& nick, const std::string& userScriptPassword)
{
	try {
		slLogDebugFunc("");
		if (!m_serv.UserExists(nick)) {
			wxLogWarning("OnUserJoinedBattle(): Lobby server inconsistency: user in battle which doesn't exist. Please report as lobby server bug!");
			return;
		}
		User& user = m_serv.GetUser(nick);
		IBattle& battle = m_serv.GetBattle(battleid);

		battle.OnUserAdded(user);
		user.BattleStatus().scriptPassword = userScriptPassword;
		ui().OnUserJoinedBattle(battle, user);
		try {
			if (&user == &battle.GetFounder()) {
				if (user.Status().in_game) {
					battle.SetInGame(true);
					battle.StartSpring();
				}
			}
		} catch (const std::exception& e) {
			wxLogWarning(_T("Exception: %s"), e.what());
		}
	} catch (const std::runtime_error& e) {
		wxLogWarning(_T("Exception: %s"), e.what());
	}
}


void ServerEvents::OnUserLeftBattle(int battleid, const std::string& nick)
{
	slLogDebugFunc("");
	try {
		IBattle& battle = m_serv.GetBattle(battleid);
		User& user = battle.GetUser(nick);
		// this is necessary since the user will be deleted when the gui function is called
		bool isbot = user.BattleStatus().IsBot();
		std::map<std::string, User*> user_map = battle.GetUsers();
		user.BattleStatus().scriptPassword.clear();
		battle.OnUserRemoved(user);
		ui().OnUserLeftBattle(battle, user, isbot);

		std::list<std::string> toremove;
		for (auto p : user_map) { // remove any bridged users that we no longer share channels with
			User& user = *p.second;
			if (!user.IsBridged())
				continue;
			std::string nick = p.first;
			if (!m_serv.UserIsOnBridge(nick)) {
				ui().OnUserOffline(user);
				toremove.push_back(nick);
			}
		}
		for (const std::string nick: toremove) {
			m_serv._RemoveUser(nick);
		}
	} catch (const std::runtime_error& e) {
		wxLogWarning(_T("Exception: %s"), e.what());
	}
}


void ServerEvents::OnBattleInfoUpdated(int battleid, int spectators, bool locked, const std::string& mapHash, const std::string& mapName)
{
	slLogDebugFunc("");
	try {
		IBattle& battle = m_serv.GetBattle(battleid);

		battle.SetSpectators(spectators);
		battle.SetIsLocked(locked);

		const std::string& oldMapName = battle.GetHostMapName();
		if (oldMapName != mapName) {
			battle.SetHostMap(mapName, mapHash);
			if (battle.UserExists(m_serv.GetMe().GetNick())) {
				battle.SendMyBattleStatus();
				battle.CustomBattleOptions().loadOptions(LSL::Enum::MapOption, mapName);
			}
		}

		ui().OnBattleInfoUpdated(battle, wxEmptyString);
	} catch (const assert_exception& e) {
		wxLogWarning(_T("Exception: %s"), e.what());
	}
}

bool IsCrap(char c)
{
	switch (c) {
		case '~':
		case '(':
		case ')':
		case '#':
			return true;
		default:
			return false;
	}
}

/*
parse TrueSkill values to a float:
https://springrts.com/phpbb/viewtopic.php?f=88&p=574686
FIXME: move to LSL
*/
static bool parseSkill(const std::string& value, double& result)
{
	if (value == "-")
		return false;

	std::string str = value;
	str.erase(std::remove_if(str.begin(), str.end(), &IsCrap), str.end());
	std::istringstream istr(str);
	istr.imbue(std::locale("C"));
	double res = -1.0f;
	istr >> res;
	result = res;
	return res != -1.0f;
}

static bool parseTeam(const std::string& value, int& result)
{
	const int res = sscanf(value.c_str(), "team%d", &result);
	return res == 1;
}

void ServerEvents::OnSetBattleInfo(int battleid, const std::string& param, const std::string& value)
{
	slLogDebugFunc("%s, %s", param.c_str(), value.c_str());
	IBattle& battle = m_serv.GetBattle(battleid);
	battle.m_script_tags[param] = value;
	const LSL::StringVector vec = LSL::Util::StringTokenize(param, "/"); //split string by slash

	switch (vec.size()) {
		case 3: { // depth 3
			if (param.find("game/mapoptions") == 0) {
				if (!battle.CustomBattleOptions().setSingleOption(vec[2], value, LSL::Enum::MapOption)) {
					wxLogWarning("OnSetBattleInfo: Couldn't set map option %s", vec[2].c_str());
				}
				return;
			}
			if (param.find("game/modoptions/") == 0) {
				if (!battle.CustomBattleOptions().setSingleOption(vec[2], value, LSL::Enum::ModOption)) {
					wxLogWarning("OnSetBattleInfo: Couldn't set game option %s", vec[2].c_str());
				}
				return;
			}
			if (param.find("game/restrict") == 0) {
				OnBattleDisableUnit(battleid, vec[2], LSL::Util::FromIntString(value));
				return;
			}
			if (param.find("game/") == 0) { //game/team0/startposx=1692.
				int team = -1;
				if (parseTeam(vec[1], team)) {
					const bool xpos = vec[2] == "startposx";
					const bool ypos = vec[2] == "startposy";
					if (xpos || ypos) {
						int numusers = battle.GetNumUsers();
						for (int i = 0; i < numusers; i++) {
							User& usr = battle.GetUser(i);
							UserBattleStatus& status = usr.BattleStatus();
							if (status.team == team) {
								if (xpos) {
									status.pos.x = LSL::Util::FromIntString(value);
								}
								if (ypos) {
									status.pos.y = LSL::Util::FromIntString(value);
								}
								battle.OnUserBattleStatusUpdated(usr, status);
								ui().OnUserBattleStatus(usr);
							}
						}
						return;
					}
				}
			}
			break;
		}
		case 4: { //depth 4
			if (param.find("game/players/") == 0) {
				if (vec[3] == "skill") {
					const std::string nick = vec[2];
					double skill;
					if (parseSkill(value, skill)) {
						battle.OnPlayerTrueskillChanged(nick, skill); //(std::string& nickname, double trueskill_value)
					}
					return;
				}
				if (vec[3] == "skilluncertainty") { //this is ignored
					return;
				}
			}
			break;
		}
		case 2: { //depth 2
			if (param == "game/hosttype") {
				if (battle.m_autohost_manager == nullptr) {
					wxLogWarning("FIXME: battle.m_autohost_manager == nullptr");
					return;
				}
				if (battle.m_autohost_manager->RecognizeAutohost(value)) {
					wxLogInfo("detected %s autohost", value.c_str()); //FIXME: add event for that + add a label?!
				}
				return;
			}
			// i.e. game/startpostype
			battle.CustomBattleOptions().setSingleOption(vec[1], value, LSL::Enum::EngineOption);
			return;
		}
			/*
		//seems unused
		case 1: { //depth 1
			battle.CustomBattleOptions().setSingleOption(vec[0], value, LSL::Enum::EngineOption);
			battle.Update(stdprintf("%d_%s", LSL::Enum::EngineOption, vec[0].c_str()));
			return;
		}
*/
	}
	wxLogWarning("Unhandled SETSCRIPTTAGS: %s=%s", param.c_str(), value.c_str());
}

void ServerEvents::OnUnsetBattleInfo(int /*battleid*/, const std::string& /*param*/)
{
	//FIXME: implement this
}


void ServerEvents::OnBattleInfoUpdated(int battleid)
{
	slLogDebugFunc("");
	try {
		IBattle& battle = m_serv.GetBattle(battleid);
		ui().OnBattleInfoUpdated(battle, wxEmptyString);
	} catch (const assert_exception& e) {
		wxLogWarning(_T("Exception in OnBattleInfoUpdated(%d): %s"), battleid, e.what());
	}
}


void ServerEvents::OnBattleClosed(int battleid)
{
	slLogDebugFunc("");
	try {
		if (!m_serv.BattleExists(battleid)) {
			wxLogWarning("Tried to close non-existing battle %d", battleid);
			return;
		}
		IBattle& battle = m_serv.GetBattle(battleid);
		ui().OnBattleClosed(battle);
		m_serv._RemoveBattle(battleid);
	} catch (const assert_exception& e) {
		wxLogWarning(_T("Exception in OnBattleClosed(%d): %s"), battleid, e.what());
	}
}


void ServerEvents::OnBattleDisableUnit(int battleid, const std::string& unitname, int count)
{
	slLogDebugFunc("");
	try {
		IBattle& battle = m_serv.GetBattle(battleid);
		battle.RestrictUnit(unitname, count);
		wxLogDebug("OnBattleDisableUnit %d %s %d", battleid, unitname.c_str(), count);
	} catch (const assert_exception& e) {
		wxLogWarning(_T("Exception: %s"), e.what());
	}
}


void ServerEvents::OnBattleEnableUnit(int battleid, const std::string& unitname)
{
	slLogDebugFunc("");
	try {
		IBattle& battle = m_serv.GetBattle(battleid);
		battle.UnrestrictUnit(unitname);
	} catch (const assert_exception& e) {
		wxLogWarning(_T("Exception: %s"), e.what());
	}
}


void ServerEvents::OnBattleEnableAllUnits(int battleid)
{
	slLogDebugFunc("");
	try {
		IBattle& battle = m_serv.GetBattle(battleid);
		battle.UnrestrictAllUnits();
	} catch (const assert_exception& e) {
		wxLogWarning(_T("Exception: %s"), e.what());
	}
}


void ServerEvents::OnJoinChannelResult(bool success, const std::string& channel, const std::string& reason)
{
	slLogDebugFunc("");
	if (success) {
		int battleid = m_serv.m_battles.BattleFromChannel(channel);
		if (battleid != -1) // if this channel is associated to a battle, don't show it as a standlone channel
			return;
		Channel& chan = m_serv._AddChannel(channel);
		chan.SetPassword(m_serv.m_channel_pw[channel]);
		ui().OnJoinedChannelSuccessful(chan);

	} else {
		ui().ShowMessage(_("Join channel failed"), _("Could not join channel ") + TowxString(channel) + _(" because: ") + TowxString(reason));
	}
}


void ServerEvents::OnChannelSaid(const std::string& channel, const std::string& who, const std::string& message)
{
	slLogDebugFunc("");
	try {
		int battleid = m_serv.m_battles.BattleFromChannel(channel);
		if (battleid != -1) {
			OnBattleSaid(battleid, who, message);
			return;
		}
		if ((m_serv.GetMe().GetNick() == who) || !useractions().DoActionOnUser(UserActions::ActIgnoreChat, TowxString(who))) {
			if (m_serv.UserExists(who)) {
				m_serv.GetChannel(channel).Said(m_serv.GetUser(who), message);
			} else {
				User u(who);
				m_serv.GetChannel(channel).Said(u, message); // offline message
			}
		}
	} catch (const std::runtime_error& e) {
		wxLogWarning(_T("Exception: %s"), e.what());
	}
}

void ServerEvents::OnBattleSaid(int battleid, const std::string& nick, const std::string& msg)
{
	try {
		IBattle& battle = m_serv.GetBattle(battleid);
		if ((m_serv.GetMe().GetNick() == nick) || !useractions().DoActionOnUser(UserActions::ActIgnoreChat, TowxString(nick))) {
			ui().OnSaidBattle(battle, TowxString(nick), TowxString(msg));
		}
		AutoHost* ah = battle.GetAutoHost();
		if (ah != NULL) {
			ah->OnSaidBattle(TowxString(nick), TowxString(msg));
		}
	} catch (const assert_exception& e) {
		wxLogWarning(_T("Exception: %s"), e.what());
	}
}


void ServerEvents::OnChannelJoin(const std::string& channel, const std::string& who)
{
	slLogDebugFunc("");
	try {
		int battleid = m_serv.m_battles.BattleFromChannel(channel);
		if (battleid != -1)
			return;
		m_serv.GetChannel(channel).OnChannelJoin(m_serv.GetUser(who));
	} catch (const std::runtime_error& e) {
		wxLogWarning(_T("Exception: %s"), e.what());
	}
}


void ServerEvents::OnChannelPart(const std::string& channel, const std::string& who, const std::string& message)
{
	slLogDebugFunc("");
	try {
		int battleid = m_serv.m_battles.BattleFromChannel(channel);
		if (battleid != -1)
			return;
		Channel& chan = m_serv.GetChannel(channel);
		chan.Left(m_serv.GetUser(who), message);

		std::map<std::string, User*> user_map = chan.GetUsers();
		for (auto p : user_map) { // remove any bridged users that we no longer share channels with
			User& user = *p.second;
			if (!user.IsBridged())
				continue;
			std::string nick = p.first;
			if (!m_serv.UserIsOnBridge(nick)) {
				ui().OnUserOffline(user);
				m_serv._RemoveUser(nick);
			}
		}
	} catch (const std::runtime_error& e) {
		wxLogWarning(_T("Exception: %s"), e.what());
	}
}


void ServerEvents::OnChannelTopic(const std::string& channel, const std::string& who, const std::string& message)
{
	slLogDebugFunc("");
	try {
		int battleid = m_serv.m_battles.BattleFromChannel(channel);
		if (battleid != -1) {
			IBattle& battle = m_serv.GetBattle(battleid);
			ui().OnBattleTopic(battle, message, who);
			return;
		}
		m_serv.GetChannel(channel).SetTopic(message, who);
	} catch (const std::runtime_error& e) {
		wxLogWarning(_T("Exception: %s"), e.what());
	}
}


void ServerEvents::OnChannelAction(const std::string& channel, const std::string& who, const std::string& action)
{
	slLogDebugFunc("");
	try {
		int battleid = m_serv.m_battles.BattleFromChannel(channel);
		if (battleid != -1) {
			OnBattleAction(battleid, who, action);
			return;
		}
		if ((m_serv.GetMe().GetNick() == who) || !useractions().DoActionOnUser(UserActions::ActIgnoreChat, TowxString(who)))
			m_serv.GetChannel(channel).DidAction(m_serv.GetUser(who), action);
	} catch (const std::runtime_error& e) {
		wxLogWarning(_T("Exception: %s"), e.what());
	}
}

void ServerEvents::OnBattleAction(int /*battleid*/, const std::string& nick, const std::string& action)
{
	try {
		UiEvents::GetUiEventSender(UiEvents::OnBattleActionEvent).SendEvent(UiEvents::OnBattleActionData(TowxString(nick), TowxString(action)));
	} catch (const assert_exception& e) {
		wxLogWarning(_T("Exception: %s"), e.what());
	}
}


void ServerEvents::OnPrivateMessage(User& chan, User& who, const std::string& message)
{
	slLogDebugFunc("");
	try {
		if (!useractions().DoActionOnUser(UserActions::ActIgnorePM, TowxString(who.GetNick())))
			ui().OnUserSaid(chan, who, TowxString(message));
	} catch (const std::runtime_error& e) {
		wxLogWarning(_T("Exception: %s"), e.what());
	}
}

void ServerEvents::OnPrivateMessageEx(User& chan, User& who, const std::string& action)
{
	slLogDebugFunc("");
	try {
		if (!useractions().DoActionOnUser(UserActions::ActIgnorePM, TowxString(who.GetNick())))
			ui().OnUserSaidEx(chan, who, TowxString(action));
	} catch (const std::runtime_error& e) {
		wxLogWarning(_T("Exception: %s"), e.what());
	}
}

void ServerEvents::OnChannelList(const std::string& channel, const int& numusers, const std::string& topic)
{
	ui().mw().OnChannelList(TowxString(channel), numusers, TowxString(topic));
}


void ServerEvents::OnUserJoinChannel(const std::string& channel, const std::string& who)
{
	slLogDebugFunc("");
	try {
		int battleid = m_serv.m_battles.BattleFromChannel(channel);
		if (battleid != -1) // don't display as a channel, if its a battle
			return;
		m_serv.GetChannel(channel).Joined(m_serv.GetUser(who));
	} catch (const std::runtime_error& e) {
		wxLogWarning(_T("Exception: %s"), e.what());
	}
}


void ServerEvents::OnRequestBattleStatus(int battleid)
{
	try {
		IBattle& battle = m_serv.GetBattle(battleid);
		battle.OnRequestBattleStatus();
	} catch (const assert_exception& e) {
		wxLogWarning(_T("Exception: %s"), e.what());
	}
}


void ServerEvents::OnBattleStartRectAdd(int battleid, int allyno, int left, int top, int right, int bottom)
{
	try {
		IBattle& battle = m_serv.GetBattle(battleid);
		battle.AddStartRect(allyno, left, top, right, bottom);
		battle.StartRectAdded(allyno);
	} catch (const assert_exception& e) {
		wxLogWarning(_T("Exception: %s"), e.what());
	}
}


void ServerEvents::OnBattleStartRectRemove(int battleid, int allyno)
{
	try {
		IBattle& battle = m_serv.GetBattle(battleid);
		battle.RemoveStartRect(allyno);
		battle.StartRectRemoved(allyno);
	} catch (const assert_exception& e) {
		wxLogWarning(_T("Exception: %s"), e.what());
	}
}


void ServerEvents::OnBattleAddBot(int battleid, const std::string& nick, UserBattleStatus status)
{
	slLogDebugFunc("");
	try {
		IBattle& battle = m_serv.GetBattle(battleid);
		battle.OnBotAdded(nick, status);
		User& bot = battle.GetUser(nick);
		ui().OnUserJoinedBattle(battle, bot);
	} catch (const assert_exception& e) {
		wxLogWarning(_T("Exception: %s"), e.what());
	}
}

void ServerEvents::OnBattleUpdateBot(int battleid, const std::string& nick, UserBattleStatus status)
{
	OnClientBattleStatus(battleid, nick, status);
}


void ServerEvents::OnBattleRemoveBot(int battleid, const std::string& nick)
{
	slLogDebugFunc("");
	try {
		IBattle& battle = m_serv.GetBattle(battleid);
		User& user = battle.GetUser(nick);
		bool isbot = user.BattleStatus().IsBot();
		ui().OnUserLeftBattle(battle, user, isbot);
		battle.OnUserRemoved(user);
	} catch (const std::runtime_error& e) {
		wxLogWarning(_T("Exception: %s"), e.what());
	}
}


void ServerEvents::OnAcceptAgreement(const std::string& agreement)
{
	ui().OnAcceptAgreement(TowxString(agreement));
}


void ServerEvents::OnRing(const std::string& from)
{
	ui().OnRing(TowxString(from));
}

void ServerEvents::OnServerBroadcast(const std::string& message)
{
	wxLogWarning("%s", message);
	ui().OnServerBroadcast(m_serv, TowxString(message));
}

void ServerEvents::OnServerMessage(const std::string& message)
{
	wxLogWarning("%s", message);
	ui().OnServerMessage(m_serv, TowxString(message));
}


void ServerEvents::OnServerMessageBox(const std::string& message)
{
	wxLogWarning("%s", message);
	ui().ShowMessage(_("Server Message"), TowxString(message));
}


void ServerEvents::OnChannelMessage(const std::string& channel, const std::string& msg)
{
	try {
		int battleid = m_serv.m_battles.BattleFromChannel(channel);
		if (battleid != -1) {
			IBattle& battle = m_serv.GetBattle(battleid);
			ui().OnBattleMessage(battle, msg);
			return; // FIXME: send topic to battle
		}
		ui().OnChannelMessage(m_serv.GetChannel(channel), msg);
	} catch (const std::runtime_error& e) {
		wxLogWarning(_T("Exception: (\"%s\", \"%s\"): %s"), channel.c_str(), msg.c_str(), e.what());
	}
}


void ServerEvents::OnJoinedFrom(const std::string& channel, const std::string& who)
{
	slLogDebugFunc("");
	try {
		int battleid = m_serv.m_battles.BattleFromChannel(channel);
		if (battleid != -1) // don't display as a channel, if its a battle
		{
			User& user = m_serv.GetUser(who);
			IBattle& battle = m_serv.GetBattle(battleid);
			battle.OnUserAdded(user);
			battle.ForceSpectator(user, true); // bridged users have no battle status
			ui().OnUserJoinedBattle(battle, user);
			return;
		}
		m_serv.GetChannel(channel).Joined(m_serv.GetUser(who));
	} catch (const std::runtime_error& e) {
		wxLogWarning(_T("Exception: %s"), e.what());
	}
}

void ServerEvents::OnLeftFrom(const std::string& channel, const std::string& who)
{
	slLogDebugFunc("");
	try {
		int battleid = m_serv.m_battles.BattleFromChannel(channel);
		if (battleid != -1) {
			IBattle& battle = m_serv.GetBattle(battleid);
			User& user = battle.GetUser(who);
			// this is necessary since the user will be deleted when the gui function is called
			bool isbot = user.BattleStatus().IsBot();
			user.BattleStatus().scriptPassword.clear();
			battle.OnUserRemoved(user);
			ui().OnUserLeftBattle(battle, user, isbot);
		} else {
			m_serv.GetChannel(channel).Left(m_serv.GetUser(who), "");
		}

		if (!m_serv.UserIsOnBridge(who)) {
			User& user = m_serv.GetUser(who);
			ui().OnUserOffline(user);
			m_serv._RemoveUser(who);
		}
	} catch (const std::runtime_error& e) {
		wxLogWarning(_T("Exception: %s"), e.what());
	}
}

void ServerEvents::OnSaidFrom(const std::string& channel, const std::string& who, const std::string& message)
{
	slLogDebugFunc("");
	try {
		if (not((m_serv.GetMe().GetNick() == who) || !useractions().DoActionOnUser(UserActions::ActIgnoreChat, TowxString(who))))
			return;
		int battleid = m_serv.m_battles.BattleFromChannel(channel);
		if (battleid != -1) {
			OnBattleSaid(battleid, who, message);
			return;
		}
		m_serv.GetChannel(channel).Said(m_serv.GetUser(who), message);
	} catch (const std::runtime_error& e) {
		wxLogWarning(_T("Exception: %s"), e.what());
	}
}

void ServerEvents::OnHostExternalUdpPort(const unsigned int udpport)
{
	if (!m_serv.GetCurrentBattle())
		return;
	if (m_serv.GetCurrentBattle()->GetNatType() == NAT_Hole_punching || m_serv.GetCurrentBattle()->GetNatType() == NAT_Fixed_source_ports)
		m_serv.GetCurrentBattle()->SetHostPort(udpport);
}


void ServerEvents::OnMyInternalUdpSourcePort(const unsigned int udpport)
{
	if (!m_serv.GetCurrentBattle())
		return;
	m_serv.GetCurrentBattle()->SetMyInternalUdpSourcePort(udpport);
}


void ServerEvents::OnMyExternalUdpSourcePort(const unsigned int udpport)
{
	if (!m_serv.GetCurrentBattle())
		return;
	m_serv.GetCurrentBattle()->SetMyExternalUdpSourcePort(udpport);
}

void ServerEvents::OnClientIPPort(const std::string& username, const std::string& ip, unsigned int udpport)
{
	wxLogMessage(_T("OnClientIPPort(%s,%s,%d)"), username.c_str(), ip.c_str(), udpport);
	if (!m_serv.GetCurrentBattle()) {
		wxLogMessage(_T("GetCurrentBattle() returned null"));
		return;
	}
	try {
		User& user = m_serv.GetCurrentBattle()->GetUser(username);

		user.BattleStatus().ip = ip;
		user.BattleStatus().udpport = udpport;
		wxLogMessage(_T("set to %s %d "), user.BattleStatus().ip.c_str(), user.BattleStatus().udpport);

		if (sett().GetShowIPAddresses()) {
			UiEvents::GetUiEventSender(UiEvents::OnBattleActionEvent).SendEvent(UiEvents::OnBattleActionData(TowxString(username), wxString::Format(_(" has ip=%s"), ip.c_str())));
		}

		if (m_serv.GetCurrentBattle()->GetNatType() != NAT_None && (udpport == 0)) {
			// todo: better warning message
			//something.OutputLine( _T(" ** ") + who.GetNick() + _(" does not support nat traversal! ") + GetChatTypeStr() + _T("."), sett().GetChatColorJoinPart(), sett().GetChatFont() );
			UiEvents::GetUiEventSender(UiEvents::OnBattleActionEvent).SendEvent(UiEvents::OnBattleActionData(TowxString(username), _(" does not really support nat traversal")));
		}
		m_serv.GetCurrentBattle()->CheckBan(user);
	} catch (const std::runtime_error& e) {
		wxLogMessage(_T("Exception: %s"), e.what());
	}
}


void ServerEvents::OnKickedFromBattle()
{
	customMessageBoxModal(SL_MAIN_ICON, _("You were kicked from the battle!"), _("Kicked by Host"));
}


void ServerEvents::OnRedirect(const std::string& address, unsigned int port, const std::string& CurrentNick, const std::string& CurrentPassword)
{
	const std::string name = stdprintf("%s:%d", address.c_str(), port);
	sett().SetServer(name, TowxString(address), port, "");
	//TODO: Rework this. Server calls it`s own method through another object!
	ServerManager::Instance()->DoConnectToServer(name, CurrentNick, CurrentPassword);
}


void ServerEvents::AutoCheckCommandSpam(IBattle& battle, User& user)
{
	const std::string nick = user.GetNick();
	MessageSpamCheck info = m_spam_check[nick];
	time_t now = time(0);
	if (info.lastmessage == now)
		info.count++;
	else
		info.count = 0;
	info.lastmessage = now;
	m_spam_check[nick] = info;
	if (info.count == 7) {
		battle.DoAction("is autokicking " + nick + " due to command spam.");
		battle.KickPlayer(user);
	}
}

void ServerEvents::OnMutelistBegin(const std::string& channel)
{
	mutelistWindow(wxString::Format(_("Begin mutelist for channel %s"), channel.c_str()), wxString::Format(_("%s mutelist"), channel.c_str()));
}

void ServerEvents::OnMutelistItem(const std::string& /*unused*/, const std::string& mutee, const std::string& description)
{
	wxString message = TowxString(mutee);
	wxString desc = TowxString(description);
	wxString mutetime = desc.AfterFirst(wxChar(' '));
	long time;
	if (mutetime == _T("indefinite"))
		message << _(" indefinite time remaining");
	else if (mutetime.ToLong(&time))
		message << wxString::Format(_(" %d minutes remaining"), time / 60 + 1);
	else
		message << mutetime;
	if (!desc.IsEmpty())
		message << _T(", ") << desc;
	mutelistWindow(message);
}

void ServerEvents::OnMutelistEnd(const std::string& channel)
{
	mutelistWindow(_("End mutelist for ") + TowxString(channel));
}

void ServerEvents::OnScriptStart(int battleid)
{
	if (!m_serv.BattleExists(battleid)) {
		return;
	}
	m_serv.GetBattle(battleid).ClearScript();
}

void ServerEvents::OnScriptLine(int battleid, const std::string& line)
{
	if (!m_serv.BattleExists(battleid)) {
		return;
	}
	m_serv.GetBattle(battleid).AppendScriptLine(line);
}

void ServerEvents::OnScriptEnd(int battleid)
{
	if (!m_serv.BattleExists(battleid)) {
		return;
	}
	m_serv.GetBattle(battleid).GetBattleFromScript(true);
}

void ServerEvents::OnForceJoinBattle(int battleid, const std::string& scriptPW)
{
	IBattle* battle = m_serv.GetCurrentBattle();
	if (battle != NULL) {
		m_serv.LeaveBattle(battle->GetID());
	}
	m_serv.JoinBattle(battleid, scriptPW);
	UiEvents::GetStatusEventSender(UiEvents::addStatusMessage).SendEvent(UiEvents::StatusData(_("Automatically moved to new battle"), 1));
}

void ServerEvents::RegistrationAccepted(const std::string& user, const std::string& pass)
{
	ui().OnRegistrationAccepted(TowxString(user), TowxString(pass));
}

void ServerEvents::RegistrationDenied(const std::string& reason)
{
	ui().OnRegistrationDenied(TowxString(reason));
}

void ServerEvents::OnLoginDenied(const std::string& reason)
{
	ui().OnLoginDenied(reason);
}

void ServerEvents::OnInvalidFingerprintReceived(const std::string& fingerprint, const std::string& expected_fingerprint)
{
	ui().OnInvalidFingerprintReceived(fingerprint, expected_fingerprint);
}
