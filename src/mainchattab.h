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
// Class: MainChatPanel
// Created on: Tue May  1 13:32:49 2007
//

#ifndef _MAINCHATPANEL_H_
#define _MAINCHATPANEL_H_

#include <wx/wx.h>

#include "chatpanel.h"

class MainChatPanel : public wxPanel
{
  public:
    MainChatPanel( wxWindow* parent );
    virtual ~MainChatPanel();
  
    // MainChatWindow interface
  
    ChatPanel* ServerChat() { return m_server_chat; }
  
  protected:
    // MainChatWindow variables
  
    wxNotebook* m_chat_tabs;
    wxBoxSizer* m_main_sizer;
    ChatPanel* m_server_chat;
    ChatPanel* m_main_chat;
  
};


#endif  //_MAINCHATWINDOW_H_

