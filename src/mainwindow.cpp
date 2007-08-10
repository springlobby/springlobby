/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: MainWindow
//

#include <wx/frame.h>
#include <wx/intl.h>
#include <wx/textdlg.h>
#include <wx/imaglist.h>
#include <wx/icon.h>
#include <wx/sizer.h>
#include <wx/listbook.h>
#include <wx/menu.h>
#include <wx/dcmemory.h>

#include "mainwindow.h"
#include "settings.h"
#include "ui.h"
#include "server.h"
#include "utils.h"
#include "battlelisttab.h"
#include "mainchattab.h"
#include "mainjoinbattletab.h"
#include "mainoptionstab.h"
#include "iunitsync.h"

#include "images/springlobby.xpm"
#include "images/chat_icon.xpm"
#include "images/join_icon.xpm"
#include "images/options_icon.xpm"
#include "images/select_icon.xpm"


BEGIN_EVENT_TABLE(MainWindow, wxFrame)

  EVT_MENU( MENU_JOIN, MainWindow::OnMenuJoin )
  EVT_MENU( MENU_CHAT, MainWindow::OnMenuChat )
  EVT_MENU( MENU_CONNECT, MainWindow::OnMenuConnect )
  EVT_MENU( MENU_DISCONNECT, MainWindow::OnMenuDisconnect )
  EVT_MENU( MENU_QUIT, MainWindow::OnMenuQuit )
  EVT_MENU( MENU_USYNC, MainWindow::OnUnitSyncReload )
  EVT_MENU( MENU_TRAC, MainWindow::OnReportBug )
  EVT_MENU( MENU_DOC, MainWindow::OnShowDocs )

  EVT_LISTBOOK_PAGE_CHANGED( MAIN_TABS, MainWindow::OnTabsChanged )

END_EVENT_TABLE()


