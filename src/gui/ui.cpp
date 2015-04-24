/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

//
// Class: Ui
//

#include <wx/textdlg.h>
#include <wx/intl.h>
#include <wx/utils.h>
#include <wx/debugrpt.h>
#include <wx/filename.h>
#include <wx/app.h>
#include <wx/log.h>
#include <wx/stdpaths.h>

#include <stdexcept>

#include <lslutils/globalsmanager.h>
#include <lslunitsync/springbundle.h>
#include <lslunitsync/unitsync.h>

#include "ui.h"
#include "offlineserver.h"
#include "tasserver.h"
#include "utils/slconfig.h"
#include "iserver.h"
#include "serverselector.h"
#include "spring.h"
#include "channel.h"
#include "gui/connectwindow.h"
#include "gui/mainwindow.h"
#include "user.h"
#include "utils/conversion.h"
#include "utils/uievents.h"
#include "utils/slpaths.h"
#include "utils/version.h"
#include "gui/uiutils.h"
#include "gui/chatpanel.h"
#include "gui/battlelist/battlelisttab.h"
#include "gui/hosting/battleroomtab.h"
#include "gui/hosting/mainjoinbattletab.h"
#include "gui/hosting/mainsingleplayertab.h"
#include "gui/mainchattab.h"
#include "crashreport.h"
#include "gui/maindownloadtab.h"
#include "downloader/prdownloader.h"
#include "gui/agreementdialog.h"
#include "updatehelper.h"
#include "gui/customdialogs.h"
#include "httpfile.h"
#include "gui/textentrydialog.h"
#include "log.h"
#include "settings.h"

#ifndef DISABLE_SOUND
#include "sound/alsound.h"
#endif


SLCONFIG("/General/AutoUpdate", true, "Determines if springlobby should check for updates on startup");
SLCONFIG("/General/LastUpdateCheck", 0L, "Last time springlobby checked for an update");
SLCONFIG("/GUI/StartTab", (long)MainWindow::PAGE_SINGLE, "which tab to show on startup");
SLCONFIG("/Chat/BroadcastEverywhere", true, "setting to spam the server messages in all channels");
SLCONFIG("/Server/Autoconnect", false, "Connect to server on startup");
SLCONFIG("/General/UpdateUrl", "http://version.springlobby.info/current.txt", "Url to check for updates");


static unsigned int s_reconnect_delay_ms = 6 * 1000;	  //initial reconnect delay
static const unsigned int s_max_reconnect_delay = 240 * 1000; //max delay for reconnecting

Ui& ui()
{
	assert(wxThread::IsMain());
	static LSL::Util::LineInfo<Ui> m(AT);
	static LSL::Util::GlobalObjectHolder<Ui, LSL::Util::LineInfo<Ui> > m_ui(m);
	return m_ui;
}

Ui::Ui()
    : m_serv(0)
    , m_main_win(0)
    , m_con_win(0)
    , m_first_update_trigger(true)
    , m_connecting(false)
    , m_connect_retries(0)
    , m_battle_info_updatedSink(this, &BattleEvents::GetBattleEventSender((BattleEvents::BattleInfoUpdate)))
{
	m_main_win = new MainWindow();
	CustomMessageBoxBase::setLobbypointer(m_main_win);
	m_serv = new TASServer();
//	m_serv = new OfflineServer();
	serverSelector().SetCurrentServer(m_serv);
	ConnectGlobalEvent(this, GlobalEvent::OnSpringTerminated, wxObjectEventFunction(&Ui::OnSpringTerminated));
	ConnectGlobalEvent(this, GlobalEvent::OnQuit, wxObjectEventFunction(&Ui::OnQuit));
	ConnectGlobalEvent(this, GlobalEvent::OnLobbyDownloaded, wxObjectEventFunction(&Ui::OnDownloadComplete));
}

Ui::~Ui()
{
	delete m_serv;
	m_serv = NULL;
}

ChatPanel* Ui::GetActiveChatPanel()
{
	return mw().GetActiveChatPanel();
}


MainWindow& Ui::mw()
{
	assert(wxThread::IsMain());
	ASSERT_LOGIC(m_main_win != 0, "m_main_win = 0");
	return *m_main_win;
}

const MainWindow& Ui::mw() const
{
	ASSERT_LOGIC(m_main_win != 0, "m_main_win = 0");
	return *m_main_win;
}


bool Ui::IsMainWindowCreated() const
{
	if (m_main_win == 0)
		return false;
	else
		return true;
}


//! @brief Shows the main window on screen
void Ui::ShowMainWindow()
{
	ASSERT_LOGIC(m_main_win != 0, "m_main_win = 0");
	mw().Show(true);
}


//! @brief Show the connect window on screen
//!
//! @note It will create the ConnectWindow if not allready created
void Ui::ShowConnectWindow()
{
	if (IsConnected())
		return;
	if (m_con_win == 0) {
		ASSERT_LOGIC(m_main_win != 0, "m_main_win = 0");
		m_con_win = new ConnectWindow(m_main_win, *this);
	}
	m_con_win->CenterOnParent();
	m_con_win->Show(true);
	m_con_win->Raise();
}


