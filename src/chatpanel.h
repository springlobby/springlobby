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
// Class: ChatPanel
// Created on: Wed May  2 21:07:18 2007
//

/*! @file
 * @brief Implements the ChatPanel class.
 */

#ifndef _CHATPANEL_H_
#define _CHATPANEL_H_

#include <wx/wx.h>
#include <string>

using namespace std;

/*! @brief wxPanel that contains a chat.
 *
 * This panel contains a chat with a multiline TextCtrl for the messages, a 
 * signle line TextCtrl for messages to send, a send button and a nick list.
 * 
 * The nick list is optional and can be removed by setting show_nick_list in the
 * constructor to false.
 */
class ChatPanel : public wxPanel
{
  public:
    ChatPanel( wxWindow* parent, bool show_nick_list );
    virtual ~ChatPanel();
  
    // ChatPanel interface
  
    void Said( wxString who, wxString message ) { m_chatlog_text->WriteText( who + _(": ")+ message ); }
    
    void SetChannelName( const string chan_name ) { m_chan_name = chan_name; }
    string GetChannelName() { return m_chan_name; }
    bool IsServerPanel() { return (m_chan_name == "$server"); }
    
  protected:
    // ChatPanel variables
  
    bool m_show_nick_list;
  
    wxBoxSizer* m_main_sizer;
    wxBoxSizer* m_chat_sizer;
    wxBoxSizer* m_say_sizer;
    wxBoxSizer* m_nick_sizer;
  
    wxTextCtrl* m_chatlog_text;
    wxTextCtrl* m_say_text;
  
    wxButton* m_say_button;
  
    string m_chan_name;
    bool m_server_chat;
  
};


#endif  //_CHATPANEL_H_

