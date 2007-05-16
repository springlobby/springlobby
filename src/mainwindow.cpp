/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */


//
// Class: MainWindow
//
// Created on: Tue May  1 11:55:40 2007
//

#include <wx/frame.h>
#include <wx/intl.h>
#include <wx/textdlg.h>

#include "mainwindow.h"
#include "springlobbyapp.h"

BEGIN_EVENT_TABLE(MainWindow, wxFrame)

  EVT_MENU( MENU_JOIN, MainWindow::OnMenuJoin )

END_EVENT_TABLE()


MainWindow::MainWindow() : wxFrame((wxFrame *)NULL, -1, _T("Spring Lobby"),
                               wxPoint(50, 50), wxSize(450, 340))
{
  
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
  
  m_main_sizer->Add( m_func_tabs, 1, wxEXPAND, 0 );
  
  SetSizer( m_main_sizer );
  
  SetSize( app().GetMainWindowLeft(), app().GetMainWindowTop(), app().GetMainWindowWidth(), app().GetMainWindowHeight() );
}


MainWindow::~MainWindow()
{
  int x, y, w, h;
  GetSize( &w, &h );
  app().SetMainWindowHeight( h );
  app().SetMainWindowWidth( w );
  GetPosition( &x, &y );
  app().SetMainWindowTop( y );
  app().SetMainWindowLeft( x );
}


//! @brief Open a new chat tab with a channel chat
//!
//! @param channel The channel name
//! @note This does NOT join the chatt.
//! @see Server::JoinChannel
void MainWindow::OpenChannelChat( wxString channel )
{
  if ( !app().ChannelExists( STL_STRING(channel) ) ) {
    assert( m_chat_tab != NULL );
    ChatPanel* chat = m_chat_tab->AddChatPannel( channel, true );
    chat->SetChannelName( STL_STRING(channel) );
    app().SetChannelPanel( STL_STRING(channel), chat );
  }
}


//! @brief Open a new chat tab with a private chat
//!
//! @param nick The user to whom the chatwindow should be opened to
void MainWindow::OpenPrivateChat( wxString nick )
{
}


void MainWindow::OnMenuJoin( wxCommandEvent& event )
{
  if ( app().Serv() == NULL ) return;
  wxTextEntryDialog name_dlg( NULL, _("Name of channel to join"), _("Join channel..."), _(""), wxOK | wxCANCEL | wxCENTRE );
  if (name_dlg.ShowModal() == wxID_OK) {
    OpenChannelChat( name_dlg.GetValue() );
    app().Serv()->JoinChannel( STL_STRING(name_dlg.GetValue()), "" );
  }
}
