/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: MainChatTab
//

#include <stdexcept>
#include <wx/intl.h>
#include <wx/imaglist.h>
#include <wx/sizer.h>
#include <wx/notebook.h>
#include <wx/listbook.h>

#include "mainchattab.h"
#include "utils.h"
#include "mainwindow.h"
#include "channel.h"
#include "user.h"
#include "chatpanel.h"
#include "ui.h"
#include "server.h"

#include "images/close.xpm"
#include "images/server.xpm"
#include "images/channel.xpm"
#include "images/userchat.xpm"

BEGIN_EVENT_TABLE(MainChatTab, wxPanel)

  EVT_NOTEBOOK_PAGE_CHANGED(CHAT_TABS, MainChatTab::OnTabsChanged)

END_EVENT_TABLE()


MainChatTab::MainChatTab( wxWindow* parent, Ui& ui )
: wxPanel( parent, -1, wxDefaultPosition, wxDefaultSize, 0, wxPanelNameStr ),m_ui(ui)
{

  m_newtab_sel = -1;
  m_server_chat = 0;

  m_main_sizer = new wxBoxSizer( wxVERTICAL );

  m_chat_tabs = new wxNotebook( this, CHAT_TABS, wxDefaultPosition, wxDefaultSize, wxLB_TOP );

  m_imagelist = new wxImageList( 12, 12 );
  m_imagelist->Add( wxBitmap(close_xpm) );
  m_imagelist->Add( wxBitmap(server_xpm) );
  m_imagelist->Add( wxBitmap(channel_xpm) );
  m_imagelist->Add( wxBitmap(userchat_xpm) );

  m_chat_tabs->AssignImageList( m_imagelist );

//  m_server_chat = new ChatPanel( m_chat_tabs, serv );
//  m_chat_tabs->AddPage( m_server_chat, _("Server"), true, 1 );

  m_close_window = new wxWindow( m_chat_tabs, -1 );
  m_chat_tabs->AddPage( m_close_window, _T(""), false, 0 );

  m_main_sizer->Add( m_chat_tabs, 1, wxEXPAND );

  SetSizer( m_main_sizer );
  m_main_sizer->SetSizeHints( this );
}


MainChatTab::~MainChatTab()
{

}

ChatPanel& MainChatTab::ServerChat()
{
  ASSERT_LOGIC( m_server_chat != 0, _T("m_server_chat = 0") );
  return *m_server_chat;
}


ChatPanel* MainChatTab::GetActiveChatPanel()
{
  return (ChatPanel*)m_chat_tabs->GetCurrentPage();
}


ChatPanel* MainChatTab::GetChannelChatPanel( const wxString& channel )
{
  for ( unsigned int i = 0; i < m_chat_tabs->GetPageCount(); i++ ) {
    ChatPanel* tmp = (ChatPanel*)m_chat_tabs->GetPage(i);
    if ( tmp->GetPanelType() == CPT_Channel ) {
      wxString name = m_chat_tabs->GetPageText(i);
      if ( name.Lower() == channel.Lower() ) return (ChatPanel*)m_chat_tabs->GetPage(i);
    }
  }
  return 0;
}


ChatPanel* MainChatTab::GetUserChatPanel( const wxString& user )
{
  for ( unsigned int i = 0; i < m_chat_tabs->GetPageCount(); i++ ) {
    ChatPanel* tmp = (ChatPanel*)m_chat_tabs->GetPage(i);
    if ( tmp->GetPanelType() == CPT_User ) {
      wxString name = m_chat_tabs->GetPageText(i);
      if ( name.Lower() == user.Lower() ) return (ChatPanel*)m_chat_tabs->GetPage(i);
    }
  }
  return 0;
}


void MainChatTab::OnUserConnected( User& user )
{
  ChatPanel* panel = GetUserChatPanel( WX_STRING(user.GetNick()) );
  if ( panel != 0 ) {
    panel->SetUser( &user );
    panel->OnUserConnected();
  }
}


void MainChatTab::OnUserDisconnected( User& user )
{
  ChatPanel* panel = GetUserChatPanel( WX_STRING(user.GetNick()) );
  if ( panel != 0 ) {
    panel->OnUserDisconnected();
    panel->SetUser( 0 );
  }
}


void MainChatTab::CloseAllChats()
{

  for ( unsigned int i = 0; i < m_chat_tabs->GetPageCount(); i++ ) {
    ChatPanel* tmp = (ChatPanel*)m_chat_tabs->GetPage(i);
    if ( tmp == 0 ) continue;
    if ( tmp->GetPanelType() == CPT_Channel ) tmp->SetChannel( 0 );
    else if ( tmp->GetPanelType() == CPT_User ) tmp->SetUser( 0 );
    else if ( tmp->GetPanelType() == CPT_Server ) tmp->SetServer( 0 );
  }

}