//! @brief Connects to default server or opens the ConnectWindow
//!
//! @todo Fix Auto Connect
//! @see DoConnect
void Ui::Connect()
{
	if (m_connecting)
		return;
	const wxString server_name = sett().GetDefaultServer();
	const wxString nick = sett().GetServerAccountNick(server_name);
	const wxString pass = sett().GetServerAccountPass(server_name);
	bool autoconnect = cfg().ReadBool(_T( "/Server/Autoconnect" ));
	if (!autoconnect || server_name.IsEmpty() || nick.IsEmpty() || pass.IsEmpty()) {
		ShowConnectWindow();
		return;
	}
	m_con_win = 0;
	m_connecting = true;
	DoConnect(server_name, nick, pass);
}


void Ui::Reconnect()
{
	if (s_reconnect_delay_ms <= s_max_reconnect_delay) {
		s_reconnect_delay_ms += std::max<unsigned int>(s_max_reconnect_delay * 0.50, 1000);
	}
	const wxString servname = sett().GetDefaultServer();
	const wxString pass = sett().GetServerAccountPass(servname);
	if (!sett().GetServerAccountSavePass(servname)) {
		ShowConnectWindow();
		return;
	}
	Disconnect();
	DoConnect(servname, sett().GetServerAccountNick(servname), pass);
}


void Ui::Disconnect()
{
	if (m_serv != 0) {
		if (IsConnected()) {
			m_serv->Disconnect();
		}
	}
}


//! @brief Opens the accutial connection to a server.
void Ui::DoConnect(const wxString& servername, const wxString& username, const wxString& password)
{
	if ((m_serv != NULL) && (m_serv->GetServerName() == STD_STRING(servername)) && IsConnected() &&
	    (m_serv->GetUserName() == STD_STRING(username)) && (m_serv->GetPassword() == STD_STRING(password))) {
		//nothing changed & already connected, do nothing
		return;
	}

	Disconnect();

	m_serv->SetUsername(STD_STRING(username));
	m_serv->SetPassword(STD_STRING(password));

	const wxString host = sett().GetServerHost(servername);
	const int port = sett().GetServerPort(servername);

	AddServerWindow(servername);
	m_serv->uidata.panel->StatusMessage(_T("Connecting to server ") + servername + _T("..."));

	// Connect
	m_serv->Connect(STD_STRING(servername), STD_STRING(host), port);
}

void Ui::AddServerWindow(const wxString& servername)
{
	if (!m_serv->uidata.panel) {
		m_serv->uidata.panel = m_main_win->GetChatTab().AddChatPanel(*m_serv, servername);
	}
}


void Ui::ReopenServerTab()
{
	if (m_serv->IsOnline()) {
		AddServerWindow(TowxString(m_serv->GetServerName()));
		// re-add all users to the user list
		const UserList& list = m_serv->GetUserList();
		for (unsigned int i = 0; i < list.GetNumUsers(); i++) {
			m_serv->uidata.panel->OnChannelJoin(list.GetUser(i));
		}
	}
}

void Ui::DoRegister(const wxString& servername, const wxString& username, const wxString& password)
{
	if (!sett().ServerExists(servername)) {
		OnRegistrationDenied(_T("Server does not exist in settings"));
		return;
	}
	m_serv->Register(STD_STRING(servername), STD_STRING(sett().GetServerHost(servername)), sett().GetServerPort(servername), STD_STRING(username), STD_STRING(password));
}

bool Ui::IsConnected() const
{
	if (m_serv == 0)
		return false;
	return m_serv->IsConnected();
}

void Ui::JoinChannel(const wxString& name, const wxString& password)
{
	if (m_serv != 0)
		m_serv->JoinChannel(STD_STRING(name), STD_STRING(password));
}


bool Ui::IsSpringRunning() const
{
	return spring().IsRunning();
}


//! @brief Quits the entire application
void Ui::Quit()
{
	Disconnect();
	if (m_con_win != 0) {
		m_con_win->Close();
		delete m_con_win;
		m_con_win = NULL;
	}
}

void Ui::Download(const std::string& category, const std::string& name, const std::string& /*hash */)
{
	int count = prDownloader().GetDownload(category, name);
	assert(count > 0);
}

//! @brief Display a dialog asking a question with OK and Canel buttons
//!
//! @return true if OK button was pressed
//! @note this does not return until the user pressed any of the buttons or closed the dialog.
bool Ui::Ask(const wxString& heading, const wxString& question) const
{
	int answer = customMessageBox(SL_MAIN_ICON, question, heading, wxYES_NO);
	return (answer == wxYES);
}

//! cannot be const because parent window cannot be const
bool Ui::AskPassword(const wxString& heading, const wxString& message, wxString& password)
{
	wxPasswordEntryDialog pw_dlg(&mw(), message, heading, password);
	int res = pw_dlg.ShowModal();
	password = pw_dlg.GetValue();
	return (res == wxID_OK);
}

bool Ui::AskText(const wxString& heading, const wxString& question, wxString& answer, bool multiline)
{
	TextEntryDialog name_dlg(&mw(), question, heading, answer, multiline);
	int res = name_dlg.ShowModal();
	answer = name_dlg.GetValue();
	return (res == wxID_OK);
}


void Ui::ShowMessage(const wxString& heading, const wxString& message) const
{
	if (m_main_win == 0)
		return;
	serverMessageBox(SL_MAIN_ICON, message, heading, wxOK);
}