MainWindow::MainWindow( Ui& ui ) : wxFrame((wxFrame *)0, -1, _("Spring Lobby"),
                               wxPoint(50, 50), wxSize(450, 340)), m_ui(ui)
{
  SetIcon( wxIcon(springlobby_xpm) );
  wxMenu *menuFile = new wxMenu;
  menuFile->Append(MENU_CONNECT, _("&Connect..."));
  menuFile->Append(MENU_DISCONNECT, _("&Disconnect"));
  menuFile->AppendSeparator();
  menuFile->Append(MENU_QUIT, _("&Quit"));

  wxMenu *menuEdit = new wxMenu;

  wxMenu *menuTools = new wxMenu;
  menuTools->Append(MENU_JOIN, _("&Join channel..."));
  menuTools->Append(MENU_CHAT, _("Open &chat..."));
  menuTools->AppendSeparator();
  menuTools->Append(MENU_USYNC, _("&Reload maps/mods"));

  wxMenu *menuHelp = new wxMenu;
  menuHelp->Append(MENU_ABOUT, _("&About"));
  menuHelp->Append(MENU_TRAC, _("&Report a bug..."));
  menuHelp->Append(MENU_DOC, _("&Documentation"));

  wxMenuBar *menubar = new wxMenuBar;
  menubar->Append(menuFile, _("&File"));
  menubar->Append(menuEdit, _("&Edit"));
  menubar->Append(menuTools, _("&Tools"));
  menubar->Append(menuHelp, _("&Help"));
  SetMenuBar(menubar);

  m_main_sizer = new wxBoxSizer( wxHORIZONTAL );
  m_func_tabs = new wxListbook( this, MAIN_TABS, wxDefaultPosition, wxDefaultSize, wxLB_LEFT );

  m_chat_icon = new wxBitmap( chat_icon_xpm );
  m_battle_icon = new wxBitmap( join_icon_xpm );
  m_options_icon = new wxBitmap( options_icon_xpm );
  m_select_image = new wxBitmap( select_icon_xpm );

  m_func_tab_images = new wxImageList( 64, 64 );
  MakeImages();

  m_func_tabs->AssignImageList( m_func_tab_images );
  m_chat_tab = new MainChatTab( m_func_tabs, m_ui );
  m_join_tab = new MainJoinBattleTab( m_func_tabs, m_ui );
  m_opts_tab = new MainOptionsTab( m_func_tabs, m_ui );

  m_func_tabs->AddPage( m_chat_tab, _T(""), true, 0 );
  m_func_tabs->AddPage( m_join_tab, _T(""), false, 1 );
  m_func_tabs->AddPage( m_opts_tab, _T(""), false, 2 );

  m_main_sizer->Add( m_func_tabs, 1, wxEXPAND | wxALL, 2 );

  SetSizer( m_main_sizer );

  SetSize( sett().GetMainWindowLeft(), sett().GetMainWindowTop(), sett().GetMainWindowWidth(), sett().GetMainWindowHeight() );
  Layout();
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
  m_ui.OnMainWindowDestruct();

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


void MainWindow::MakeImages()
{
  m_func_tab_images->RemoveAll();

  if ( m_func_tabs->GetSelection() == 0 ) {
    wxBitmap img( *m_select_image );
    DrawBmpOnBmp( img, *m_chat_icon, 0, 0 );
    m_func_tab_images->Add( img );
  } else {
    m_func_tab_images->Add( *m_chat_icon );
  }

  if ( m_func_tabs->GetSelection() == 1 ) {
    wxBitmap img( *m_select_image );
    DrawBmpOnBmp( img, *m_battle_icon, 0, 0 );
    m_func_tab_images->Add( img );
  } else {
    m_func_tab_images->Add( *m_battle_icon );
  }

  if ( m_func_tabs->GetSelection() == 2 ) {
    wxBitmap img( *m_select_image );
    DrawBmpOnBmp( img, *m_options_icon, 0, 0 );
    m_func_tab_images->Add( img );
  } else {
    m_func_tab_images->Add( *m_options_icon );
  }

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
  assert( m_chat_tab != 0 );
  return *m_chat_tab;
}

MainJoinBattleTab& MainWindow::GetJoinTab()
{
  assert( m_join_tab != 0 );
  return *m_join_tab;
}

//! @brief Open a new chat tab with a channel chat
//!
//! @param channel The channel name
//! @note This does NOT join the chatt.
//! @sa Server::JoinChannel OpenPrivateChat
void MainWindow::OpenChannelChat( Channel& channel )
{
  assert( m_chat_tab != 0 );
  m_chat_tab->AddChatPannel( channel );
}


//! @brief Open a new chat tab with a private chat
//!
//! @param nick The user to whom the chatwindow should be opened to
void MainWindow::OpenPrivateChat( User& user )
{
  assert( m_chat_tab != 0 );
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
  m_func_tabs->SetSelection( 2 );
}


//! @brief Called when join channel menuitem is clicked
void MainWindow::OnMenuJoin( wxCommandEvent& event )
{

  if ( !m_ui.IsConnected() ) return;
  wxString answer;
  if ( Ui::AskText( _("Join channel..."), _("Name of channel to join"), answer ) ) {
    m_ui.JoinChannel( answer, _T("") );
  }

}


void MainWindow::OnMenuChat( wxCommandEvent& event )
{

  if ( !m_ui.IsConnected() ) return;
  wxString answer;
  if ( Ui::AskText( _("Open Private Chat..."), _("Name of user"), answer ) ) {
    if (m_ui.GetServer().UserExists( STD_STRING(answer) ) ) {
      OpenPrivateChat( m_ui.GetServer().GetUser( STD_STRING(answer) ) );
    }
  }

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

void MainWindow::OnUnitSyncReload( wxCommandEvent& event )
{
  usync()->FreeUnitSyncLib();
  usync()->LoadUnitSyncLib();
  m_join_tab->GetBattleListTab().UpdateList();
}


void MainWindow::OnReportBug( wxCommandEvent& event )
{
  wxLaunchDefaultBrowser( _T("http://tc.serveftp.net/trac/newticket") );
}


void MainWindow::OnShowDocs( wxCommandEvent& event )
{
  wxLaunchDefaultBrowser( _T("http://tc.serveftp.net/trac") );
}


void MainWindow::OnTabsChanged( wxListbookEvent& event )
{
  MakeImages();
}

