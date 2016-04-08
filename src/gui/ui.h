/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_UI_H
#define SPRINGLOBBY_HEADERGUARD_UI_H

class IServer;
class TASServer;
class ConnectWindow;
class Spring;
class MainWindow;
class Channel;
class User;
class IBattle;
class ChatPanel;
class HttpDownloaderThread;
class wxEvtHandler;

#include "downloader/prdownloader.h"
#include <wx/string.h>
#include <wx/timer.h>
#include "utils/mixins.h"

//! @brief UI main class
class Ui : public wxTimer, public SL::NonCopyable
{
public:
	Ui();
	~Ui();

	enum PlaybackEnum {
		ReplayPlayback,
		SavegamePlayback
	};

	ChatPanel* GetActiveChatPanel();
	ChatPanel* GetChannelChatPanel(const wxString& channel);

	void ConsoleHelp();

	void ShowMainWindow();
	void ShowConnectWindow();
	void ReopenServerTab();

	bool IsSpringRunning() const;

	void Quit();

	//TODO: remove it
	void AddServerWindow(const wxString&);

	bool Ask(const wxString& heading, const wxString& question) const;
	bool AskText(const wxString& heading, const wxString& question, wxString& answer, bool multiline = false);
	bool AskPassword(const wxString& heading, const wxString& message, wxString& password);
	void ShowMessage(const wxString& heading, const wxString& message) const;

	MainWindow& mw();
	const MainWindow& mw() const;

	bool IsMainWindowCreated() const;

	void OnInit();

	void OnConnected(IServer& server, const wxString& server_name, const wxString& server_ver, bool supported);
	void OnLoggedIn();
	void OnDisconnected(IServer& server, bool wasonline);

	void OnJoinedChannelSuccessful(Channel& chan, bool doFocus = false);
	void OnChannelMessage(Channel& chan, const std::string& msg);

	void OnChannelList(const wxString& channel, const int& numusers);
	void OnUserOnline(User& user);
	void OnUserOffline(User& user);
	void OnUserStatusChanged(User& user);
	void OnUserSaid(User& chan, User& user, const wxString& message);
	void OnUserSaidEx(User& chan, User& user, const wxString& action);

	void OnUnknownCommand(IServer& server, const wxString& command, const wxString& params);
	void OnMotd(IServer& server, const wxString& message);
	void OnServerBroadcast(IServer& server, const wxString& message);
	void OnServerMessage(IServer& server, const wxString& message);

	void OnBattleOpened(IBattle& battle);
	void OnBattleClosed(IBattle& battle);
	void OnUserJoinedBattle(IBattle& battle, User& user);
	void OnUserLeftBattle(IBattle& battle, User& user, bool isbot);
	void OnBattleInfoUpdated(IBattle& battle, const wxString& Tag);

	void OnJoinedBattle(IBattle& battle);
	void OnHostedBattle(IBattle& battle);
	void OnUserBattleStatus(User& user);
	void OnRequestBattleStatus(IBattle& battle);

	void OnSaidBattle(IBattle& battle, const wxString& nick, const wxString& msg);

	void OnSpringTerminated(wxCommandEvent& data);

	void OnAcceptAgreement(const wxString& agreement);

	void OnMainWindowDestruct();

	void OnRing(const wxString& from);
	void OnQuit(wxCommandEvent& data);
	void OnRegistrationAccepted(const wxString& user, const wxString& pass);
	void OnRegistrationDenied(const wxString& reason);
	void OnLoginDenied(const std::string& reason);

	// return true when engine/game/map is missing & prompts user to dl when needed
	bool NeedsDownload(const IBattle* battle, bool uiprompt = true,DownloadEnum::Category cat = DownloadEnum::CAT_NONE);

	bool IsThisMe(User& other) const;
	bool IsThisMe(const User* other) const;
	bool IsThisMe(const wxString& other) const;

	void ReloadPresetList();

	void OpenFileInEditor(const wxString& filepath);

	//! the welcome box, should be called in all code paths directly after MainWindow might be shown for the first time
	void FirstRunWelcome();
	void CheckForUpdates(bool show);
	void EnableDebug(bool enable);

private:
	void OnLobbyDownloaded(wxCommandEvent& /*data*/);
	void Notify();

	IServer* m_serv;
	MainWindow* m_main_win;
	ConnectWindow* m_con_win;

	wxString m_last_used_backup_server;

	bool m_first_update_trigger;
	bool m_connecting;
	int m_connect_retries;

};

Ui& ui();

#endif // SPRINGLOBBY_HEADERGUARD_UI_H