bool Ui::ExecuteSayCommand(const wxString& cmd)
{
	if (!IsConnected())
		return false;

	if ((cmd.BeforeFirst(' ').Lower() == _T("/join")) || (cmd.BeforeFirst(' ').Lower() == _T("/j"))) {
		wxString channel = cmd.AfterFirst(' ');
		const wxString pass = channel.AfterFirst(' ');
		if (!pass.IsEmpty())
			channel = channel.BeforeFirst(' ');
		if (channel.StartsWith(_T("#")))
			channel.Remove(0, 1);
		m_serv->JoinChannel(STD_STRING(channel), STD_STRING(pass));
		return true;
	} else if (cmd.BeforeFirst(' ').Lower() == _T("/away")) {
		m_serv->GetMe().Status().away = true;
		m_serv->GetMe().SendMyUserStatus();
		mw().GetJoinTab().GetBattleRoomTab().UpdateMyInfo();
		return true;
	} else if (cmd.BeforeFirst(' ').Lower() == _T("/back")) {
		if (IsConnected()) {
			m_serv->GetMe().Status().away = false;
			m_serv->GetMe().SendMyUserStatus();
			mw().GetJoinTab().GetBattleRoomTab().UpdateMyInfo();
			return true;
		}
	} else if (cmd.BeforeFirst(' ').Lower() == _T("/ingame")) {
		const wxString nick = cmd.AfterFirst(' ');
		m_serv->RequestInGameTime(STD_STRING(nick));
		return true;
	} else if (cmd.BeforeFirst(' ').Lower() == _T("/help")) {
		const wxString topic = cmd.AfterFirst(' ');
		ConsoleHelp(topic.Lower());
		return true;
	} else if (cmd.BeforeFirst(' ').Lower() == _T("/msg")) {
		const wxString user = cmd.AfterFirst(' ').BeforeFirst(' ');
		const wxString msg = cmd.AfterFirst(' ').AfterFirst(' ');
		m_serv->SayPrivate(STD_STRING(user), STD_STRING(msg));
		return true;
	} else if (cmd.BeforeFirst(' ').Lower() == _T("/channels")) {
		mw().ShowChannelChooser();
		return true;
	}
	return false;
}


void Ui::ConsoleHelp(const wxString& topic)
{
	ChatPanel* panel = GetActiveChatPanel();
	if (panel == 0) {
		wxLogError(_T("GetActiveChatPanel() failed: couldn't find current active panel."));
		return;
	}
	if (topic == wxEmptyString) {
		panel->ClientMessage(_("SpringLobby commands help."));
		panel->ClientMessage(wxEmptyString);
		panel->ClientMessage(_("Global commands:"));
		panel->ClientMessage(_("  \"/away\" - Sets your status to away."));
		panel->ClientMessage(_("  \"/back\" - Resets your away status."));
		panel->ClientMessage(_("  \"/changepassword2 newpassword\" - Changes the current active account's password, needs the old password saved in login box"));
		panel->ClientMessage(_("  \"/changepassword oldpassword newpassword\" - Changes the current active account's password, password cannot contain spaces"));
		panel->ClientMessage(_("  \"/channels\" - Lists currently active channels."));
		panel->ClientMessage(_("  \"/help [topic]\" - Put topic if you want to know more specific information about a command."));
		panel->ClientMessage(_("  \"/join channel [password]\" - Joins a channel."));
		panel->ClientMessage(_("  \"/j\" - Alias to /join."));
		panel->ClientMessage(_("  \"/ingame\" - Shows how much time you have in game."));
		panel->ClientMessage(_("  \"/msg username [text]\" - Sends a private message containing text to username."));
		panel->ClientMessage(_("  \"/part\" - Leaves current channel."));
		panel->ClientMessage(_("  \"/p\" - Alias to /part."));
		panel->ClientMessage(_("  \"/rename newalias\" - Changes your nickname to newalias."));
		panel->ClientMessage(_("  \"/sayver\" - Says what version of SpringLobby you have in chat."));
		panel->ClientMessage(_("  \"/testmd5 text\" - Returns md5-b64 hash of given text."));
		panel->ClientMessage(_("  \"/ver\" - Displays what version of SpringLobby you have."));
		panel->ClientMessage(_("  \"/clear\" - Clears all text from current chat panel"));
		panel->ClientMessage(wxEmptyString);
		panel->ClientMessage(_("Chat commands:"));
		panel->ClientMessage(_("  \"/me action\" - Say IRC style action message."));
		panel->ClientMessage(wxEmptyString);
		panel->ClientMessage(_("If you are missing any commands, go to #springlobby and try to type it there :)"));
		//    panel->ClientMessage( _("  \"/\" - .") );
	} else if (topic == _T("topics")) {
		panel->ClientMessage(_("No topics written yet."));
	} else {
		panel->ClientMessage(_("The topic \"") + topic + _("\" was not found. Type \"/help topics\" only for available topics."));
	}
}


ChatPanel* Ui::GetChannelChatPanel(const wxString& channel)
{
	return mw().GetChannelChatPanel(channel);
}


////////////////////////////////////////////////////////////////////////////////////////////
// EVENTS
////////////////////////////////////////////////////////////////////////////////////////////

