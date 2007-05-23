//
// Class: MainWindow
//

#include <wx/frame.h>
#include <wx/intl.h>
#include <wx/textdlg.h>
#include <wx/imaglist.h>
#include "mainwindow.h"
#include "system.h"
#include "settings.h"
#include "ui.h"
#include "images/bot.xpm"


BEGIN_EVENT_TABLE(MainWindow, wxFrame)

  EVT_MENU( MENU_JOIN, MainWindow::OnMenuJoin )
  EVT_MENU( MENU_CONNECT, MainWindow::OnMenuConnect )
  EVT_MENU( MENU_DISCONNECT, MainWindow::OnMenuDisconnect )

END_EVENT_TABLE()


MainWindow::MainWindow() : wxFrame((wxFrame *)NULL, -1, _T("Spring Lobby"),
                               wxPoint(50, 50), wxSize(450, 340))
{
  SetIcon( wxICON(bot) );
  wxMenu *menuFile = new wxMenu;
  menuFile->Append(MENU_CONNECT, _T("&Connect..."));
  menuFile->Append(MENU_DISCONNECT, _T("&Disconnect"));
  menuFile->AppendSeparator();
  menuFile->Append(MENU_QUIT, _T("&Quit"));

  wxMenu *menuEdit = new wxMenu;

  wxMenu *menuTools = new wxMenu;
  menuTools->Append(MENU_JOIN, _T("&Join channel..."));

  wxMenu *menuHelp = new wxMenu;
  menuHelp->Append(MENU_ABOUT, _T("&About"));

  wxMenuBar *menubar = new wxMenuBar;
  menubar->Append(menuFile, _T("&File"));
  menubar->Append(menuEdit, _T("&Edit"));
  menubar->Append(menuTools, _T("&Tools"));
  menubar->Append(menuHelp, _T("&Help"));
  SetMenuBar(menubar);

  m_main_sizer = new wxBoxSizer( wxHORIZONTAL );
  m_func_tabs = new wxListbook( this, -1, wxDefaultPosition, wxDefaultSize, wxLB_LEFT );

  m_func_tab_images = new wxImageList( 64, 64 );
  m_func_tab_images->Add( wxBITMAP(chat_icon) );

  m_func_tabs->AssignImageList( m_func_tab_images );

  m_chat_tab = new MainChatTab( m_func_tabs );
  m_func_tabs->AddPage( m_chat_tab, _(""), true, 0 );
  //m_func_tabs->AddPage( m_chat_tab, _(""), false, 0 );

  m_main_sizer->Add( m_func_tabs, 1, wxEXPAND | wxALL, 2 );

  SetSizer( m_main_sizer );

  SetSize( sett().GetMainWindowLeft(), sett().GetMainWindowTop(), sett().GetMainWindowWidth(), sett().GetMainWindowHeight() );
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
}


//! @brief Get reference to the MainWindow
MainWindow& mw()
{
  return ui().mw();
}


//! @brief Get the ChatPanel dedicated to server output and input
ChatPanel& servwin()
{
  return ui().mw().GetChatTab().ServerChat();
}


//! @brief Returns the curent MainChatTab object
MainChatTab& MainWindow::GetChatTab()
{
  assert( m_chat_tab != NULL );
  return *m_chat_tab;
}


//! @brief Open a new chat tab with a channel chat
//!
//! @param channel The channel name
//! @note This does NOT join the chatt.
//! @sa Server::JoinChannel OpenPrivateChat
void MainWindow::OpenChannelChat( Channel& channel )
{
  assert( m_chat_tab != NULL );
  ChatPanel* chat = m_chat_tab->AddChatPannel( channel, true );
  chat->SetChannel( &channel );
}


//! @brief Open a new chat tab with a private chat
//!
//! @param nick The user to whom the chatwindow should be opened to
void MainWindow::OpenPrivateChat( wxString nick )
{
}


//! @brief Close all chat tabs, both private and channel
//!
//! @todo Implement
void MainWindow::CloseAllChats()
{
}


//! @brief Called when join channel menuitem is clicked
void MainWindow::OnMenuJoin( wxCommandEvent& event )
{
  if ( sys().serv() == NULL ) return;
  wxTextEntryDialog name_dlg( NULL, _("Name of channel to join"), _("Join channel..."), _(""), wxOK | wxCANCEL | wxCENTRE );
  if (name_dlg.ShowModal() == wxID_OK) {
    //OpenChannelChat( name_dlg.GetValue() );
    sys().serv()->JoinChannel( STL_STRING(name_dlg.GetValue()), "" );
  }
}


void MainWindow::OnMenuConnect( wxCommandEvent& event )
{
  ui().ShowConnectWindow();
}

void MainWindow::OnMenuDisconnect( wxCommandEvent& event )
{
  sys().Disconnect();
}

