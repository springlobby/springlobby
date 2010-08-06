/* Copyright (C) 2007-2009 The SpringLobby Team. All rights reserved. */
//
// Class: MainWindow
//

#ifdef _MSC_VER
#ifndef NOMINMAX
    #define NOMINMAX
#endif // NOMINMAX
#include <winsock2.h>
#endif // _MSC_VER

#include <wx/frame.h>
#include <wx/intl.h>
#include <wx/textdlg.h>
#include <wx/imaglist.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/sizer.h>
#include <wx/menu.h>
#include <wx/log.h>
#include <wx/dcmemory.h>
#include <wx/choicdlg.h>
#include <wx/wupdlock.h>
#include <wx/aui/auibook.h>
#include <wx/tooltip.h>
#include <wx/aboutdlg.h>

#include <stdexcept>
#include <iostream>

#include "aui/auimanager.h"
#include "aui/slbook.h"
#include "aui/artprovider.h"
#include "gui/statusbar.h"
#include "springlobbyapp.h"
#include "mainwindow.h"
#include "settings.h"
#include "ui.h"
#include "server.h"
#include "utils/debug.h"
#include "utils/platform.h"
#include "battlelisttab.h"
#include "mainchattab.h"
#include "mainjoinbattletab.h"
#include "battlelisttab.h"
#include "mainsingleplayertab.h"
#include "mainoptionstab.h"
#include "iunitsync.h"
#include "uiutils.h"
#include "utils/misc.h"
#include "chatpanel.h"
#include "playback/playbacktraits.h"
#include "playback/playbacktab.h"
#ifndef NO_TORRENT_SYSTEM
	#include "maintorrenttab.h"
	#include "torrentwrapper.h"
#endif
#include "user.h"
#include "mapselectdialog.h"

#include "images/springlobby.xpm"
#include "images/chat_icon.png.h"
#include "images/join_icon.png.h"
#include "images/single_player_icon.png.h"
#include "images/options_icon.png.h"
#include "images/downloads_icon.png.h"
#include "images/replay_icon.png.h"
#include "images/broom_tab_icon.png.h"
#include "images/floppy_icon.png.h"

#include "settings++/frame.h"
#include "utils/customdialogs.h"

#include "updater/updatehelper.h"
#include "channel/autojoinchanneldialog.h"
#include "channel/channelchooserdialog.h"
#include "Helper/imageviewer.h"

#if defined(__WXMSW__)
    #include <wx/msw/winundef.h>
    #include <iostream>
#endif
#include <wx/aboutdlg.h>

BEGIN_EVENT_TABLE(MainWindow, wxFrame)

  EVT_MENU( MENU_JOIN,					MainWindow::OnMenuJoin				)
  EVT_MENU( MENU_CHAT,					MainWindow::OnMenuChat				)
  EVT_MENU( MENU_CONNECT,				MainWindow::OnMenuConnect			)
  EVT_MENU( MENU_DISCONNECT,			MainWindow::OnMenuDisconnect		)
  EVT_MENU( MENU_SAVE_OPTIONS,			MainWindow::OnMenuSaveOptions		)
  EVT_MENU( MENU_QUIT,					MainWindow::OnMenuQuit				)
  EVT_MENU( MENU_USYNC,					MainWindow::OnUnitSyncReload		)
  EVT_MENU( MENU_TRAC,					MainWindow::OnReportBug				)
  EVT_MENU( MENU_DOC,					MainWindow::OnShowDocs				)
  EVT_MENU( MENU_SETTINGSPP,			MainWindow::OnShowSettingsPP		)
  EVT_MENU( MENU_VERSION,				MainWindow::OnMenuVersion			)
  EVT_MENU( MENU_ABOUT,					MainWindow::OnMenuAbout				)
  EVT_MENU( MENU_SAVE_LAYOUT,			MainWindow::OnMenuSaveLayout		)
  EVT_MENU( MENU_LOAD_LAYOUT,			MainWindow::OnMenuLoadLayout		)
  EVT_MENU( MENU_RESET_LAYOUT,			MainWindow::OnMenuResetLayout		)