//! @brief Called when connected to a server
//!
//! @todo Display in servertab
void Ui::OnConnected(IServer& server, const wxString& server_name, const wxString& version, bool /*supported*/)
{
	slLogDebugFunc("");
	m_connect_retries = 10;

	if (server.uidata.panel)
		server.uidata.panel->StatusMessage(_T("Connected to ") + server_name + _T("."));
	mw().GetBattleListTab().OnConnected();

	delete m_con_win;
	m_con_win = 0;

	if (version.empty()) {
		wxLogWarning("default version supllied from server is empty!");
		return;
	}
	std::map<std::string, LSL::SpringBundle> enginebundles = SlPaths::GetSpringVersionList();
	if (enginebundles.size() == 0) {
		if (Ask(_("Spring can't be found"), wxString::Format(_T("No useable spring engine can be found, download it? (spring %s)"), version.c_str()))) {
			Download(PrDownloader::GetEngineCat(), "spring " + STD_STRING(version), "");
		}
	}
}


bool Ui::IsSpringCompatible(const std::string& engine, const std::string& version)
{
	assert(engine == "spring");
	if (sett().GetDisableSpringVersionCheck())
		return true;
	const std::string ver = SlPaths::GetCompatibleVersion(version);
	if (!ver.empty()) {
		if (SlPaths::GetCurrentUsedSpringIndex() != ver) {
			wxLogMessage(_T("server enforce usage of version: %s, switching to profile: %s"), TowxString(ver).c_str(), TowxString(ver).c_str());
			SlPaths::SetUsedSpringIndex(ver);
			LSL::usync().ReloadUnitSyncLib();
		}
		return true;
	}
	return false; // no compatible version found
}

bool Ui::DownloadArchives(const IBattle& battle)
{
	const std::string engineVersion = battle.GetBattleOptions().engineVersion;
	const std::string engineName = battle.GetBattleOptions().engineName;

	if (!IsSpringCompatible(engineName, engineVersion)) {
		wxLogWarning(_T( "trying to join battles with incompatible spring version" ));

		if (wxYES == customMessageBox(SL_MAIN_ICON,
					      wxString::Format(_("The selected preset requires the engine '%s' version '%s'. Should it be downloaded?"), TowxString(engineName).c_str(), TowxString(engineVersion).c_str()),
					      _("Engine missing"),
					      wxYES_NO | wxICON_QUESTION)) {
			ui().Download(PrDownloader::GetEngineCat(), engineVersion, "");
		}
		return false;
	}

	if (battle.MapExists() && battle.ModExists()) {
		return true;
	}

	wxString prompt;
	if (!battle.ModExists()) {
		prompt += wxString::Format(_("the game '%s'"), TowxString(battle.GetHostModName()).c_str());
	}

	if (!battle.MapExists()) {
		if (!prompt.empty()) {
			prompt += _(" and ");
		}
		prompt += wxString::Format(_("the map '%s'"), TowxString(battle.GetHostMapName()).c_str());
	}
	if (prDownloader().IsRunning()) {
		return true;
	}
	if (customMessageBox(SL_MAIN_ICON, wxString::Format(_("You need to download %s to be able to play.\n\n Shall I download it?"), prompt.c_str()),
			     _("Content needed to be downloaded"), wxYES_NO | wxICON_QUESTION) == wxYES) {
		if (!battle.MapExists()) {
			ui().Download("map", battle.GetHostMapName(), battle.GetHostMapHash());
		}
		if (!battle.ModExists()) {
			ui().Download("game", battle.GetHostModName(), battle.GetHostModHash());
		}
		return true;
	}
	return false;
}


void Ui::OnLoggedIn()
{
	if (m_main_win == 0)
		return;
	mw().GetChatTab().RejoinChannels();
	mw().GetBattleListTab().SortBattleList();
}


void Ui::OnDisconnected(IServer& server, bool wasonline)
{
	Start(s_reconnect_delay_ms, true);
	m_connecting = false;

	if (m_main_win == 0)
		return;
	slLogDebugFunc("");
	if (!&server) {
		wxLogError(_T("WTF got null reference!!!"));
		return;
	}

	mw().GetJoinTab().LeaveCurrentBattle();
	mw().GetBattleListTab().RemoveAllBattles();

	mw().GetChatTab().LeaveChannels();

	const wxString disconnect_msg = wxString::Format(_("disconnected from server: %s"), server.GetServerName().c_str());
	UiEvents::GetStatusEventSender(UiEvents::addStatusMessage).SendEvent(UiEvents::StatusData(disconnect_msg, 1));
	if (!wxTheApp->IsActive()) {
		UiEvents::GetNotificationEventSender().SendEvent(UiEvents::NotficationData(UiEvents::ServerConnection, disconnect_msg));
	}
	if (server.uidata.panel) {
		server.uidata.panel->StatusMessage(disconnect_msg);
		server.uidata.panel->SetServer(0);
	}
	if ((!wasonline) && (m_connect_retries <= 0)) { // couldn't even estabilish a socket, prompt the user to switch to another server
		ShowConnectWindow();
	}
	m_connect_retries--;
}

