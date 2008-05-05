#ifndef SPRINGLOBBY_HEADERGUARD_MAINWINDOW_H
#define SPRINGLOBBY_HEADERGUARD_MAINWINDOW_H

#include <wx/frame.h>

class Ui;
class Channel;
class User;
class wxCommandEvent;
class wxListbookEvent;
class MainChatTab;
class MainJoinBattleTab;
class MainSinglePlayerTab;
#ifndef NO_TORRENT_SYSTEM
class MainTorrentTab;
#endif
class wxBoxSizer;
class wxListbook;
class MainOptionsTab;
class wxBitmap;
class wxImageList;
class ChatPanel;
class settings_frame;
class wxMenuItem;
class wxMenuBar;
class wxMenu;
class ReplayTab;

// FIXME shouldn't copy this here
typedef wxWindow wxNotebookPage;


// Page indexes
#define PAGE_CHAT 0
#define PAGE_JOIN 1
#define PAGE_SINGLE 2
#define PAGE_OPTOS 3

//! @brief wxFrame that contains the main window of the client.
class MainWindow : public wxFrame
{
  public:
    MainWindow( Ui& ui );
    virtual ~MainWindow();

    // MainWindow interface
    void OpenChannelChat( Channel& channel );
    void OpenPrivateChat( User& user );

    void CloseAllChats();

    void ShowConfigure();

    void OnMenuAbout( wxCommandEvent& event );
    void OnMenuJoin( wxCommandEvent& event );
    void OnMenuChat( wxCommandEvent& event );
    void OnMenuConnect( wxCommandEvent& event );
    void OnMenuDisconnect( wxCommandEvent& event );
    void OnMenuQuit( wxCommandEvent& event );
    void OnMenuVersion ( wxCommandEvent& event );
    void OnUnitSyncReload( wxCommandEvent& event );
    void OnReportBug( wxCommandEvent& event );
    void OnShowDocs( wxCommandEvent& event );
    void OnShowSettingsPP( wxCommandEvent& event );
    void forceSettingsFrameClose();
    void OnUnitSyncReloaded();


    void OnTabsChanged( wxListbookEvent& event );
    MainChatTab& GetChatTab();
    MainJoinBattleTab& GetJoinTab();
    MainSinglePlayerTab& GetSPTab();
    #ifndef NO_TORRENT_SYSTEM
    MainTorrentTab& GetTorrentTab();
    #endif
    ChatPanel* GetActiveChatPanel();
    ChatPanel* GetChannelChatPanel( const wxString& channel );
    void MakeImages();

  protected:
    // MainWindow variables
    Ui& m_ui;

    wxMenuItem* m_settings_menu;
    wxMenuBar* m_menubar;
    wxMenu* m_menuTools;

    wxBoxSizer* m_main_sizer;
    wxListbook* m_func_tabs;
    wxNotebookPage* m_chat_page;

    MainChatTab* m_chat_tab;
    MainJoinBattleTab* m_join_tab;
    MainSinglePlayerTab* m_sp_tab;
    MainOptionsTab* m_opts_tab;
    #ifndef NO_TORRENT_SYSTEM
    MainTorrentTab* m_torrent_tab;
    #endif

    wxBitmap* m_chat_icon;
    wxBitmap* m_battle_icon;
    wxBitmap* m_options_icon;
    wxBitmap* m_sp_icon;
    wxBitmap* m_downloads_icon;
    wxBitmap* m_replay_icon;
    wxBitmap* m_select_image;

    wxImageList* m_func_tab_images;

    settings_frame* se_frame;
    bool se_frame_active;

    ReplayTab* m_replay_tab;
    DECLARE_EVENT_TABLE()
};

//ChatPanel& servwin();

// wxWidget IDs
enum
{
	MENU_SETTINGSPP,
    MENU_ABOUT = wxID_ABOUT,
    MENU_QUIT = wxID_EXIT,

    MENU_CONNECT = wxID_HIGHEST,
    MENU_DISCONNECT,
    MENU_JOIN,
    MENU_USYNC,
    MENU_TRAC,
    MENU_DOC,
    MENU_CHAT,
    MAIN_TABS,
    MENU_VERSION

};

#endif // SPRINGLOBBY_HEADERGUARD_MAINWINDOW_H
