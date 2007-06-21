//
// Class: MainChatTab
//

#include <wx/intl.h>
#include <wx/imaglist.h>
#include "mainchattab.h"
#include "utils.h"
#include "mainwindow.h"
#include "channel.h"

#include "images/close.xpm"
#include "images/server.xpm"
#include "images/channel.xpm"
#include "images/userchat.xpm"

BEGIN_EVENT_TABLE(MainChatTab, wxPanel)

  EVT_NOTEBOOK_PAGE_CHANGED(CHAT_TABS, MainChatTab::OnTabsChanged)

END_EVENT_TABLE()


MainChatTab::MainChatTab( wxWindow* parent )
: wxPanel( parent, -1, wxDefaultPosition, wxDefaultSize, 0, wxPanelNameStr )
{

  m_newtab_sel = -1;
  m_server_chat = NULL;

  m_main_sizer = new wxBoxSizer( wxVERTICAL );

  m_chat_tabs = new wxNotebook( this, CHAT_TABS, wxDefaultPosition, wxDefaultSize, wxLB_TOP );

  m_imagelist = new wxImageList( 12, 12 );
  m_imagelist->Add( wxBitmap(close_xpm) );
  m_imagelist->Add( wxBitmap(server_xpm) );
  m_imagelist->Add( wxBitmap(channel_xpm) );
  m_imagelist->Add( wxBitmap(userchat_xpm) );

  m_chat_tabs->AssignImageList( m_imagelist );

//  m_server_chat = new ChatPanel( m_chat_tabs, serv );
//  m_chat_tabs->AddPage( m_server_chat, _T("Server"), true, 1 );

  m_close_window = new wxWindow( m_chat_tabs, -1 );
  m_chat_tabs->AddPage( m_close_window, _(""), false, 0 );

  m_main_sizer->Add( m_chat_tabs, 1, wxEXPAND );

  SetSizer( m_main_sizer );
  m_main_sizer->SetSizeHints( this );
}


MainChatTab::~MainChatTab()
{

}


ChatPanel* MainChatTab::AddChatPannel( Channel& channel )
{
  ChatPanel* chat = new ChatPanel( m_chat_tabs, channel );
  m_chat_tabs->InsertPage( m_chat_tabs->GetPageCount() - 1, chat, WX_STRING(channel.GetName()), true, 2 );
  return chat;
}

ChatPanel* MainChatTab::AddChatPannel( Server& server, const wxString& name )
{
  ChatPanel* chat = new ChatPanel( m_chat_tabs, server );
  m_chat_tabs->InsertPage( m_chat_tabs->GetPageCount() - 1, chat, name, true, 1 );
  return chat;
}

ChatPanel* MainChatTab::AddChatPannel( User& user )
{
  ChatPanel* chat = new ChatPanel( m_chat_tabs, user );
  m_chat_tabs->InsertPage( m_chat_tabs->GetPageCount() - 1, chat, WX_STRING(user.GetNick()), true, 3 );
  return chat;
}


void MainChatTab::OnTabsChanged( wxNotebookEvent& event )
{
  debug_func( "" );

  int oldsel = event.GetOldSelection();
  if ( oldsel < 0 ) return;
  int newsel = event.GetSelection();
  if ( newsel < 0 ) return;

  wxWindow* newpage = m_chat_tabs->GetPage( newsel );
  if ( newpage == NULL ) { // Not sure what to do here
    debug_error( "Newpage NULL." );
    return;
  }

  if ( newsel >= (int)m_chat_tabs->GetPageCount() - 1 ) { // We are going to remove page
    ChatPanel* delpage = (ChatPanel*)m_chat_tabs->GetPage( oldsel );
    ASSERT_LOGIC( delpage != NULL , "MainChatTab::OnTabsChanged(): delpage NULL" );
    
    if ( !delpage->IsServerPanel() ) {
      delpage->Part();
      m_chat_tabs->DeletePage( oldsel );
      m_chat_tabs->SetSelection( 0 );
    } else {
      m_chat_tabs->SetSelection( 0 );
    }

  }

}