//! @brief Called when client has joined a channel
//!
//! @todo Check if a pannel allready exists for this channel
void Ui::OnJoinedChannelSuccessful(Channel& chan, bool doFocus)
{
	slLogDebugFunc("");
	if (m_main_win == NULL) {
		return;
	}
	mw().OpenChannelChat(chan, doFocus);
}


void Ui::OnChannelMessage(Channel& chan, const std::string& msg)
{
	ChatPanel* panel = GetChannelChatPanel(TowxString(chan.GetName()));
	if (panel != 0) {
		panel->StatusMessage(TowxString(msg));
	}
}

void Ui::OnChannelList(const wxString& channel, const int& numusers)
{
	ChatPanel* panel = GetActiveChatPanel();
	if (panel == 0) {
		ShowMessage(_("error"), _("no active chat panels open."));
		return;
	}
	panel->StatusMessage(wxString::Format(_("%s (%d users)"), channel.c_str(), numusers));
}


void Ui::OnUserOnline(User& user)
{
	if (m_main_win == 0)
		return;
	mw().GetChatTab().OnUserConnected(user);
}


void Ui::OnUserOffline(User& user)
{
	if (m_main_win == 0)
		return;
	mw().GetChatTab().OnUserDisconnected(user);
	if (user.uidata.panel) {
		user.uidata.panel->SetUser(0);
		user.uidata.panel = 0;
	}
}


void Ui::OnUserStatusChanged(User& user)
{
	if (m_main_win == 0)
		return;
	for (int i = 0; i < m_serv->GetNumChannels(); i++) {
		Channel& chan = m_serv->GetChannel(i);
		if ((chan.UserExists(user.GetNick())) && (chan.uidata.panel != 0)) {
			chan.uidata.panel->UserStatusUpdated(user);
		}
	}
	if (user.uidata.panel) {
		user.uidata.panel->UserStatusUpdated(user);
	}
	if (user.GetStatus().in_game)
		mw().GetBattleListTab().UserUpdate(user);
	try {
		ChatPanel& server = mw().GetChatTab().ServerChat();
		server.UserStatusUpdated(user);
	} catch (...) {
	}
}


void Ui::OnUnknownCommand(IServer& server, const wxString& command, const wxString& params)
{
	if (server.uidata.panel != 0)
		server.uidata.panel->UnknownCommand(command, params);
}


void Ui::OnMotd(IServer& server, const wxString& message)
{
	if (server.uidata.panel != 0)
		server.uidata.panel->Motd(message);
}

void Ui::OnServerBroadcast(IServer& /*server*/, const wxString& message)
{
	if (m_main_win == 0)
		return;
	mw().GetChatTab().BroadcastMessage(message);
	try { // send it to battleroom too
		mw().GetJoinTab().GetBattleRoomTab().GetChatPanel().StatusMessage(message);
	} catch (...) {
	}
}


void Ui::OnServerMessage(IServer& server, const wxString& message)
{
	if (!cfg().ReadBool(_T("/Chat/BroadcastEverywhere"))) {
		if (server.uidata.panel != 0)
			server.uidata.panel->StatusMessage(message);
	} else {
		if (server.uidata.panel != 0)
			server.uidata.panel->StatusMessage(message);
		if (m_main_win == 0)
			return;
		ChatPanel* activepanel = mw().GetChatTab().GetActiveChatPanel();
		if (activepanel != 0) {
			if (activepanel != server.uidata.panel)
				activepanel->StatusMessage(message); // don't send it twice to the server tab
		}
		try { // send it to battleroom too
			mw().GetJoinTab().GetBattleRoomTab().GetChatPanel().StatusMessage(message);
		} catch (...) {
		}
	}
}


void Ui::OnUserSaid(User& user, const wxString& message, bool fromme)
{
	if (m_main_win == 0)
		return;
	if (user.uidata.panel == 0) {
		mw().OpenPrivateChat(user);
	}
	if (fromme)
		user.uidata.panel->Said(TowxString(m_serv->GetMe().GetNick()), message);
	else
		user.uidata.panel->Said(TowxString(user.GetNick()), message);
}

void Ui::OnUserSaidEx(User& user, const wxString& action, bool fromme)
{
	if (m_main_win == 0)
		return;
	if (user.uidata.panel == 0) {
		mw().OpenPrivateChat(user);
	}
	if (fromme)
		user.uidata.panel->DidAction(TowxString(m_serv->GetMe().GetNick()), action);
	else
		user.uidata.panel->DidAction(TowxString(user.GetNick()), action);
}

void Ui::OnBattleOpened(IBattle& battle)
{
	if (m_main_win == 0)
		return;
	mw().GetBattleListTab().AddBattle(battle);
	try {
		User& user = battle.GetFounder();
		for (int i = 0; i < m_serv->GetNumChannels(); i++) {
			Channel& chan = m_serv->GetChannel(i);
			if ((chan.UserExists(user.GetNick())) && (chan.uidata.panel != 0)) {
				chan.uidata.panel->UserStatusUpdated(user);
			}
		}
	} catch (...) {
	}
}


