/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: MainWindow
//

#include <wx/frame.h>
#include <wx/intl.h>
#include <wx/textdlg.h>
#include <wx/imaglist.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/sizer.h>
#include <wx/listbook.h>
#include <wx/menu.h>
#include <wx/dcmemory.h>

#include <stdexcept>

#include "mainwindow.h"
#include "settings.h"
#include "ui.h"
#include "server.h"
#include "utils.h"
#include "battlelisttab.h"
#include "mainchattab.h"
#include "mainjoinbattletab.h"
#include "mainsingleplayertab.h"
#include "mainoptionstab.h"
#include "iunitsync.h"
#include "uiutils.h"
#include "maintorrenttab.h"

#include "images/springlobby.xpm"
#include "images/chat_icon.png.h"
#include "images/chat_icon_text.png.h"
#include "images/join_icon.png.h"
#include "images/join_icon_text.png.h"
#include "images/single_player_icon.png.h"
#include "images/single_player_icon_text.png.h"
#include "images/options_icon.png.h"
#include "images/options_icon_text.png.h"
#include "images/select_icon.xpm"
#include "images/downloads_icon.png.h"
#include "images/downloads_icon_text.png.h"

#include "settings++/frame.h"
#include "settings++/custom_dialogs.h"

#include "updater/versionchecker.h"

#ifdef HAVE_WX28
#include <wx/aboutdlg.h>
#endif

BEGIN_EVENT_TABLE(MainWindow, wxFrame)

  EVT_MENU( MENU_JOIN, MainWindow::OnMenuJoin )
  EVT_MENU( MENU_CHAT, MainWindow::OnMenuChat )
  EVT_MENU( MENU_CONNECT, MainWindow::OnMenuConnect )
  EVT_MENU( MENU_DISCONNECT, MainWindow::OnMenuDisconnect )
  EVT_MENU( MENU_QUIT, MainWindow::OnMenuQuit )
  EVT_MENU( MENU_USYNC, MainWindow::OnUnitSyncReload )
  EVT_MENU( MENU_TRAC, MainWindow::OnReportBug )
  EVT_MENU( MENU_DOC, MainWindow::OnShowDocs )
  EVT_MENU( MENU_SETTINGSPP, MainWindow::OnShowSettingsPP )
  EVT_MENU( MENU_VERSION, MainWindow::OnMenuVersion )
  EVT_MENU( MENU_ABOUT, MainWindow::OnMenuAbout )


  EVT_LISTBOOK_PAGE_CHANGED( MAIN_TABS, MainWindow::OnTabsChanged )

END_EVENT_TABLE()