void MainChatTab::RejoinChannels()
{
  for ( unsigned int i = 0; i < m_chat_tabs->GetPageCount(); i++ ) {
    ChatPanel* tmp = (ChatPanel*)m_chat_tabs->GetPage(i);
    if ( tmp->GetPanelType() == CPT_Channel ) {

      // TODO: This will not rejoin passworded channels.
      std::string name = STD_STRING(m_chat_tabs->GetPageText(i));
      // #springlobby is joined automatically
      if ( name != "springlobby" ) m_ui.GetServer().JoinChannel( name, "" );

    } else if (tmp->GetPanelType() == CPT_User ) {

      std::string name = STD_STRING(m_chat_tabs->GetPageText(i));
      if ( m_ui.GetServer().UserExists( name ) ) tmp->SetUser( &m_ui.GetServer().GetUser( name ) );

    }
  }
}


ChatPanel* MainChatTab::AddChatPannel( Channel& channel )
{

  for ( unsigned int i = 0; i < m_chat_tabs->GetPageCount(); i++ ) {
    if ( m_chat_tabs->GetPageText(i) == WX_STRING(channel.GetName()) ) {
      ChatPanel* tmp = (ChatPanel*)m_chat_tabs->GetPage(i);
      if ( tmp->GetPanelType() == CPT_Channel ) {
        m_chat_tabs->SetSelection( i );
        tmp->SetChannel( &channel );
        return tmp;
      }
    }
  }

  ChatPanel* chat = new ChatPanel( m_chat_tabs, m_ui, channel );
  m_chat_tabs->InsertPage( m_chat_tabs->GetPageCount() - 1, chat, WX_STRING(channel.GetName()), true, 2 );
  return chat;
}

ChatPanel* MainChatTab::AddChatPannel( Server& server, const wxString& name )
{

  for ( unsigned int i = 0; i < m_chat_tabs->GetPageCount(); i++ ) {
    if ( m_chat_tabs->GetPageText(i) == name ) {
      ChatPanel* tmp = (ChatPanel*)m_chat_tabs->GetPage(i);
      if ( tmp->GetPanelType() == CPT_Server ) {
        m_chat_tabs->SetSelection( i );
        tmp->SetServer( &server );
        return tmp;
      }
    }
  }

  ChatPanel* chat = new ChatPanel( m_chat_tabs, m_ui, server );
  m_chat_tabs->InsertPage( m_chat_tabs->GetPageCount() - 1, chat, name, true, 1 );

  return chat;
}

ChatPanel* MainChatTab::AddChatPannel( User& user )
{
  for ( unsigned int i = 0; i < m_chat_tabs->GetPageCount(); i++ ) {
    if ( m_chat_tabs->GetPageText(i) == WX_STRING(user.GetNick()) ) {
      ChatPanel* tmp = (ChatPanel*)m_chat_tabs->GetPage(i);
      if ( tmp->GetPanelType() == CPT_User ) {
        m_chat_tabs->SetSelection( i );
        tmp->SetUser( &user );
        return tmp;
      }
    }
  }

  ChatPanel* chat = new ChatPanel( m_chat_tabs, m_ui, user );
  m_chat_tabs->InsertPage( m_chat_tabs->GetPageCount() - 1, chat, WX_STRING(user.GetNick()), true, 3 );
  return chat;
}


void MainChatTab::OnTabsChanged( wxNotebookEvent& event )
{
  wxLogDebugFunc( _T("") );

  int oldsel = event.GetOldSelection();
  if ( oldsel < 0 ) return;
  int newsel = event.GetSelection();
  if ( newsel < 0 ) return;

  wxWindow* newpage = m_chat_tabs->GetPage( newsel );
  if ( newpage == 0 ) { // Not sure what to do here
    wxLogError( _T("Newpage NULL.") );
    return;
  }

  if ( newsel >= (int)m_chat_tabs->GetPageCount() - 1 ) { // We are going to remove page
    ChatPanel* delpage = (ChatPanel*)m_chat_tabs->GetPage( oldsel );
    ASSERT_LOGIC( delpage != 0 , _T("MainChatTab::OnTabsChanged(): delpage NULL") );

    if ( !delpage->IsServerPanel() ) {
      delpage->Part();
      m_chat_tabs->DeletePage( oldsel );
      m_chat_tabs->SetSelection( 0 );
    } else {
      m_chat_tabs->SetSelection( 0 );
    }

  }

}

