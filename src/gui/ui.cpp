/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

//
// Class: Ui
//
#include "ui.h"

#include <lslunitsync/springbundle.h>
#include <lslunitsync/unitsync.h>
#include <lslutils/globalsmanager.h>
#include <wx/app.h>
#include <wx/debugrpt.h>
#include <wx/filename.h>
#include <wx/intl.h>
#include <wx/log.h>
#include <wx/stdpaths.h>
#include <wx/textdlg.h>
#include <wx/utils.h>
#include <stdexcept>

#include "channel.h"
#include "downloader/prdownloader.h"
#include "exception.h"
#include "gui/agreementdialog.h"
#include "gui/battlelist/battlelisttab.h"
#include "gui/chatpanel.h"
#include "gui/connectwindow.h"
#include "gui/crashreporterdialog.h"
#include "gui/customdialogs.h"
#include "gui/hosting/battleroomtab.h"
#include "gui/hosting/mainjoinbattletab.h"
#include "gui/hosting/mainsingleplayertab.h"
#include "gui/mainchattab.h"
#include "gui/maindownloadtab.h"
#include "gui/mainwindow.h"
#include "gui/textentrydialog.h"
#include "gui/uiutils.h"
#include "ibattle.h"
#include "iserver.h"
#include "log.h"
#include "offlineserver.h"
#include "servermanager.h"
#include "serverselector.h"
#include "settings.h"
#include "spring.h"
#include "tasserver.h"
#include "updatehelper.h"
#include "user.h"
#include "utils/conversion.h"
#include "utils/globalevents.h"
#include "utils/platform.h"
#include "utils/slconfig.h"
#include "utils/slpaths.h"
#include "utils/sortutil.h"
#include "utils/uievents.h"
#include "utils/version.h"
#ifndef DISABLE_SOUND
#include "sound/alsound.h"
#endif


SLCONFIG("/General/AutoUpdate", true, "Determines if springlobby should check for updates on startup");
SLCONFIG("/GUI/StartTab", (long)MainWindow::PAGE_MULTIPLAYER, "which tab to show on startup");
SLCONFIG("/Chat/BroadcastEverywhere", true, "setting to spam the server messages in all channels");
SLCONFIG("/Server/Autoconnect", true, "Connect to server on startup");


static unsigned int s_reconnect_delay_ms = 6 * 1000; //initial reconnect delay

Ui& ui()
{
	assert(wxThread::IsMain());
	static LSL::Util::LineInfo<Ui> m(AT);
	static LSL::Util::GlobalObjectHolder<Ui, LSL::Util::LineInfo<Ui>> m_ui(m);
	return m_ui;
}

Ui::Ui()
    : m_serv(0)
    , m_main_win(0)
    , m_con_win(nullptr)
    , m_first_update_trigger(true)
    , m_connecting(false)
    , m_connect_retries(0)
{
	m_main_win = new MainWindow();
	CustomMessageBoxBase::setLobbypointer(m_main_win);
	m_serv = new TASServer();
	//	m_serv = new OfflineServer();
	serverSelector().SetCurrentServer(m_serv);
	SUBSCRIBE_GLOBAL_EVENT(GlobalEventManager::OnSpringTerminated, Ui::OnSpringTerminated);
	SUBSCRIBE_GLOBAL_EVENT(GlobalEventManager::OnQuit, Ui::OnQuit);
	SUBSCRIBE_GLOBAL_EVENT(GlobalEventManager::OnLobbyDownloaded, Ui::OnLobbyDownloaded);
}

Ui::~Ui()
{
	GlobalEventManager::Instance()->UnSubscribeAll(this);
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

	SlPaths::ValidatePaths();
}


//! @brief Show the connect window on screen
//!
//! @note It will create the ConnectWindow if not allready created
void Ui::ShowConnectWindow()
{
	if (ServerManager::Instance()->IsConnected()) {
		return;
	}
	if (m_con_win == nullptr) {
		ASSERT_LOGIC(m_main_win != 0, "m_main_win = 0");
		m_con_win = new ConnectWindow(m_main_win);
	}
	m_con_win->CenterOnParent();
	m_con_win->Show(true);
	m_con_win->Raise();
}