//  EVT_MENU( MENU_SHOW_TOOLTIPS,		MainWindow::OnShowToolTips			)
  EVT_MENU( MENU_AUTOJOIN_CHANNELS,		MainWindow::OnMenuAutojoinChannels	)
  EVT_MENU( MENU_SELECT_LOCALE,			MainWindow::OnMenuSelectLocale		)
  EVT_MENU( MENU_CHANNELCHOOSER,		MainWindow::OnShowChannelChooser	)
  EVT_MENU( MENU_SCREENSHOTS,			MainWindow::OnShowScreenshots		)
  EVT_MENU( MENU_PREFERENCES,			MainWindow::OnMenuPreferences		)
  EVT_MENU( MENU_RENAME,				MainWindow::OnMenuRename			)
  EVT_SET_FOCUS(                        MainWindow::OnSetFocus              )
  EVT_KILL_FOCUS(                       MainWindow::OnKillFocus             )
  EVT_AUINOTEBOOK_PAGE_CHANGED( MAIN_TABS, MainWindow::OnTabsChanged )
  EVT_CLOSE( MainWindow::OnClose )
END_EVENT_TABLE()

MainWindow::TabNames MainWindow::m_tab_names;

MainWindow::MainWindow( )
	: wxFrame( (wxFrame*)0, -1, _("SpringLobby"), wxPoint(50, 50) ),
	WindowAttributesPickle( _T("MAINWINDOW"), this, wxSize(450, 340) ),
	m_opts_dialog(NULL),
    m_autojoin_dialog(NULL),
    m_channel_chooser(NULL),
	m_log_win(NULL),
	m_has_focus(true)
{
	assert( !wxGetApp().IsSimple() );
	SetIcon( wxIcon(springlobby_xpm) );

	GetAui().manager = new AuiManagerContainer::ManagerType( this );

	wxMenu *menuServer = new wxMenu;
	menuServer->Append(MENU_CONNECT, _("&Connect..."));
	menuServer->Append(MENU_DISCONNECT, _("&Disconnect"));
	menuServer->AppendSeparator();
#ifndef NDEBUG
	//this is a prime spot to add experimental stuff, or stubs used to test things not really meant to be in mainwindow
	menuServer->Append(MENU_SAVE_OPTIONS, _("&Save options"));
	menuServer->AppendSeparator();
#endif
	menuServer->Append(MENU_QUIT, _("&Quit"));

	m_menuEdit = new wxMenu;
	m_menuEdit->Append(MENU_AUTOJOIN_CHANNELS, _("&Autojoin channels"));
	m_menuEdit->Append(MENU_PREFERENCES, _("&Preferences"));
	m_settings_menu = new wxMenuItem( m_menuEdit, MENU_SETTINGSPP, _("&Spring settings"), wxEmptyString, wxITEM_NORMAL );
	m_menuEdit->Append (m_settings_menu);


    // loading layouts currently borked
	wxMenu* menuView = new wxMenu;
	menuView->Append( MENU_SAVE_LAYOUT, _("&Save layout") );
	menuView->Append( MENU_LOAD_LAYOUT, _("&Load layout") );
	menuView->Append( MENU_RESET_LAYOUT, _("&Reset layout") );
//	menuView->Append( MENU_DEFAULT_LAYOUT, _("&Set &Layout as default") );


	m_menuTools = new wxMenu;
	m_menuTools->Append(MENU_JOIN, _("&Join channel..."));
	m_menuTools->Append(MENU_CHANNELCHOOSER, _("Channel &list"));
	m_menuTools->Append(MENU_CHAT, _("Open private &chat..."));
	m_menuTools->Append(MENU_RENAME, _("Change &username"));
	m_menuTools->Append(MENU_SCREENSHOTS, _("&View screenshots"));
	m_menuTools->AppendSeparator();
	m_menuTools->Append(MENU_USYNC, _("&Reload maps/mods"));


#ifndef NO_TORRENT_SYSTEM
	m_menuTools->AppendSeparator();
#endif
	m_menuTools->Append(MENU_VERSION, _("Check for new Version"));


	wxMenu *menuHelp = new wxMenu;
	menuHelp->Append(MENU_ABOUT, _("&About"));
	menuHelp->Append(MENU_SELECT_LOCALE, _("&Change language"));
	menuHelp->Append(MENU_TRAC, _("&Report a bug..."));
	menuHelp->Append(MENU_DOC, _("&Documentation"));

	m_menubar = new wxMenuBar;
	m_menubar->Append(menuServer, _("&Server"));
	m_menubar->Append(m_menuEdit, _("&Edit"));

	m_menubar->Append(menuView, _("&View")); //layout stuff --> disabled

	m_menubar->Append(m_menuTools, _("&Tools"));
	m_menubar->Append(menuHelp, _("&Help"));
	SetMenuBar(m_menubar);

	m_main_sizer = new wxBoxSizer( wxHORIZONTAL );
	m_func_tabs = new SLNotebook(  this, _T("mainfunctabs"), MAIN_TABS, wxDefaultPosition, wxDefaultSize,
		wxAUI_NB_WINDOWLIST_BUTTON | wxAUI_NB_TAB_SPLIT | wxAUI_NB_TAB_MOVE | wxAUI_NB_TAB_EXTERNAL_MOVE | wxAUI_NB_SCROLL_BUTTONS | wxAUI_NB_LEFT );
	m_func_tabs->SetArtProvider(new SLArtProvider);

	//IMPORTANT: event handling needs to be disabled while constructing, otherwise deadlock occurs
	SetEvtHandlerEnabled( false );

	m_chat_tab = new MainChatTab( m_func_tabs );
	m_list_tab = new BattleListTab( m_func_tabs );
	m_join_tab = new MainJoinBattleTab( m_func_tabs );
	m_sp_tab = new MainSinglePlayerTab( m_func_tabs );
	m_savegame_tab = new SavegameTab( m_func_tabs );
	m_replay_tab = new ReplayTab ( m_func_tabs );
#ifndef NO_TORRENT_SYSTEM
	m_torrent_tab = new MainTorrentTab( m_func_tabs);
#endif

    //use Insert so no Changepage events are triggered
    m_func_tabs->InsertPage( PAGE_CHAT,     m_chat_tab,     m_tab_names[PAGE_CHAT],     true  );
    m_func_tabs->InsertPage( PAGE_LIST,     m_list_tab,     m_tab_names[PAGE_LIST],     false  );
    m_func_tabs->InsertPage( PAGE_JOIN,     m_join_tab,     m_tab_names[PAGE_JOIN],     false );
    m_func_tabs->InsertPage( PAGE_SINGLE,   m_sp_tab,       m_tab_names[PAGE_SINGLE],   false );
    m_func_tabs->InsertPage( PAGE_SAVEGAME, m_savegame_tab, m_tab_names[PAGE_SAVEGAME], false );
    m_func_tabs->InsertPage( PAGE_REPLAY,   m_replay_tab,   m_tab_names[PAGE_REPLAY],   false );
#ifndef NO_TORRENT_SYSTEM
    m_func_tabs->InsertPage( PAGE_TORRENT,  m_torrent_tab,  m_tab_names[PAGE_TORRENT],  false );
#endif

    LoadPerspectives();
	SetTabIcons();
	m_main_sizer->Add( m_func_tabs, 1, wxEXPAND | wxALL, 0 );

	SetSizer( m_main_sizer );

	Layout();

	se_frame_active = false;

	wxToolTip::Enable(sett().GetShowTooltips());

	m_channel_chooser = new ChannelChooserDialog( this, -1, _("Choose channels to join") );

	m_statusbar = new Statusbar( this );
	SetStatusBar( m_statusbar );
    // re-enable eventhandling
    SetEvtHandlerEnabled( true );

	UpdateMainAppHasFocus(m_has_focus);
}