MainWindow::MainWindow( Ui& ui ) :
  wxFrame( (wxFrame*)0, -1, _("SpringLobby"), wxPoint(50, 50), wxSize(450, 340) ),
  m_ui(ui)
{
  SetIcon( wxIcon(springlobby_xpm) );
  wxMenu *menuFile = new wxMenu;
  menuFile->Append(MENU_CONNECT, _("&Connect..."));
  menuFile->Append(MENU_DISCONNECT, _("&Disconnect"));
  menuFile->AppendSeparator();
  menuFile->Append(MENU_QUIT, _("&Quit"));

  wxMenu *menuEdit = new wxMenu;

  m_menuTools = new wxMenu;
  m_menuTools->Append(MENU_JOIN, _("&Join channel..."));
  m_menuTools->Append(MENU_CHAT, _("Open private &chat..."));
  m_menuTools->AppendSeparator();
  m_menuTools->Append(MENU_USYNC, _("&Reload maps/mods"));
  m_menuTools->AppendSeparator();
  m_menuTools->Append(MENU_VERSION, _("Check for new Version"));
  m_settings_menu = new wxMenuItem( m_menuTools, MENU_SETTINGSPP, _("SpringSettings"), wxEmptyString, wxITEM_NORMAL );
  m_menuTools->Append (m_settings_menu);

  wxMenu *menuHelp = new wxMenu;
  menuHelp->Append(MENU_ABOUT, _("&About"));
  menuHelp->Append(MENU_TRAC, _("&Report a bug..."));
  menuHelp->Append(MENU_DOC, _("&Documentation"));

  m_menubar = new wxMenuBar;
  m_menubar->Append(menuFile, _("&File"));
  m_menubar->Append(menuEdit, _("&Edit"));
  m_menubar->Append(m_menuTools, _("&Tools"));
  m_menubar->Append(menuHelp, _("&Help"));
  SetMenuBar(m_menubar);

  m_main_sizer = new wxBoxSizer( wxHORIZONTAL );
  m_func_tabs = new wxListbook( this, MAIN_TABS, wxDefaultPosition, wxDefaultSize, wxLB_LEFT );

  m_chat_icon =  charArr2wxBitmapAddText( chat_icon_png , sizeof (chat_icon_png) , chat_icon_text_png, sizeof(chat_icon_text_png), 64 ) ;
  m_battle_icon = charArr2wxBitmapAddText( join_icon_png , sizeof (join_icon_png), join_icon_text_png , sizeof (join_icon_text_png), 64 ) ;
  m_sp_icon = charArr2wxBitmapAddText( single_player_icon_png , sizeof (single_player_icon_png), single_player_icon_text_png , sizeof (single_player_icon_text_png), 64 ) ;
  m_options_icon =   charArr2wxBitmapAddText( options_icon_png , sizeof (options_icon_png), options_icon_text_png , sizeof (options_icon_text_png), 64 ) ;
  m_downloads_icon = charArr2wxBitmapAddText( downloads_icon_png , sizeof (downloads_icon_png), downloads_icon_text_png , sizeof (downloads_icon_text_png), 64 ) ;
  m_select_image = new wxBitmap( select_icon_xpm );

  m_func_tab_images = new wxImageList( 64, 64 );
  MakeImages();

  m_func_tabs->AssignImageList( m_func_tab_images );
  m_chat_tab = new MainChatTab( m_func_tabs, m_ui );
  m_join_tab = new MainJoinBattleTab( m_func_tabs, m_ui );
  m_sp_tab = new MainSinglePlayerTab( m_func_tabs, m_ui );
  m_opts_tab = new MainOptionsTab( m_func_tabs, m_ui );
  m_torrent_tab = new MainTorrentTab( m_func_tabs, m_ui);

  m_func_tabs->AddPage( m_chat_tab, _T(""), true, 0 );
  m_func_tabs->AddPage( m_join_tab, _T(""), false, 1 );
  m_func_tabs->AddPage( m_sp_tab, _T(""), false, 2 );
  m_func_tabs->AddPage( m_opts_tab, _T(""), false, 3 );
  m_func_tabs->AddPage( m_torrent_tab, _T(""), false, 4 );
  //TODO insert real downloads panel
  //m_func_tabs->AddPage( m_opts_tab, _T(""), false, 4 );

  m_main_sizer->Add( m_func_tabs, 1, wxEXPAND | wxALL, 2 );

  SetSizer( m_main_sizer );

  SetSize( sett().GetMainWindowLeft(), sett().GetMainWindowTop(), sett().GetMainWindowWidth(), sett().GetMainWindowHeight() );
  Layout();

  se_frame_active = false;
}

void MainWindow::forceSettingsFrameClose()
{
	if (se_frame_active && se_frame != 0)
		se_frame->handleExternExit();
}

