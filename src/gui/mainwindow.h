/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_MAINWINDOW_H
#define SPRINGLOBBY_HEADERGUARD_MAINWINDOW_H

#include <wx/frame.h>
#include <wx/intl.h>
#include "gui/windowattributespickle.h"
class Ui;
class Channel;
class User;
class wxCommandEvent;
class wxListbookEvent;
class wxAuiNotebookEvent;
class MainChatTab;
class BattleListTab;
class MainJoinBattleTab;
class MainSinglePlayerTab;
class MainDownloadTab;
class wxBoxSizer;
class SLNotebook;
class wxListbook;
class MainOptionsTab;
class wxBitmap;
class wxImageList;
class ChatPanel;
class settings_frame;
class wxMenuItem;
class wxMenuBar;
class wxMenu;
class ChannelChooserDialog;
class PlaybackTab;
class AutojoinChannelDialog;
class WidgetDownloadDialog;
class wxLogWindow;
class wxLogChain;
class wxCloseEvent;
class Statusbar;
class OptionsDialog;
class ReplayTraits;
class wxFocusEvent;

extern const wxEventType MainwindowMessageEvent;

//! @brief wxFrame that contains the main window of the client.
class MainWindow : public wxFrame, public WindowAttributesPickle
{
public:
	MainWindow();
	virtual ~MainWindow();

	// MainWindow interface
	void OpenChannelChat(Channel& channel, bool doFocus = true);
	void OpenPrivateChat(User& user, bool doFocus = false);

	void ShowConfigure(const unsigned int page = OPT_PAGE_SPRING);
	void ShowTab(const unsigned int idx);

	/** Show the channel list dialog. */
	void ShowChannelChooser();

	//! cannot be const while using static event tables (function pointer sig don't match)
	void OnMenuAbout(wxCommandEvent& event);
	void OnMenuJoin(wxCommandEvent& event);
	void OnMenuChat(wxCommandEvent& event);
	void OnMenuConnect(wxCommandEvent& event);
	void OnMenuDisconnect(wxCommandEvent& event);
	void OnMenuDownload(wxCommandEvent& event);
	void OnMenuServerTab(wxCommandEvent& event);
	void OnMenuSaveOptions(wxCommandEvent& event);
	void OnMenuQuit(wxCommandEvent& event);
	void OnMenuPathInfo(wxCommandEvent& event);
	void OnMenuVersion(wxCommandEvent& event);
	void OnMenuResetLayout(wxCommandEvent& event);
	void OnMenuPreferences(wxCommandEvent& event);
	void OnMenuFirstStart(wxCommandEvent& event);
	void OnUnitSyncReload(wxCommandEvent& event);
	void OnMenuAutojoinChannels(wxCommandEvent& event);
	void OnReportBug(wxCommandEvent& event);
	void OnShowDocs(wxCommandEvent& event);
	void OnShowSettingsPP(wxCommandEvent& event);
	void OnMenuSelectLocale(wxCommandEvent& event);
	void OnShowChannelChooser(wxCommandEvent& event);
	void OnShowWriteableDir(wxCommandEvent& event);
	void forceSettingsFrameClose();
	void OnChannelList(const wxString& channel, const int& numusers, const wxString& topic);
	void OnChannelListStart();
	void OnClose(wxCloseEvent&);
	void OnSetFocus(wxFocusEvent&);
	void OnKillFocus(wxFocusEvent&);
	void OnMessage(wxCommandEvent& event);
	void AddMessageEvent(const wxString& message);
	void OnShowErrorMessage(wxCommandEvent& event);

	void OnTabsChanged(wxAuiNotebookEvent& event);
	MainChatTab& GetChatTab();
	BattleListTab& GetBattleListTab();
	MainJoinBattleTab& GetJoinTab();
	MainSinglePlayerTab& GetSPTab();
	PlaybackTab& GetReplayTab();
	//    SavegameTab& GetSavegameTab();
	MainDownloadTab& GetDownloadTab();