wxBitmap MainWindow::GetTabIcon( const unsigned char* data, size_t size )
{
    if ( sett().GetUseTabIcons() )
        return charArr2wxBitmap( data , size );
    else
        return wxNullBitmap;
}

void MainWindow::SetTabIcons()
{
		unsigned int count = 0;
    m_func_tabs->SetPageBitmap( count++, GetTabIcon( chat_icon_png, sizeof(chat_icon_png)  ) );
    m_func_tabs->SetPageBitmap( count++, GetTabIcon( join_icon_png, sizeof(join_icon_png)  ) );
    m_func_tabs->SetPageBitmap( count++, GetTabIcon( broom_tab_icon_png, sizeof(broom_tab_icon_png) ) );
    m_func_tabs->SetPageBitmap( count++, GetTabIcon( single_player_icon_png , sizeof (single_player_icon_png) ) );
    m_func_tabs->SetPageBitmap( count++, GetTabIcon( floppy_icon_png , sizeof (floppy_icon_png) ) );
    m_func_tabs->SetPageBitmap( count++, GetTabIcon( replay_icon_png , sizeof (replay_icon_png) ) );
#ifndef NO_TORRENT_SYSTEM
    m_func_tabs->SetPageBitmap( count++, GetTabIcon(  downloads_icon_png , sizeof (downloads_icon_png) ) );
#endif
		m_func_tabs->SetPageBitmap( count++, GetTabIcon( options_icon_png , sizeof (options_icon_png) ) );
    Refresh();
}

