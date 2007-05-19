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
// Class: MainChatTab
//
// Created on: Tue May  1 13:32:49 2007
//

#include <wx/intl.h>
#include "mainchattab.h"
#include "system.h"
#include "utils.h"
#include "mainwindow.h"

#include "images/close.xpm"

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
  m_imagelist->Add( wxBITMAP(close) );
  
  m_chat_tabs->AssignImageList( m_imagelist );

  m_server_chat = new ChatPanel( m_chat_tabs, false );
//  sys().SetServerPanel( m_server_chat );

  m_chat_tabs->AddPage( m_server_chat, _T("Server"), true, -1 );
  
  m_close_window = new wxWindow( m_chat_tabs, -1 );
  m_chat_tabs->AddPage( m_close_window, _(""), false, 0 );
  
  m_main_sizer->Add( m_chat_tabs, 1, wxEXPAND );
  
  SetSizer( m_main_sizer );
  m_main_sizer->SetSizeHints( this );
}


MainChatTab::~MainChatTab()
{
  
}


ChatPanel* MainChatTab::AddChatPannel( Channel& channel, bool nick_list )
{
  ChatPanel* chat = new ChatPanel( m_chat_tabs, nick_list );
  chat->SetChannel( &channel );
  m_chat_tabs->InsertPage( m_chat_tabs->GetPageCount() - 1, chat, WX_STRING(channel.GetName()), true, -1 );
  return chat;
}

void MainChatTab::OnTabsChanged( wxNotebookEvent& event )
{
  std::cout << "** MainChatTab::OnTabsChanged()" << std::endl;

  int oldsel = event.GetOldSelection();
  if ( oldsel < 0 ) return;
  int newsel = event.GetSelection();
  if ( newsel < 0 ) return;
    
  wxWindow* newpage = m_chat_tabs->GetPage( newsel );
  if ( newpage == NULL ) { // Not sure what to do here
    std::cout << "  !! Newpage NULL." << std::endl;
    return;
  }
  
  std::cout << "  -- newsel: " << newsel << " tot: " << m_chat_tabs->GetPageCount() << std::endl;
  if ( newsel >= m_chat_tabs->GetPageCount() - 1 ) { // We are going to remove page
    std::cout << "  -- Closepage." << std::endl;
    ChatPanel* delpage = (ChatPanel*)m_chat_tabs->GetPage( oldsel );
    if ( (delpage != &servwin()) && ( delpage != NULL ) ) {
      std::cout << "  -- Remove last." << std::endl;
      delpage->Part();
      m_chat_tabs->DeletePage( oldsel );
      m_chat_tabs->SetSelection( 0 );
    } else {
      std::cout << "  -- Select first." << std::endl;
      m_chat_tabs->SetSelection( 0 );
    }
    
  }
  
}

