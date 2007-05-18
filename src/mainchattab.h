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
// Created on: Tue May  1 13:32:49 2007
//

#ifndef _MAINCHATTAB_H_
#define _MAINCHATTAB_H_

//#include <wx/wx.h>

#include "chatpanel.h"


//! @brief The main chat tab.
class MainChatTab : public wxPanel
{
  public:
    MainChatTab( wxWindow* parent );
    virtual ~MainChatTab();
  
    // MainChatWindow interface
  
    ChatPanel& ServerChat() { assert( m_server_chat != NULL ); return *m_server_chat; }
    
    ChatPanel* AddChatPannel( Channel& channel, bool nick_list );
    
    void OnTabsChanged( wxNotebookEvent& event );
    
    
  protected:
    // MainChatWindow variables
    wxWindow* m_close_window;
    wxNotebook* m_chat_tabs;
    wxBoxSizer* m_main_sizer;
    wxImageList* m_imagelist;
    ChatPanel* m_server_chat;
    ChatPanel* m_main_chat;
    int m_newtab_sel;
  
    DECLARE_EVENT_TABLE()
};

enum
{
    CHAT_TABS = wxID_HIGHEST
};

#endif  //_MAINCHATTAB_H_