void Ui::OnBattleClosed(IBattle& battle)
{
	if (m_main_win == 0)
		return;
	mw().GetBattleListTab().RemoveBattle(battle);
	try {
		if (mw().GetJoinTab().GetBattleRoomTab().GetBattle() == &battle) {
			if (!battle.IsFounderMe())
				customMessageBoxNoModal(SL_MAIN_ICON, _("The current battle was closed by the host."), _("Battle closed"));
			mw().GetJoinTab().LeaveCurrentBattle();
		}
	} catch (...) {
	}
	for (unsigned int b = 0; b < battle.GetNumUsers(); b++) {
		User& user = battle.GetUser(b);
		user.SetBattle(0);
		for (int i = 0; i < m_serv->GetNumChannels(); i++) {
			Channel& chan = m_serv->GetChannel(i);
			if ((chan.UserExists(user.GetNick())) && (chan.uidata.panel != 0)) {
				chan.uidata.panel->UserStatusUpdated(user);
			}
		}
	}
}


void Ui::OnUserJoinedBattle(IBattle& battle, User& user)
{
	if (m_main_win == 0)
		return;
	mw().GetBattleListTab().UpdateBattle(battle);

	try {
		if (mw().GetJoinTab().GetBattleRoomTab().GetBattle() == &battle) {
			mw().GetJoinTab().GetBattleRoomTab().OnUserJoined(user);
			OnBattleInfoUpdated(std::make_pair(&battle, ""));
		}
	} catch (...) {
	}

	for (int i = 0; i < m_serv->GetNumChannels(); i++) {
		Channel& chan = m_serv->GetChannel(i);
		if ((chan.UserExists(user.GetNick())) && (chan.uidata.panel != 0)) {
			chan.uidata.panel->UserStatusUpdated(user);
		}
	}
}


void Ui::OnUserLeftBattle(IBattle& battle, User& user, bool isbot)
{
	assert(wxThread::IsMain());
	if (m_main_win == 0)
		return;
	user.SetSideiconIndex(-1);    //just making sure he's not running around with some icon still set
	user.BattleStatus().side = 0; // and reset side, so after rejoin we don't potentially stick with a num higher than avail
	mw().GetBattleListTab().UpdateBattle(battle);
	try {
		if (mw().GetJoinTab().GetBattleRoomTab().GetBattle() == &battle) {
			mw().GetJoinTab().GetBattleRoomTab().OnUserLeft(user);
			OnBattleInfoUpdated(std::make_pair(&battle, ""));
			if (&user == &m_serv->GetMe()) {
				mw().GetJoinTab().LeaveCurrentBattle();
				mw().ShowTab(MainWindow::PAGE_LIST);
			}
		}
	} catch (...) {
	}
	if (isbot)
		return;
	for (int i = 0; i < m_serv->GetNumChannels(); i++) {
		Channel& chan = m_serv->GetChannel(i);
		if ((chan.UserExists(user.GetNick())) && (chan.uidata.panel != 0)) {
			chan.uidata.panel->UserStatusUpdated(user);
		}
	}
}

void Ui::OnBattleInfoUpdated(BattleEvents::BattleEventData data)
{
	IBattle& battle = *data.first;
	const wxString Tag = TowxString(data.second);
	if (m_main_win == 0)
		return;
	mw().GetBattleListTab().UpdateBattle(battle);
	if (mw().GetJoinTab().GetCurrentBattle() == &battle) {
		if (Tag.IsEmpty())
			mw().GetJoinTab().UpdateCurrentBattle();
		else
			mw().GetJoinTab().UpdateCurrentBattle(Tag);
	}
}

void Ui::OnJoinedBattle(IBattle& battle)
{
	if (m_main_win == 0)
		return;
	mw().GetJoinTab().JoinBattle(battle);
	mw().ShowTab(MainWindow::PAGE_JOIN);
	if (battle.GetNatType() != NAT_None) {
		wxLogWarning(_T("joining game with NAT transversal"));
	}
}


void Ui::OnHostedBattle(IBattle& battle)
{
	if (m_main_win == 0)
		return;
	mw().GetJoinTab().HostBattle(battle);
	mw().ShowTab(MainWindow::PAGE_JOIN);
}


void Ui::OnUserBattleStatus(IBattle& battle, User& user)
{
	if (m_main_win == 0)
		return;
	mw().GetJoinTab().BattleUserUpdated(user);
	OnBattleInfoUpdated(std::make_pair(&battle, ""));
}


void Ui::OnRequestBattleStatus(IBattle& battle)
{
	if (m_main_win == 0)
		return;
	try {
		if (mw().GetJoinTab().GetBattleRoomTab().GetBattle() == &battle) {
			mw().GetJoinTab().GetBattleRoomTab().GetBattle()->OnRequestBattleStatus();
		}
	} catch (...) {
	}
}


void Ui::OnBattleStarted(IBattle& battle)
{
	if (m_main_win == 0)
		return;
	slLogDebugFunc("");
	mw().GetBattleListTab().UpdateBattle(battle);
}


void Ui::OnSaidBattle(IBattle& /*battle*/, const wxString& nick, const wxString& msg)
{
	if (m_main_win == 0)
		return;
	try {
		mw().GetJoinTab().GetBattleRoomTab().GetChatPanel().Said(nick, msg);
	} catch (...) {
	}
}

