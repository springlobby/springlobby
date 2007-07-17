//
// Class: MainChatTab
//

#include <stdexcept>
#include <wx/intl.h>
#include <wx/imaglist.h>
#include <wx/sizer.h>

#include "mainchattab.h"
#include "utils.h"
#include "mainwindow.h"
#include "channel.h"
#include "user.h"
#include "chatpanel.h"

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
  assert( m_server_chat != 0 );
  return *m_server_chat;
}

ChatPanel* MainChatTab::AddChatPannel( Channel& channel )
{
  ChatPanel* chat = new ChatPanel( m_chat_tabs, m_ui, channel );
  m_chat_tabs->InsertPage( m_chat_tabs->GetPageCount() - 1, chat, WX_STRING(channel.GetName()), true, 2 );
  return chat;
}

ChatPanel* MainChatTab::AddChatPannel( Server& server, const wxString& name )
{
  ChatPanel* chat;

  for ( unsigned int i = 0; i < m_chat_tabs->GetPageCount(); i++ ) {
    if ( m_chat_tabs->GetPageText(i) == name ) {
      ChatPanel* tmp = (ChatPanel*)m_chat_tabs->GetPage(i);
      if ( tmp->GetServer() == 0 ) {
        m_chat_tabs->SetSelection( i );
        tmp->SetServer( &server );
        return tmp;
      }
    }
  }

  chat = new ChatPanel( m_chat_tabs, m_ui, server );
  m_chat_tabs->InsertPage( m_chat_tabs->GetPageCount() - 1, chat, name, true, 1 );

  return chat;
}

ChatPanel* MainChatTab::AddChatPannel( User& user )
{
  ChatPanel* chat = new ChatPanel( m_chat_tabs, m_ui, user );
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
  if ( newpage == 0 ) { // Not sure what to do here
    debug_error( "Newpage NULL." );
    return;
  }

  if ( newsel >= (int)m_chat_tabs->GetPageCount() - 1 ) { // We are going to remove page
    ChatPanel* delpage = (ChatPanel*)m_chat_tabs->GetPage( oldsel );
    ASSERT_LOGIC( delpage != 0 , "MainChatTab::OnTabsChanged(): delpage NULL" );

    if ( !delpage->IsServerPanel() ) {
      delpage->Part();
      m_chat_tabs->DeletePage( oldsel );
      m_chat_tabs->SetSelection( 0 );
    } else {
      m_chat_tabs->SetSelection( 0 );
    }

  }

}