void MainWindow::forceSettingsFrameClose()
{
	if (se_frame_active && se_frame != 0)
		se_frame->handleExternExit();
}

void MainWindow::SetLogWin( wxLogWindow* log, wxLogChain* logchain  )
{
    m_log_win = log;
    m_log_chain = logchain;
    if ( m_log_win )
        m_log_win->GetFrame()->SetParent( this );
}

MainWindow::~MainWindow()
{
	SetEvtHandlerEnabled( false );
	if ( m_opts_dialog )
	{
		m_opts_dialog->Show(false);
		m_opts_dialog->Destroy();
	}
}

void MainWindow::OnClose( wxCloseEvent& /*unused*/ )
{
	GetGlobalEventSender(GlobalEvents::OnQuit).SendEvent( 0 );
    SetEvtHandlerEnabled(false);
    {
		wxWindowUpdateLocker lock( this );
		SavePerspectives();
		AuiManagerContainer::ManagerType* manager=GetAui().manager;
		if(manager){
			GetAui().manager=NULL;
			manager->UnInit();
			delete manager;
		}
		//interim fix for resize crashes on metacity and kwin
		#ifndef __WXMSW__
			mapSelectDialog().Show( false );
			mapSelectDialog().Reparent( &ui().mw() );
			mapSelectDialog().Destroy( );
		#endif

		ui().Quit();
		forceSettingsFrameClose();
		freeStaticBox();

		if ( m_autojoin_dialog )
		{
			delete m_autojoin_dialog;
			m_autojoin_dialog = 0;
		}

		sett().SaveSettings();
		if ( m_log_win ) {
			m_log_win->GetFrame()->Destroy();
			if ( m_log_chain ) // if logwin was created, it's the current "top" log
				m_log_chain->DetachOldLog();  //so we need to tellwx not to delete it on its own
				//since we absolutely need to destroy the logwin here, set a fallback for the time until app cleanup
			#if(wxUSE_STD_IOSTREAM)
				wxLog::SetActiveTarget( new wxLogStream( &std::cout ) );
			#endif
		}
	}
	Destroy();
}

void MainWindow::OnSetFocus(wxFocusEvent&)
{
	m_has_focus = true;
	UpdateMainAppHasFocus(m_has_focus);
}

void MainWindow::OnKillFocus(wxFocusEvent&)
{
	m_has_focus = false;
	UpdateMainAppHasFocus(m_has_focus);
}

bool MainWindow::HasFocus()
{
	return m_has_focus;
}

void DrawBmpOnBmp( wxBitmap& canvas, wxBitmap& object, int x, int y )
{
  wxMemoryDC dc;
  dc.SelectObject( canvas );
  dc.DrawBitmap( object, x, y, true );
  dc.SelectObject( wxNullBitmap );
}

//void MainWindow::DrawTxtOnBmp( wxBitmap& canvas, wxString text, int x, int y )
//{
//  wxMemoryDC dc;
//  dc.SelectObject( canvas );
//
//  dc.DrawText( text, x, y);
//  dc.SelectObject( wxNullBitmap );
//}


/*
//! @brief Get the ChatPanel dedicated to server output and input
ChatPanel& servwin()
{
  return ui().mw().GetChatTab().ServerChat();
}
*/

//! @brief Returns the curent MainChatTab object
MainChatTab& MainWindow::GetChatTab()
{
  ASSERT_EXCEPTION( m_chat_tab != 0, _T("m_chat_tab = 0") );
  return *m_chat_tab;
}

//! @brief Returns the curent BattleListTab object
BattleListTab& MainWindow::GetBattleListTab()
{
	ASSERT_EXCEPTION( m_list_tab != 0, _T( "m_list_tab = 0" ) );
	return *m_list_tab;
}