void Ui::OnSpringTerminated(wxCommandEvent& data)
{
	const int exit_code = data.GetInt();

	if (!m_serv)
		return;

	try {
		m_serv->GetMe().Status().in_game = false;
		m_serv->GetMe().SendMyUserStatus();
		IBattle* battle = m_serv->GetCurrentBattle();
		if (!battle)
			return;
		if (battle->IsFounderMe() && battle->GetAutoLockOnStart()) {
			battle->SetIsLocked(false);
			battle->SendHostInfo(IBattle::HI_Locked);
		}
	} catch (assert_exception) {
	}

	if (exit_code != 0) {
		SpringDebugReport report;
		if (wxDebugReportPreviewStd().Show(report))
			report.Process();
	}
}


void Ui::OnAcceptAgreement(const wxString& agreement)
{
	AgreementDialog dlg(m_main_win, agreement);
	if (dlg.ShowModal() == 1) {
		m_serv->AcceptAgreement();
		m_serv->Login();
	} else {
		m_connect_retries = 0;
		m_serv->Disconnect();
	}
}


void Ui::OnRing(const wxString& from)
{
	if (m_main_win == 0)
		return;
	m_main_win->RequestUserAttention();

	if (!wxTheApp->IsActive()) {
		const wxString msg = wxString::Format(_("%s:\nring!"), from.c_str());
		UiEvents::GetNotificationEventSender().SendEvent(UiEvents::NotficationData(UiEvents::ServerConnection, msg));
	}

	if (sett().GetChatPMSoundNotificationEnabled()) {
		slsound().ring();
	}
}

bool Ui::IsThisMe(User& other) const
{
	return IsThisMe(TowxString(other.GetNick()));
}

bool Ui::IsThisMe(User* other) const
{
	return ((other != 0) && IsThisMe(TowxString(other->GetNick())));
}

bool Ui::IsThisMe(const wxString& other) const
{
	//if i'm not connected i have no identity
	if (!IsConnected() || m_serv == 0)
		return false;
	else
		return (STD_STRING(other) == m_serv->GetMe().GetNick());
}

int Ui::TestHostPort(unsigned int port) const
{
	return m_serv->TestOpenPort(port);
}

void Ui::ReloadPresetList()
{
	try {
		mw().GetSPTab().ReloadPresetList();
	} catch (...) {
	}
	try {
		mw().GetJoinTab().ReloadPresetList();
	} catch (...) {
	}
}

bool Ui::OnPresetRequiringMap(const wxString& mapname)
{
	if (wxYES == customMessageBox(SL_MAIN_ICON,
				      _("The selected preset requires the map ") + mapname + _(". Should it be downloaded? \
                                    \n Selecting \"no\" will remove the missing map from the preset.\n\
                                    Please reselect the preset after download finished"),
				      _("Map missing"),
				      wxYES_NO)) {
		Download("map", STD_STRING(mapname), "");
		return true;
	}
	return false;
}

void Ui::OpenFileInEditor(const wxString& filepath)
{
	const wxString editor_path = TowxString(SlPaths::GetEditorPath());
	if (editor_path == wxEmptyString) {
		customMessageBoxNoModal(SL_MAIN_ICON, _T("You have not chosen an external text editor to open files with.\nPlease Select one now."), _T("No editor set"));
		mw().ShowConfigure(MainWindow::OPT_PAGE_GENERAL);
		return;
	}
	bool success = (wxExecute(editor_path + _T(" \"") + filepath + _T("\""), wxEXEC_ASYNC) != 0);
	if (!success) {
		customMessageBoxNoModal(SL_MAIN_ICON, _T("There was a problem launching the editor.\nPlease make sure the path is correct and the binary executable for your user.\nNote it's currently not possible to use shell-only editors like ed, vi, etc."), _T("Problem launching editor"));
		mw().ShowConfigure(MainWindow::OPT_PAGE_GENERAL);
	}
}

void Ui::OnInit()
{
	if (sett().IsFirstRun()) {
		FirstRunWelcome();
	} else {
		if (cfg().ReadBool(_T( "/Server/Autoconnect" ))) {
			Connect(); // OnConnect changes tab
		}
		mw().ShowTab(cfg().ReadLong(_T( "/GUI/StartTab" )));
		//don't ask for updates on first run, that's a bit much for a newbie
		if (cfg().ReadBool(_T("/General/AutoUpdate"))) {
			const time_t now = time(0);
			const size_t lastcheck = cfg().ReadLong(_T("/General/LastUpdateCheck"));
			if (now - lastcheck > 3600) {
				CheckForUpdates(false);
				cfg().Write(_T("/General/LastUpdateCheck"), (long)now);
			}
		}
	}
}


void Ui::FirstRunWelcome()
{
	wxLogMessage(_T("first time startup"));

	//this ensures that for new configs there's a default perspective to fall back on
	mw().SavePerspectives(_T("SpringLobby-default"));
	ShowConnectWindow();
}