	ChatPanel* GetActiveChatPanel();
	ChatPanel* GetChannelChatPanel(const wxString& channel);

	void SetTabIcons();

	void SetLogWin(wxLogWindow* log);

	void LoadPerspectives(const wxString& perspective_name = wxEmptyString);
	void SavePerspectives(const wxString& perspective_name = wxEmptyString);

private:
	void OnUnitSyncReloaded(wxCommandEvent& /*unused*/);

private:
	wxMenuBar* m_menubar;
	wxMenu* m_menuTools;
	wxMenu* m_menuEdit;
	Statusbar* m_statusbar;

	wxBoxSizer* m_main_sizer;
	SLNotebook* m_func_tabs;

	MainChatTab* m_chat_tab;
	BattleListTab* m_list_tab;
	MainJoinBattleTab* m_join_tab;
	MainSinglePlayerTab* m_sp_tab;
	MainDownloadTab* m_torrent_tab;
	OptionsDialog* m_opts_dialog;

	AutojoinChannelDialog* m_autojoin_dialog;
	friend class settings_frame;
	settings_frame* se_frame;
	bool se_frame_active;
	ChannelChooserDialog* m_channel_chooser;

	PlaybackTab* m_replay_tab;

	wxBitmap GetTabIcon(const unsigned char* data, size_t size) const;
	wxString AddPerspectivePostfix(const wxString& pers_name) const;

	wxLogWindow* m_log_win;

	bool m_has_focus;

	enum {
		MENU_ABOUT = wxID_ABOUT,
		MENU_QUIT = wxID_EXIT,

		MENU_CONNECT = wxID_HIGHEST,
		MENU_SETTINGSPP,
		MENU_DISCONNECT,
		MENU_DOWNLOAD,
		MENU_SERVER_TAB,
		MENU_SAVE_OPTIONS,
		MENU_JOIN,
		MENU_USYNC,
		MENU_TRAC,
		MENU_DOC,
		MENU_CHAT,
		MAIN_TABS,
		MENU_VERSION,
		MENU_START_TORRENT,
		MENU_STOP_TORRENT,
		MENU_AUTOJOIN_CHANNELS,
		MENU_SELECT_LOCALE,
		MENU_CHANNELCHOOSER,
		MENU_SAVE_LAYOUT,
		MENU_LOAD_LAYOUT,
		MENU_RESET_LAYOUT,
		MENU_DEFAULT_LAYOUT,
		MENU_SHOWWRITEABLEDIR,
		MENU_PREFERENCES,
		MENU_GENERAL_HELP,
		MENU_PATHINFO
	};

	wxArrayString m_tab_names;

public:
	// Page indexes
	static const unsigned int PAGE_CHAT = 0;
	static const unsigned int PAGE_MULTIPLAYER = 1;
	static const unsigned int PAGE_BATTLEROOM = 2;
	static const unsigned int PAGE_SINGLEPLAYER = 3;
	static const unsigned int PAGE_REPLAYS = 4;
	//        static const unsigned int PAGE_SAVEGAME = 4;
	static const unsigned int PAGE_DOWNLOADS = 5;

	static const unsigned int OPT_PAGE_SPRING = 0;
	static const unsigned int OPT_PAGE_CHAT = 1;
	static const unsigned int OPT_PAGE_TORRENT = 2;
	static const unsigned int OPT_PAGE_GENERAL = 3;
	static const unsigned int OPT_PAGE_GROUPS = 4;

	const wxArrayString& GetTabNames();

	static const unsigned int mySHOW_ERROR_MESSAGE = wxID_HIGHEST + 1;

private:
	DECLARE_EVENT_TABLE()
};

//ChatPanel& servwin();

// wxWidget IDs


#endif // SPRINGLOBBY_HEADERGUARD_MAINWINDOW_H