MainJoinBattleTab& MainWindow::GetJoinTab()
{
  ASSERT_EXCEPTION( m_join_tab != 0, _T("m_join_tab = 0") );
  return *m_join_tab;
}


MainSinglePlayerTab& MainWindow::GetSPTab()
{
  ASSERT_EXCEPTION( m_sp_tab != 0, _T("m_sp_tab = 0") );
  return *m_sp_tab;
}

MainWindow::ReplayTab& MainWindow::GetReplayTab()
{
    ASSERT_EXCEPTION( m_replay_tab != 0, _T("m_replay_tab = 0") );
    return *m_replay_tab;
}

MainWindow::SavegameTab& MainWindow::GetSavegameTab()
{
    ASSERT_EXCEPTION( m_replay_tab != 0, _T("m_replay_tab = 0") );
    return *m_savegame_tab;
}

#ifndef NO_TORRENT_SYSTEM
MainTorrentTab& MainWindow::GetTorrentTab()
{
  ASSERT_EXCEPTION( m_torrent_tab  != 0, _T("m_torrent_tab = 0") );
  return *m_torrent_tab ;
}
#endif
ChatPanel* MainWindow::GetActiveChatPanel()
{
  unsigned int index = m_func_tabs->GetSelection();
  if ( index == PAGE_CHAT ) return m_chat_tab->GetActiveChatPanel();
  if ( index == PAGE_JOIN ) return m_join_tab->GetActiveChatPanel();
  return 0;
}


ChatPanel* MainWindow::GetChannelChatPanel( const wxString& channel )
{
  return m_chat_tab->GetChannelChatPanel( channel );
}

//! @brief Open a new chat tab with a channel chat
//!
//! @param channel The channel name
//! @note This does NOT join the chatt.
//! @sa Server::JoinChannel OpenPrivateChat
void MainWindow::OpenChannelChat( Channel& channel, bool doFocus )
{
    ASSERT_LOGIC( m_chat_tab != 0, _T("m_chat_tab") );
    if ( doFocus )
        m_func_tabs->SetSelection( PAGE_CHAT );
    m_chat_tab->AddChatPanel( channel );
}


//! @brief Open a new chat tab with a private chat
//!
//! @param nick The user to whom the chatwindow should be opened to
void MainWindow::OpenPrivateChat( const User& user, bool doFocus )
{
  ASSERT_LOGIC( m_chat_tab != 0, _T("m_chat_tab") );
  m_func_tabs->SetSelection( PAGE_CHAT );
  ChatPanel* cp = m_chat_tab->AddChatPanel( user );
  if ( doFocus )
    cp->FocusInputBox();

}

//! @brief Displays the lobby singleplayer tab.
void MainWindow::ShowSingleplayer()
{
    ShowTab( PAGE_SINGLE );
}

void MainWindow::ShowTab( const unsigned int idx )
{
    if ( idx < m_tab_names.GetCount() )
        m_func_tabs->SetSelection( idx );
    else
        wxLogError( _T("tab selection oob: %d"), idx );
}

//! @brief Displays the lobby configuration.
void MainWindow::ShowConfigure( const unsigned int page )
{
	OptionsDialog* opts = new OptionsDialog( this );
	//possibly out of bounds is captured by m_opts_tab itslef
	opts->SetSelection( page );
	opts->Show();
}

void MainWindow::ShowChannelChooser()
{
    if ( m_channel_chooser && m_channel_chooser->IsShown() )
        return;

    if ( !ui().IsConnected() )
        customMessageBox( SL_MAIN_ICON, _("You need to be connected to server to view channel list"), _("Not connected") );
    else {
        m_channel_chooser->ClearChannels();
		serverSelector().GetServer().RequestChannels();
        m_channel_chooser->Show( true );
    }
}

//! @brief Called when join channel menuitem is clicked
void MainWindow::OnMenuJoin( wxCommandEvent& /*unused*/ )
{

  if ( !ui().IsConnected() ) return;
  wxString answer;
  if ( ui().AskText( _("Join channel..."), _("Name of channel to join"), answer ) ) {
    ui().JoinChannel( answer, _T("") );
  }

}