MainWindow::~MainWindow()
{
  int x, y, w, h;
  GetSize( &w, &h );
  sett().SetMainWindowHeight( h );
  sett().SetMainWindowWidth( w );
  GetPosition( &x, &y );
  sett().SetMainWindowTop( y );
  sett().SetMainWindowLeft( x );
  sett().SaveSettings();
  m_ui.Quit();
  m_ui.OnMainWindowDestruct();
  freeStaticBox();

  delete m_chat_icon;
  delete m_battle_icon;
  delete m_options_icon;
  delete m_select_image;

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

void MainWindow::MakeImages()
{
  m_func_tab_images->RemoveAll();

  //if ( m_func_tabs->GetSelection() == 0 ) {
    /*wxBitmap img( *m_select_image );
    DrawBmpOnBmp( img, *m_chat_icon, 0, 0 );
    m_func_tab_images->Add( img );
  } else {*/
 // DrawTxtOnBmp( *m_battle_icon, _("Test"), 1,1);
    m_func_tab_images->Add( *m_chat_icon );
  //}

  //if ( m_func_tabs->GetSelection() == 1 ) {
    /*wxBitmap img( *m_select_image );
    DrawBmpOnBmp( img, *m_battle_icon, 0, 0 );
    m_func_tab_images->Add( img );
  } else {*/
    m_func_tab_images->Add( *m_battle_icon );
  //}

  //if ( m_func_tabs->GetSelection() == 2 ) {
    /*wxBitmap img( *m_select_image );
    DrawBmpOnBmp( img, *m_sp_icon, 0, 0 );
    m_func_tab_images->Add( img );
  } else {*/
    m_func_tab_images->Add( *m_sp_icon );
  //}

  //if ( m_func_tabs->GetSelection() == 3 ) {
    /*wxBitmap img( *m_select_image );
    DrawBmpOnBmp( img, *m_options_icon, 0, 0 );
    m_func_tab_images->Add( img );
  } else {*/
    m_func_tab_images->Add( *m_options_icon );
     m_func_tab_images->Add( *m_options_icon );
   // m_func_tab_images->Add( *m_downloads_icon );
  //}

}


/*
//! @brief Get the ChatPanel dedicated to server output and input
ChatPanel& servwin()
{
  return m_ui.mw().GetChatTab().ServerChat();
}
*/

//! @brief Returns the curent MainChatTab object
MainChatTab& MainWindow::GetChatTab()
{
  ASSERT_LOGIC( m_chat_tab != 0, _T("m_chat_tab = 0") );
  return *m_chat_tab;
}

MainJoinBattleTab& MainWindow::GetJoinTab()
{
  ASSERT_LOGIC( m_join_tab != 0, _T("m_join_tab = 0") );
  return *m_join_tab;
}


MainSinglePlayerTab& MainWindow::GetSPTab()
{
  ASSERT_LOGIC( m_sp_tab != 0, _T("m_sp_tab = 0") );
  return *m_sp_tab;
}

MainTorrentTab& MainWindow::GetTorrentTab()
{
  ASSERT_LOGIC( m_torrent_tab  != 0, _T("m_torrent_tab = 0") );
  return *m_torrent_tab ;
}

ChatPanel* MainWindow::GetActiveChatPanel()
{
  int index = m_func_tabs->GetSelection();
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
void MainWindow::OpenChannelChat( Channel& channel )
{
  ASSERT_LOGIC( m_chat_tab != 0, _T("m_chat_tab") );
  m_func_tabs->SetSelection( 0 );
  m_chat_tab->AddChatPannel( channel );
}


//! @brief Open a new chat tab with a private chat
//!
//! @param nick The user to whom the chatwindow should be opened to
void MainWindow::OpenPrivateChat( User& user )
{
  ASSERT_LOGIC( m_chat_tab != 0, _T("m_chat_tab") );
  m_func_tabs->SetSelection( 0 );
  m_chat_tab->AddChatPannel( user );
}


//! @brief Close all chat tabs, both private and channel
//!
//! @todo Implement
void MainWindow::CloseAllChats()
{
}


//! @brief Displays the lobby configuration.
void MainWindow::ShowConfigure()
{
  m_func_tabs->SetSelection( 3 );
}


//! @brief Called when join channel menuitem is clicked
void MainWindow::OnMenuJoin( wxCommandEvent& event )
{

  if ( !m_ui.IsConnected() ) return;
  wxString answer;
  if ( m_ui.AskText( _("Join channel..."), _("Name of channel to join"), answer ) ) {
    m_ui.JoinChannel( answer, _T("") );
  }

}


void MainWindow::OnMenuChat( wxCommandEvent& event )
{

  if ( !m_ui.IsConnected() ) return;
  wxString answer;
  if ( m_ui.AskText( _("Open Private Chat..."), _("Name of user"), answer ) ) {
    if (m_ui.GetServer().UserExists( answer ) ) {
      OpenPrivateChat( m_ui.GetServer().GetUser( answer ) );
    }
  }

}

void MainWindow::OnMenuAbout( wxCommandEvent& event )
{
#ifdef HAVE_WX28
    wxAboutDialogInfo info;
	info.SetName(_T("SpringLobby"));
	info.SetVersion (GetSpringLobbyVersion());
	info.SetDescription(_("SpringLobby is a cross-plattform lobby client for the RTS Spring engine"));
	//info.SetCopyright(_T("");
	info.SetLicence(_T("GPL"));
	info.AddDeveloper(_T("BrainDamage"));
	info.AddDeveloper(_T("dizekat"));
	info.AddDeveloper(_T("koshi"));
	info.AddDeveloper(_T("semi_"));
	info.AddDeveloper(_T("tc-"));
  info.AddTranslator(_T("chaosch (simplified chinese)"));
	info.AddTranslator(_T("lejocelyn (french)"));
	info.AddTranslator(_T("Suprano (german)"));
  info.AddTranslator(_T("tc- (swedish)"));
	info.SetIcon(wxIcon(springlobby_xpm));
	wxAboutBox(info);

#else
    customMessageBoxNoModal(SL_MAIN_ICON,_T("SpringLobby version: ")+GetSpringLobbyVersion(),_T("About"));
#endif
}

void MainWindow::OnMenuConnect( wxCommandEvent& event )
{
  m_ui.ShowConnectWindow();
}


void MainWindow::OnMenuDisconnect( wxCommandEvent& event )
{
  m_ui.Disconnect();
}


void MainWindow::OnMenuQuit( wxCommandEvent& event )
{
  m_ui.Quit();
}

//! @brief checks for latest version of SpringLobby via HTTP, and compares it with users current version.
void MainWindow::OnMenuVersion( wxCommandEvent& event )
{
  wxString latestVersion = GetLatestVersion();
  // Need to replace crap chars or versions will always be inequal
  latestVersion.Replace(_T(" "), _T(""), true);
  latestVersion.Replace(_T("\n"), _T(""), true);
  latestVersion.Replace(_T("\t"), _T(""), true);
  if (latestVersion == _T("-1"))
  {
    customMessageBoxNoModal(SL_MAIN_ICON, _("There was an error checking for the latest version.\nPlease try again later.\nIf the problem persists, please use Help->Report Bug to report this bug."), _("Error"));
    return;
  }
  wxString myVersion = GetSpringLobbyVersion();

  wxString msg = _("Your Version: ") + myVersion + _T("\n") + _("Latest Version: ") + latestVersion;

  if (latestVersion.IsSameAs(myVersion, false))
  {
    customMessageBoxNoModal(SL_MAIN_ICON, _("Your SpringLobby version is up to date!\n\n") + msg, _("Up to Date"));
  }
  else
  {
    int answer = customMessageBox(SL_MAIN_ICON, _("Your SpringLobby version is not up to date.\n\n") + msg + _("\n\nWould you like to visit a page with instructions on how to download the newest version?"), _("Not up to Date"), wxYES_NO);
    if (answer == wxYES)
    {
      m_ui.OpenWebBrowser(_T("http://trac.springlobby.info/wiki/Install"));
    }
  }
}

void MainWindow::OnUnitSyncReload( wxCommandEvent& event )
{
  m_ui.ReloadUnitSync();
}


void MainWindow::OnReportBug( wxCommandEvent& event )
{
  m_ui.OpenWebBrowser( _T("http://trac.springlobby.info/newticket") );
}


void MainWindow::OnShowDocs( wxCommandEvent& event )
{
  m_ui.OpenWebBrowser( _T("http://springlobby.info") );
}

void MainWindow::OnTabsChanged( wxListbookEvent& event )
{
  MakeImages();

  int newsel = event.GetSelection();

  if ( newsel == 0 || newsel == 1 )
  {
    if ( !m_ui.IsConnected() && m_ui.IsMainWindowCreated() ) m_ui.Connect();
  }
}

void MainWindow::OnUnitSyncReloaded()
{
  wxLogDebugFunc( _T("") );
  wxLogMessage( _T("Reloading join tab") );
  GetJoinTab().OnUnitSyncReloaded();
  wxLogMessage( _T("Join tab updated") );
  wxLogMessage( _T("Reloading Singleplayer tab") );
  GetSPTab().OnUnitSyncReloaded();
  wxLogMessage( _T("Singleplayer tab updated") );
}

void MainWindow::OnShowSettingsPP( wxCommandEvent& event )
{
	se_frame = new settings_frame(this,wxID_ANY,wxT("Settings++"),wxDefaultPosition,
	  	    		wxDefaultSize,wxMINIMIZE_BOX  | wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN);
	se_frame_active = true;
	se_frame->Show();
}
