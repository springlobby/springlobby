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
// Class: MainChatWindow
// Created on: Tue May  1 13:32:49 2007
//

#ifndef _MAINCHATWINDOW_H_
#define _MAINCHATWINDOW_H_

#include <wx/wx.h>

#include "chatwindow.h"

class MainChatWindow : public wxPanel
{
  public:
    MainChatWindow( wxWindow* parent );
    virtual ~MainChatWindow();
  
    // MainChatWindow interface
  
  protected:
    // MainChatWindow variables
  
    wxNotebook* m_chat_tabs;
    wxBoxSizer* m_main_sizer;
    ChatWindow* m_server_chat;
    ChatWindow* m_main_chat;
  
};


#endif  //_MAINCHATWINDOW_H_

