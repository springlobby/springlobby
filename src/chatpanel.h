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

#define SERVER_CHAT_NAME "$server"

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
  
    void Said( wxString who, wxString message );
    void DidAction( wxString who, wxString action );
    void Motd( wxString message );
  
    void UnknownCommand( wxString command, wxString params );
  
    void Joined( wxString who );
    void Parted( wxString who, wxString message );
    void SetTopic( wxString who, wxString message );
  
    void SetChannelName( const string chan_name );
    string GetChannelName();
    bool IsServerPanel();
    
    void Say( wxString message );
  
    void OnSay( wxCommandEvent& event );
  protected:
    // ChatPanel variables
  
    bool m_show_nick_list;      //!< If the nicklist should be shown or not.
  
    wxBoxSizer* m_main_sizer;   //!< Main sizer containing all other sizers.
    wxBoxSizer* m_chat_sizer;   //!< Sizer containing the chat messages, and send input and button.
    wxBoxSizer* m_say_sizer;    //!< Sizer containing send input and button.
    wxBoxSizer* m_nick_sizer;   //!< Sizer containing the nicklist.
  
    wxTextCtrl* m_chatlog_text; //!< The chat log textcontrol.
    wxTextCtrl* m_say_text;     //!< The say textcontrol.
  
    wxButton* m_say_button;     //!< The say button.
  
    string m_chan_name;         //!< Name of the chat/channel.
  
    DECLARE_EVENT_TABLE()
};


enum
{
    CHAT_SEND = wxID_HIGHEST,
    CHAT_TEXT,
};

#endif  //_CHATPANEL_H_