void Ui::ReopenServerTab()
{
	if (m_serv->IsConnected()) {
		AddServerWindow(TowxString(m_serv->GetServerName()));
		// re-add all users to the user list
		const UserList& list = m_serv->GetUserList();
		for (unsigned int i = 0; i < list.GetNumUsers(); i++) {
			m_serv->panel->OnChannelJoin(list.GetUser(i));
		}
	}
}

void Ui::AddServerWindow(const wxString& servername)
{
	if (m_serv->panel == nullptr) {
		m_serv->panel = ui().mw().GetChatTab().AddChatPanel(*m_serv, servername);
	}
}

bool Ui::IsSpringRunning() const
{
	return spring().IsRunning();
}


//! @brief Quits the entire application
void Ui::Quit()
{
	ServerManager::Instance()->DisconnectFromServer();
	if (m_con_win != nullptr) {
		m_con_win->Destroy();
		m_con_win = nullptr;
	}
}

//! @brief Display a dialog asking a question with OK and Canel buttons
//!
//! @return true if OK button was pressed
//! @note this does not return until the user pressed any of the buttons or closed the dialog.
bool Ui::Ask(const wxString& heading, const wxString& question) const
{
	int answer = customMessageBox(SL_MAIN_ICON, question, heading, wxYES_NO | wxCANCEL);
	return (answer == wxYES);
}