bool Ui::StartUpdate(const std::string& latestVersion)
{
	const wxString updatedir = TowxString(SlPaths::GetUpdateDir());
	const size_t mindirlen = 9; // safety, minimal is/should be: C:\update
	if ((updatedir.size() <= mindirlen)) {
		wxLogError(_T("Invalid update dir: ") + updatedir);
		return false;
	}
	if (wxDirExists(updatedir)) {
		if (!SlPaths::RmDir(STD_STRING(updatedir))) {
			wxLogWarning(_T("Couldn't cleanup ") + updatedir);
		}
	}
	if (!SafeMkdir(updatedir)) {
		wxLogWarning(_T("couldn't create update directory") + updatedir);
	}

	if (!wxFileName::IsDirWritable(updatedir)) {
		wxLogError(_T("dir not writable: ") + updatedir);
		return false;
	}

	const std::string dlfilepath = SlPaths::GetLobbyWriteDir() + "springlobby-latest.zip";
	if (wxFileExists(TowxString(dlfilepath)) && !(wxRemoveFile(TowxString(dlfilepath)))) {
		wxLogError(_T("couldn't delete: ") + TowxString(dlfilepath));
		return false;
	}
	const std::string dlurl = GetDownloadUrl(latestVersion);
	return prDownloader().Download(dlfilepath, dlurl);
}

void Ui::OnDownloadComplete(wxCommandEvent& data)
{
	if (data.GetInt() != 0) {
		wxLogError(_T("Download springlobby update failed"));
		return;
	}
	const wxString m_newexe = TowxString(SlPaths::GetUpdateDir()) + _T("springlobby_updater.exe");
	wxArrayString params;
#ifdef WIN32
	DWORD id = GetCurrentProcessId();
#else
	long id = wxThread::GetCurrentId();
#endif
	params.push_back(wxString::Format(_T("%ld"), id));
	params.push_back(TowxString(SlPaths::GetExecutable()));
	params.push_back(m_newexe);
	wxString updateDir = TowxString(SlPaths::GetUpdateDir()); //FIXME: for some reason calling the updater with backslash in this dir doesn't work
	params.push_back(updateDir.SubString(0, updateDir.length() - 2));
	params.push_back(TowxString(SlPaths::GetExecutableFolder()));
	const int res = RunProcess(m_newexe, params, true);
	if (res != 0) {
		wxLogError(_T("Tried to call %s %s"), m_newexe.c_str());
		return;
	}
	mw().Close();
}

void Ui::CheckForUpdates(bool show)
{
	wxString latestVersion = GetHttpFile(cfg().ReadString("/General/UpdateUrl"));
	// Need to replace crap chars or versions will always be inequal
	latestVersion.Replace(_T(" "), wxEmptyString, true);
	latestVersion.Replace(_T("\n"), wxEmptyString, true);
	latestVersion.Replace(_T("\t"), wxEmptyString, true);


	if (latestVersion.empty()) {
		customMessageBoxNoModal(SL_MAIN_ICON, _("There was an error checking for the latest version.\nPlease try again later.\nIf the problem persists, please use Help->Report Bug to report this bug."), _("Error"));
		return;
	}
	//get current rev w/o AUX_VERSION added
	const wxString myVersion = TowxString(getSpringlobbyVersion());

	const wxString msg = _("Your Version: ") + myVersion + _T("\n") + _("Latest Version: ") + latestVersion;
	if (!latestVersion.IsSameAs(myVersion, false)) {
#ifdef __WXMSW__
		int answer = customMessageBox(SL_MAIN_ICON,
					      wxString::Format(_("Your %s version is not up to date.\n\n%s\n\nWould you like to update to the new version?"),
							       TowxString(getSpringlobbyName()).c_str(),
							       msg.c_str()),
					      _("Not up to date"), wxOK | wxCANCEL);
		if (answer == wxOK) {
			if (!StartUpdate(STD_STRING(latestVersion))) {
				//this will also happen if updater exe is not present so we don't really ne special check for existance of it
				customMessageBox(SL_MAIN_ICON, _("Automatic update failed\n\nyou will be redirected to a web page with instructions and the download link will be opened in your browser.") + msg, _("Updater error."));
				OpenWebBrowser(_T("https://github.com/springlobby/springlobby/wiki/Install#Windows_Binary"));
				OpenWebBrowser(TowxString(GetDownloadUrl(STD_STRING(latestVersion))));
			}
		}
#else
		customMessageBoxNoModal(SL_MAIN_ICON, _("Your SpringLobby version is not up to date.\n\n") + msg, _("Not up to Date"));
#endif
	} else if (show) {
		customMessageBoxNoModal(SL_MAIN_ICON, _("Your SpringLobby version is up to date.\n\n") + msg, _("Up to Date"));
	}
}


void Ui::OnQuit(wxCommandEvent& /*data*/)
{
	Disconnect();
	delete m_serv;
	m_serv = NULL;
}


void Ui::Notify()
{
	if (m_serv->IsConnected() || (m_con_win != NULL && m_con_win->IsVisible())) {
		Stop();
	} else {
		Reconnect();
	}
}

void Ui::OnRegistrationAccepted(const wxString& user, const wxString& pass)
{
	if (m_con_win == 0) {
		m_con_win = new ConnectWindow(m_main_win, *this);
	}
	m_con_win->OnRegistrationAccepted(user, pass);
}

void Ui::OnRegistrationDenied(const wxString& reason)
{
	if (m_con_win == 0) {
		m_con_win = new ConnectWindow(m_main_win, *this);
	}
	m_con_win->OnRegistrationDenied(reason);
	Disconnect();
}

void Ui::OnLoginDenied(const std::string& reason)
{
	if (m_con_win == 0) {
		m_con_win = new ConnectWindow(m_main_win, *this);
	}
	m_con_win->OnLoginDenied(TowxString(reason));
	Disconnect();
}