void MainWindow::OnMenuChat( wxCommandEvent& /*unused*/ )
{

  if ( !ui().IsConnected() ) return;
  wxString answer;
  if ( ui().AskText( _("Open Private Chat..."), _("Name of user"), answer ) ) {
	if (serverSelector().GetServer().UserExists( answer ) ) {
        //true puts focus on new tab
	  OpenPrivateChat( serverSelector().GetServer().GetUser( answer ), true  );
    }
  }

}

void MainWindow::OnMenuAbout( wxCommandEvent& /*unused*/ )
{
    wxAboutDialogInfo info;
	info.SetName(_T("SpringLobby"));
	info.SetVersion (GetSpringLobbyVersion());
	info.SetDescription(_("SpringLobby is a cross-plattform lobby client for the RTS Spring engine"));
	//info.SetCopyright(_T("");
	info.SetLicence(_T("GPL"));
	info.AddDeveloper(_T("BrainDamage"));
	info.AddDeveloper(_T("dizekat"));
	info.AddDeveloper(_T("insaneinside"));
	info.AddDeveloper(_T("Kaot"));
	info.AddDeveloper(_T("koshi"));
	info.AddDeveloper(_T("semi_"));
	info.AddDeveloper(_T("tc-"));
    info.AddTranslator(_T("chaosch (simplified chinese)"));
	info.AddTranslator(_T("lejocelyn (french)"));
	info.AddTranslator(_T("Suprano (german)"));
    info.AddTranslator(_T("tc- (swedish)"));
	info.SetIcon(wxIcon(springlobby_xpm));
	wxAboutBox(info);
}

void MainWindow::OnMenuConnect( wxCommandEvent& /*unused*/ )
{
  ui().ShowConnectWindow();
}


void MainWindow::OnMenuDisconnect( wxCommandEvent& /*unused*/ )
{
  ui().Disconnect();
}

void MainWindow::OnMenuSaveOptions( wxCommandEvent& /*unused*/ )
{
	wxString text;
	if ( ui().AskText(wxEmptyString,wxEmptyString,text) ){
		wxMessageBox( Paste2Pastebin( text ) );
	}
  sett().SaveSettings();
}

void MainWindow::OnMenuQuit( wxCommandEvent& /*unused*/ )
{
  Close();
}

#include "utils/uievents.h"

void MainWindow::OnMenuVersion( wxCommandEvent& /*unused*/ )
{
	ui().CheckForUpdates();
}

void MainWindow::OnUnitSyncReload( wxCommandEvent& /*unused*/ )
{
	usync().AddReloadEvent();
}

void MainWindow::OnShowScreenshots( wxCommandEvent& /*unused*/ )
{
    wxArrayString ar = usync().GetScreenshotFilenames();
    if ( ar.Count() == 0 ) {
        customMessageBoxNoModal( SL_MAIN_ICON, _("There were no screenshots found in your spring data directory."), _("No files found") );
        return;
    }
    ar.Sort();
    ImageViewerDialog* img  = new ImageViewerDialog( ar, true, this, -1, _T("Screenshots") );
    img->Show( true );
}


void MainWindow::OnReportBug( wxCommandEvent& /*unused*/ )
{
	OpenWebBrowser( _T("http://projects.springlobby.info/projects/springlobby/issues/new") );
}


void MainWindow::OnShowDocs( wxCommandEvent& /*unused*/ )
{
    OpenWebBrowser( _T("http://springlobby.info") );
}

void MainWindow::OnTabsChanged( wxAuiNotebookEvent& event )
{
  int newsel = event.GetSelection();

  if ( newsel == 0 || newsel == 1 )
  {
    if ( !ui().IsConnected() && ui().IsMainWindowCreated() ) ui().Connect();
  }
}

void MainWindow::OnShowSettingsPP( wxCommandEvent&  )
{
	se_frame = new settings_frame(this,wxID_ANY,wxT("SpringSettings"),wxDefaultPosition,
	  	    		wxDefaultSize);
	se_frame_active = true;
	se_frame->Show();
}

void MainWindow::OnMenuAutojoinChannels( wxCommandEvent& /*unused*/ )
{
    m_autojoin_dialog = new AutojoinChannelDialog (this);
    m_autojoin_dialog->Show();
}