int Ui::AskCrashReporter(const wxString& heading, const wxString& question, const wxString& filePath) const
{
	return CrashReporterDialog::RunCrashReporterDialog(m_main_win, heading, question, filePath);
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

void Ui::ConsoleHelp()
{
	ChatPanel* panel = GetActiveChatPanel();
	if (panel == 0) {
		wxLogError(_T("GetActiveChatPanel() failed: couldn't find current active panel."));
		return;
	}
	panel->ClientMessage(wxString::Format(_("%s commands help:"), GetSpringlobbyName()));
	panel->ClientMessage(wxEmptyString);
	panel->ClientMessage(_("Global commands:"));
	panel->ClientMessage(_("  \"/away\" - Sets your status to away."));
	panel->ClientMessage(_("  \"/back\" - Resets your away status."));
	panel->ClientMessage(_("  \"/changepassword newpassword\" - Changes the current active account's password, needs the old password saved in login box"));
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
	panel->ClientMessage(wxString::Format(
	  _("  \"/sayver\" - Says what version of %s you have in chat."), GetSpringlobbyName()));
	panel->ClientMessage(_("  \"/testmd5 text\" - Returns md5-b64 hash of given text."));
	panel->ClientMessage(wxString::Format(
	  _("  \"/ver\" - Displays what version of %s you have."), GetSpringlobbyName()));
	panel->ClientMessage(_("  \"/clear\" - Clears all text from current chat panel"));
	panel->ClientMessage(wxEmptyString);
	panel->ClientMessage(_("Chat commands:"));
	panel->ClientMessage(_("  \"/me action\" - Say IRC style action message."));
	panel->ClientMessage(wxEmptyString);
	panel->ClientMessage(_("If you are missing any commands, go to #springlobby and try to type it there :)"));
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
void Ui::OnConnected(IServer& server, const wxString& server_name, const wxString& /*version*/, bool /*supported*/)
{
	slLogDebugFunc("");

	if (m_con_win != nullptr) {
		m_con_win->Destroy();
		m_con_win = nullptr;
	}
	m_connect_retries = 10;

	if (server.panel != nullptr)
		server.panel->StatusMessage(_T("Connected to ") + server_name + _T("."));
	mw().GetBattleListTab().OnConnected();

	ReopenServerTab();
}

void Ui::OnLoggedIn()
{
	if (m_main_win == 0)
		return;
	mw().GetChatTab().OnLoggedIn();
	mw().GetBattleListTab().SortBattleList();
}

void Ui::OnDisconnected(IServer& server, bool wasonline)
{
	Start(s_reconnect_delay_ms, true);
	m_connecting = false;

	if (m_main_win == 0)
		return;
	slLogDebugFunc("");

	mw().GetJoinTab().OnDisconnected();
	mw().GetBattleListTab().OnDisconnected();
	mw().GetChatTab().OnDisconnected();

	const wxString disconnect_msg = wxString::Format(_("disconnected from server: %s"), server.GetServerName().c_str());
	UiEvents::GetStatusEventSender(UiEvents::addStatusMessage).SendEvent(UiEvents::StatusData(disconnect_msg, 1));
	if (!wxTheApp->IsActive()) {
		UiEvents::GetNotificationEventSender().SendEvent(UiEvents::NotficationData(UiEvents::ServerConnection, disconnect_msg));
	}
	if (server.panel != nullptr) {
		server.panel->StatusMessage(disconnect_msg);
		server.panel->SetServer(0);
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

void Ui::OnBattleMessage(IBattle& /*battle*/, const std::string& msg)
{
	mw().GetJoinTab().GetBattleRoomTab().GetChatPanel().StatusMessage(TowxString(msg));
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
	if (user.panel != nullptr) {
		user.panel->SetUser(0);
		user.panel = nullptr;
	}
}


void Ui::OnUserStatusChanged(User& user)
{
	if (m_main_win == 0)
		return;
	for (int i = 0; i < m_serv->GetNumChannels(); i++) {
		Channel& chan = m_serv->GetChannel(i);
		if ((chan.UserExists(user.GetNick())) && (chan.panel != nullptr)) {
			chan.panel->UserStatusUpdated(user);
		}
	}
	if (user.panel != nullptr) {
		user.panel->UserStatusUpdated(user);
	}
	if (user.GetStatus().in_game)
		mw().GetBattleListTab().UserUpdate(user);
	try {
		ChatPanel& server = mw().GetChatTab().ServerChat();
		server.UserStatusUpdated(user);
	} catch (const std::exception& e) {
		wxLogWarning(_T("Exception: %s"), e.what());
	}
}


void Ui::OnUnknownCommand(IServer& server, const wxString& command, const wxString& params)
{
	if (server.panel != nullptr)
		server.panel->UnknownCommand(command, params);
}


void Ui::OnMotd(IServer& server, const wxString& message)
{
	if (server.panel != nullptr)
		server.panel->Motd(message);
}

void Ui::OnServerBroadcast(IServer& /*server*/, const wxString& message)
{
	if (m_main_win == 0)
		return;
	mw().GetChatTab().BroadcastMessage(message);
	try { // send it to battleroom too
		mw().GetJoinTab().GetBattleRoomTab().GetChatPanel().StatusMessage(message);
	} catch (const std::exception& e) {
		wxLogWarning(_T("Exception: %s"), e.what());
	}
}


void Ui::OnServerMessage(IServer& server, const wxString& message)
{
	if (!cfg().ReadBool(_T("/Chat/BroadcastEverywhere"))) {
		if (server.panel != nullptr)
			server.panel->StatusMessage(message);
	} else {
		if (server.panel != nullptr)
			server.panel->StatusMessage(message);
		if (m_main_win == nullptr)
			return;
		ChatPanel* activepanel = mw().GetChatTab().GetActiveChatPanel();
		if (activepanel != nullptr) {
			if (activepanel != server.panel)
				activepanel->StatusMessage(message); // don't send it twice to the server tab
		}
		try { // send it to battleroom too
			mw().GetJoinTab().GetBattleRoomTab().GetChatPanel().StatusMessage(message);
		} catch (const std::exception& e) {
			wxLogWarning(_T("Exception: %s"), e.what());
		}
	}
}


void Ui::OnUserSaid(User& chan, User& user, const wxString& message)
{
	if (m_main_win == 0)
		return;
	if (chan.panel == nullptr) {
		mw().OpenPrivateChat(chan);
	}
	chan.panel->Said(TowxString(user.GetNick()), message);
}

void Ui::OnUserSaidEx(User& chan, User& user, const wxString& action)
{
	if (m_main_win == 0)
		return;
	if (chan.panel == 0) {
		mw().OpenPrivateChat(chan);
	}
	chan.panel->DidAction(TowxString(user.GetNick()), action);
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
			if ((chan.UserExists(user.GetNick())) && (chan.panel != nullptr)) {
				chan.panel->UserStatusUpdated(user);
			}
		}
	} catch (const std::exception& e) {
		wxLogWarning(_T("Exception: %s"), e.what());
	}
}


void Ui::OnBattleClosed(IBattle& battle)
{
	if (m_main_win == 0)
		return;
	mw().GetBattleListTab().RemoveBattle(battle);
	try {
		if (mw().GetJoinTab().GetBattleRoomTab().GetBattle() == &battle) {
			if (!battle.IsFounderMe()) {
				const wxString msg = _("The current battle was closed by the host.");
				UiEvents::GetNotificationEventSender().SendEvent(UiEvents::NotficationData(UiEvents::ServerConnection, msg));
			}
			mw().GetJoinTab().LeaveCurrentBattle();
		}
	} catch (const std::exception& e) {
		wxLogWarning(_T("Exception: %s"), e.what());
	}

	for (auto userpair : battle.GetUsers()) {
		assert(userpair.second != nullptr);

		User& user = *userpair.second;
		user.SetBattle(0);
		for (int i = 0; i < m_serv->GetNumChannels(); i++) {
			Channel& chan = m_serv->GetChannel(i);
			if ((chan.UserExists(user.GetNick())) && (chan.panel != nullptr)) {
				chan.panel->UserStatusUpdated(user);
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
			OnBattleInfoUpdated(battle, wxEmptyString);
		}
	} catch (const std::exception& e) {
		wxLogWarning(_T("Exception: %s"), e.what());
	}

	for (int i = 0; i < m_serv->GetNumChannels(); i++) {
		Channel& chan = m_serv->GetChannel(i);
		if ((chan.UserExists(user.GetNick())) && (chan.panel != nullptr)) {
			chan.panel->UserStatusUpdated(user);
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
			OnBattleInfoUpdated(battle, wxEmptyString);
			if (&user == &m_serv->GetMe()) {
				mw().GetJoinTab().LeaveCurrentBattle();
				mw().GetBattleListTab().LeftBattle();
				mw().ShowTab(MainWindow::PAGE_MULTIPLAYER);
			}
		}
	} catch (const std::exception& e) {
		wxLogWarning(_T("Exception: %s"), e.what());
	}
	if (isbot)
		return;
	for (int i = 0; i < m_serv->GetNumChannels(); i++) {
		Channel& chan = m_serv->GetChannel(i);
		if ((chan.UserExists(user.GetNick())) && (chan.panel != nullptr)) {
			chan.panel->UserStatusUpdated(user);
		}
	}
}

void Ui::OnBattleInfoUpdated(IBattle& battle, const wxString& Tag)
{
	if (m_main_win == 0)
		return;
	mw().GetBattleListTab().UpdateBattle(battle);
	if (mw().GetJoinTab().GetCurrentBattle() == &battle) {
		mw().GetJoinTab().UpdateCurrentBattle(Tag);
	}
}

void Ui::OnJoinedBattle(IBattle& battle)
{
	if (m_main_win == 0)
		return;
	mw().GetJoinTab().JoinBattle(battle);
	mw().GetBattleListTab().JoinBattle(battle);
	mw().ShowTab(MainWindow::PAGE_BATTLEROOM);
	if (battle.GetNatType() != NAT_None) {
		wxLogWarning(_T("joining game with NAT transversal"));
	}
}


void Ui::OnHostedBattle(IBattle& battle)
{
	if (m_main_win == 0)
		return;
	mw().GetJoinTab().HostBattle(battle);
	mw().ShowTab(MainWindow::PAGE_BATTLEROOM);
}


void Ui::OnUserBattleStatus(User& user)
{
	if (m_main_win == 0) {
		return;
	}
	if (m_serv == 0 || !m_serv->IsOnline()) {
		return;
	}

	mw().GetJoinTab().BattleUserUpdated(user);
	IBattle* battle = user.GetBattle();
	if (battle == nullptr) {
		wxLogWarning("trying to update non-existing battle");
		return;
	}
	OnBattleInfoUpdated(*battle, wxEmptyString);
}

void Ui::OnBattleTopic(IBattle& /*battle*/, const wxString& who, const wxString& msg)
{
	if (m_main_win == 0)
		return;
	try {
		mw().GetJoinTab().GetBattleRoomTab().GetChatPanel().SetTopic(who, msg);
	} catch (const std::exception& e) {
		wxLogWarning(_T("Exception: %s"), e.what());
	}
}

void Ui::OnSaidBattle(IBattle& /*battle*/, const wxString& nick, const wxString& msg)
{
	if (m_main_win == 0)
		return;
	try {
		mw().GetJoinTab().GetBattleRoomTab().GetChatPanel().Said(nick, msg);
	} catch (const std::exception& e) {
		wxLogWarning(_T("Exception: %s"), e.what());
	}
}

void Ui::OnSpringTerminated(wxCommandEvent&)
{
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
	} catch (const assert_exception& e) {
		wxLogWarning(_T("Exception: %s"), e.what());
	}
}


void Ui::OnAcceptAgreement(const wxString& agreement)
{
	std::string verif_code;
	AgreementDialog dlg(m_main_win, agreement, &verif_code);
	if (dlg.ShowModal() == wxID_OK) {
		m_serv->AcceptAgreement(verif_code);
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

bool Ui::IsThisMe(const User* other) const
{
	return ((other != nullptr) && IsThisMe(TowxString(other->GetNick())));
}

bool Ui::IsThisMe(const wxString& other) const
{
	//if i'm not connected i have no identity
	if (!ServerManager::Instance()->IsConnected() || m_serv == 0)
		return false;
	else
		return (STD_STRING(other) == m_serv->GetMe().GetNick());
}

void Ui::ReloadPresetList()
{
	try {
		mw().GetSPTab().ReloadPresetList();
	} catch (const std::exception& e) {
		wxLogWarning(_T("Exception: %s"), e.what());
	}
	try {
		mw().GetJoinTab().ReloadPresetList();
	} catch (const std::exception& e) {
		wxLogWarning(_T("Exception: %s"), e.what());
	}
}

void Ui::OpenFileInEditor(const wxString& filepath)
{
	const wxString editor_path = TowxString(SlPaths::GetEditorPath());
	if (editor_path == wxEmptyString) {
		customMessageBoxModal(SL_MAIN_ICON, _T("You have not chosen an external text editor to open files with.\nPlease Select one now."), _T("No editor set"));
		mw().ShowConfigure(MainWindow::OPT_PAGE_GENERAL);
		return;
	}
	bool success = (wxExecute(editor_path + _T(" \"") + filepath + _T("\""), wxEXEC_ASYNC) != 0);
	if (!success) {
		customMessageBoxModal(SL_MAIN_ICON, _T("There was a problem launching the editor.\nPlease make sure the path is correct and the binary executable for your user.\nNote it's currently not possible to use shell-only editors like ed, vi, etc."), _T("Problem launching editor"));
		mw().ShowConfigure(MainWindow::OPT_PAGE_GENERAL);
	}
}

void Ui::OnInit()
{
	if (sett().IsFirstRun()) {
		FirstRunWelcome();
	} else {
		if (cfg().ReadBool(_T( "/Server/Autoconnect" ))) {
			ServerManager::Instance()->ConnectToServer(); // OnConnect changes tab
		}
		mw().ShowTab(cfg().ReadLong(_T( "/GUI/StartTab" )));
		//don't ask for updates on first run, that's a bit much for a newbie
		if (cfg().ReadBool(_T("/General/AutoUpdate"))) {
			CheckForUpdates(false);
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

void Ui::OnLobbyDownloaded(wxCommandEvent& data)
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

void Ui::CheckForUpdates(bool is_interactive)
{
	std::string latestVersion = GetLatestVersion(!is_interactive);

	if (latestVersion.empty()) {
		if (is_interactive) {
			customMessageBoxModal(SL_MAIN_ICON, _(
			  "There was an error checking for the latest version.\n"
			  "Please try again later.\n"
			  "If the problem persists, please use Help->Report Bug to report this bug."),
			  _("Error"));
		}
		return;
	}
	//get current rev w/o AUX_VERSION added
	const std::string myVersion = GetSpringlobbyVersion();

	const wxString versionMessage = _("Your Version: ") + myVersion + '\n' + _("Latest Version: ") + latestVersion;
	int isMyVersionOld = CompareVersionStrings(myVersion, latestVersion);

	if (0 == isMyVersionOld) {
		if (is_interactive) {
			customMessageBoxModal(SL_MAIN_ICON, wxString::Format(
			  _("Your %s version is up to date."), GetSpringlobbyName())
			  + _T("\n\n") + versionMessage, _("Up to Date"));
		}
	} else if (isMyVersionOld > 0) {
		wxString message;
		message += wxString::Format(_("Your %s version is not up to date."), GetSpringlobbyName());
		message += wxString::Format(_T("\n\n%s\n\n"), versionMessage);

#ifdef __WXMSW__
		message += _("Would you like to update to the new version?");

		const wxString heading = wxString::Format(_("Update %s?"), GetSpringlobbyName());
		if (!Ask(heading, message))
			return;
		try {
			prDownloader().UpdateApplication(GetDownloadUrl(latestVersion));
		} catch (Exception& ex) {
			//this will also happen if updater exe is not present so we don't really ne special check for existance of it
			wxString errorMsg = _("Automatic update failed.") + '\n';
			errorMsg += ex.Reason() + '\n';
			errorMsg += versionMessage + '\n';
			errorMsg += _("The instructions for updating manually have been opened in your web browser.");
			customMessageBox(SL_MAIN_ICON, errorMsg, _("Updater error."));
			OpenWebBrowser(_T("https://github.com/springlobby/springlobby/wiki/Install#Windows_Binary"));
			OpenWebBrowser(TowxString(GetDownloadUrl(latestVersion)));
		} catch (const std::exception& e) {
			wxLogError(_T("Exception: %s"), e.what());
		}
#else
		message += _("Please update to the latest version before reporting bugs.");
		customMessageBoxModal(SL_MAIN_ICON, message, _("Not up to Date"));
#endif
	} else {
		if (is_interactive) {
			wxString message;
			message += wxString::Format(_("Your %s version is newer than the latest release."), GetSpringlobbyName());
			message += wxString::Format(_T("\n\n%s\n\n"), versionMessage);
			message += _("Thank you for helping with testing!");
			customMessageBoxModal(SL_MAIN_ICON, message, _T("Much development, wow"));
		}
	}
}


void Ui::OnQuit(wxCommandEvent& /*data*/)
{
	ServerManager::Instance()->DisconnectFromServer();
	delete m_serv;
	m_serv = NULL;
}


void Ui::Notify()
{
	if (m_serv->IsConnected() || (m_con_win != nullptr && m_con_win->IsVisible())) {
		Stop();
	} else {
		ServerManager::Instance()->ReconnectToServer();
	}
}

void Ui::OnRegistrationAccepted(const wxString& user, const wxString& pass)
{
	if (m_con_win == nullptr) {
		m_con_win = new ConnectWindow(m_main_win);
	}
	m_con_win->OnRegistrationAccepted(user, pass);
}

void Ui::OnRegistrationDenied(const wxString& reason)
{
	if (m_con_win == nullptr) {
		m_con_win = new ConnectWindow(m_main_win);
	}
	m_con_win->OnRegistrationDenied(reason);
	ServerManager::Instance()->DisconnectFromServer();
}

void Ui::OnLoginDenied(const std::string& reason)
{
	if (m_con_win == nullptr) {
		m_con_win = new ConnectWindow(m_main_win);
	}
	m_con_win->OnLoginDenied(TowxString(reason));
	ServerManager::Instance()->DisconnectFromServer();
}

static bool requested(DownloadEnum::Category req, DownloadEnum::Category cat)
{
	if (req == cat)
		return true;
	if (req == DownloadEnum::CAT_NONE)
		return true;
	return false;
}

bool Ui::NeedsDownload(const IBattle* battle, bool uiprompt, DownloadEnum::Category cat)
{
	if (battle == nullptr) {
		wxLogWarning("Battle is null, nothing required!");
		return true;
	}
	bool needstuff = false;
	std::string stodl;
	std::list<std::pair<DownloadEnum::Category, std::string>> todl;

	if (requested(cat, DownloadEnum::CAT_ENGINE) && !battle->EngineExists() && !battle->GetEngineName().empty() && !battle->GetEngineVersion().empty()) {
		stodl.append("- engine " + battle->GetEngineName() + " " + battle->GetEngineVersion() + "\n");
		todl.push_back(std::make_pair(DownloadEnum::CAT_ENGINE, battle->GetEngineName() + " " + battle->GetEngineVersion()));
	}
	if (requested(cat, DownloadEnum::CAT_MAP) && !battle->MapExists(false) && !battle->GetHostMapName().empty()) {
		stodl.append("- map " + battle->GetHostMapName() + "\n");
		todl.push_back(std::make_pair(DownloadEnum::CAT_MAP, battle->GetHostMapName()));
	}
	if (requested(cat, DownloadEnum::CAT_GAME) && !battle->GameExists(false) && !battle->GetHostGameNameAndVersion().empty()) {
		stodl.append("- game " + battle->GetHostGameNameAndVersion() + "\n");
		todl.push_back(std::make_pair(DownloadEnum::CAT_GAME, battle->GetHostGameNameAndVersion()));
	}


	if (!todl.empty()) {
		needstuff = true;
		if (uiprompt) {
			const wxString prompt = _("Content is required to play. Should it be downloaded?") + _T("\n") + stodl;

			if (!Ask(_("Content is missing"), prompt))
				return true;
		}

		for (auto dl : todl) {
			prDownloader().Download(dl.first, dl.second);
		}
	}

	if (battle->GetEngineVersion().empty()) {
		wxLogWarning("No engine version in battle set, assuming no prequesites");
		return needstuff;
	}

	const std::string compatversion = SlPaths::GetCompatibleVersion(battle->GetEngineVersion());
	if (compatversion == SlPaths::GetCurrentUsedSpringIndex()) {
		return needstuff;
	}
	wxLogWarning("Required engine version doesn't match current selected version, switching to %s", compatversion.c_str());
	SlPaths::SetUsedSpringIndex(compatversion);
	LSL::usync().ReloadUnitSyncLib();

	return true;
}

void Ui::OnInvalidFingerprintReceived(const std::string& fingerprint, const std::string& expected_fingerprint)
{
	int answer = wxCANCEL;
	if (expected_fingerprint.empty()) {
		answer = customMessageBox(SL_MAIN_ICON, _("The certificate by the server is unkown. Do you want to trust it? Please verify the fingerprint:\n") + TowxString(fingerprint)  , _("Unknown Certificate"), wxYES_NO | wxCANCEL);
	} else {
		answer = customMessageBox(SL_MAIN_ICON,
		  wxString::Format(_("The server provided an invalid certificate! Maybe there is a MITM attack ongoing. Do you want ignore this warning?\nExpected:\n%s\nReceived:\n%s"),
		  expected_fingerprint, fingerprint), _("Unknown Certificate"), wxYES_NO | wxCANCEL);
	}
	if (answer != wxYES) {
		return;
	}

	sett().SetServerFingerprint(m_serv->GetServerName(), fingerprint);
	sett().SaveSettings();
}
