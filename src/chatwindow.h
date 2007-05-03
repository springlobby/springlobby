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
// Class: ChatWindow
// Created on: Wed May  2 21:07:18 2007
//

#ifndef _CHATWINDOW_H_
#define _CHATWINDOW_H_

#include <wx/wx.h>

class ChatWindow : public wxPanel
{
  public:
    ChatWindow( wxWindow* parent, bool show_nick_list );
    virtual ~ChatWindow();
  
    // ChatWindow interface
  
    // TODO: add member function declarations...
  
  protected:
    // ChatWindow variables
  
    bool m_show_nick_list;
  
    wxBoxSizer* m_main_sizer;
    wxBoxSizer* m_chat_sizer;
    wxBoxSizer* m_say_sizer;
    wxBoxSizer* m_nick_sizer;
  
    wxTextCtrl* m_chatlog_text;
    wxTextCtrl* m_say_text;
  
    wxButton* m_say_button;
  
};


#endif  //_CHATWINDOW_H_