void MainWindow::OnMenuSelectLocale( wxCommandEvent& /*unused*/ )
{
    if ( wxGetApp().SelectLanguage() ) {
        customMessageBoxNoModal( SL_MAIN_ICON, _("You need to restart SpringLobby for the language change to take effect."),
                                    _("Restart required"), wxICON_EXCLAMATION | wxOK );
    }
}

void MainWindow::OnShowChannelChooser( wxCommandEvent& /*unused*/ )
{
    ShowChannelChooser();
}

void MainWindow::OnChannelList( const wxString& channel, const int& numusers, const wxString& topic )
{
    m_channel_chooser->AddChannel( channel, numusers, topic );
}

void MainWindow::OnChannelListStart( )
{
    m_channel_chooser->ClearChannels();
}

wxString MainWindow::AddPerspectivePostfix( const wxString& pers_name )
{
    wxString perspective_name  = pers_name.IsEmpty() ? sett().GetLastPerspectiveName() : pers_name;
    if ( m_join_tab &&  m_join_tab->UseBattlePerspective() )
        perspective_name += BattlePostfix;
    return perspective_name;
}

void MainWindow::OnMenuSaveLayout( wxCommandEvent& /*unused*/ )
{
	wxString answer;
	if ( !ui().AskText( _("Layout manager"),_("Enter a profile name"), answer ) )
        return;
    while ( answer == _T("SpringLobby-default") ) {
        customMessageBox( SL_MAIN_ICON, _("This profile is write protected, please choose another name"), _("Error") );

        if ( !ui().AskText( _("Layout manager"),_("Enter a profile name"), answer ) )
           return;
    }
    SavePerspectives( answer );
}

void MainWindow::OnMenuLoadLayout( wxCommandEvent& /*unused*/ )
{
	wxArrayString layouts = sett().GetPerspectives();
	unsigned int result = wxGetSingleChoiceIndex( _("Which profile fo you want to load?"), _("Layout manager"), layouts );
	if ( result > layouts.GetCount() )
        return;

    LoadPerspectives( layouts[result] );
}

void MainWindow::OnMenuResetLayout( wxCommandEvent& /*event*/ )
{
	sett().SetDoResetPerspectives( true );
	sett().SaveSettings();
	customMessageBoxNoModal( SL_MAIN_ICON, _("Please restart SpringLobby now"), wxEmptyString );
}

const MainWindow::TabNames& MainWindow::GetTabNames()
{
    return m_tab_names;
}

#ifdef __WXMSW__
	#include "battleroomtab.h"
#endif

void MainWindow::LoadPerspectives( const wxString& pers_name )
{
    sett().SetLastPerspectiveName( pers_name );
    wxString perspective_name = AddPerspectivePostfix( pers_name );

    LoadNotebookPerspective( m_func_tabs, perspective_name );
    m_sp_tab->LoadPerspective( perspective_name );
    m_join_tab->LoadPerspective( perspective_name );
    wxWindow* active_chat_tab = static_cast<wxWindow*> ( m_chat_tab->GetActiveChatPanel() );
    if ( active_chat_tab )
        active_chat_tab->Refresh();
    //chat tab saving won't work w/o further work
//    m_chat_tab->LoadPerspective( perspective_name );
}

void MainWindow::SavePerspectives( const wxString& pers_name )
{
    sett().SetLastPerspectiveName( pers_name );
    wxString perspective_name = AddPerspectivePostfix( pers_name );

    m_sp_tab->SavePerspective( perspective_name );
    m_join_tab->SavePerspective( perspective_name );
//    m_chat_tab->SavePerspective( perspective_name );
    SaveNotebookPerspective( m_func_tabs, perspective_name );
}

void MainWindow::FocusBattleRoomTab()
{
	m_func_tabs->SetSelection( PAGE_JOIN );
	GetJoinTab().FocusBattleRoomTab();
}

void MainWindow::OnMenuPreferences( wxCommandEvent& /*event*/ )
{
	m_opts_dialog = new OptionsDialog( this );
	m_opts_dialog->Show();
}

void MainWindow::OnMenuRename( wxCommandEvent& /*event*/ )
{
	wxString new_username;
	if ( ui().AskText( _("Rename"), _("Enter new nickname"), new_username ) )
		serverSelector().GetServer().ExecuteSayCommand( _T("/rename ") + new_username );
}
