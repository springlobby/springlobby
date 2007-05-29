//
// Class: ChatPanel
// Created on: Wed May  2 21:07:18 2007
//

/*! @file
 * @brief Implements the ChatPanel class.
 */

#ifndef _CHATPANEL_H_
#define _CHATPANEL_H_

#include <wx/splitter.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/listctrl.h>
#include <wx/combobox.h>
#include <string>

#include "nicklistctrl.h"


#define SERVER_CHAT_NAME "$server"

enum ChatPanelType {
  CPT_Channel,
  CPT_Server,
  CPT_User
};

class Channel;
class Server;

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
    //ChatPanel( wxWindow* parent, bool show_nick_list );
    ChatPanel( wxWindow* parent, Channel& chan );
    ChatPanel( wxWindow* parent, User& user );
    ChatPanel( wxWindow* parent, Server& serv );
    ~ChatPanel();
  
    // ChatPanel interface
  
    void Said( const wxString& who, const wxString& message );
    void DidAction( const wxString& who, const wxString& action );
    void Motd( const wxString& message );
  
    void UnknownCommand( const wxString& command, const wxString& params );
  
    void Joined( User& who );
    void Parted( User& who, const wxString& message );
    void SetTopic( const wxString& who, const wxString& message );
  
    Channel& GetChannel() { return *m_channel; }
  
    bool IsServerPanel() { return (m_type == CPT_Server); }
    
    void Say( const wxString& message );
    void Part();
  
    void OnSay( wxCommandEvent& event );
    void OnResize( wxSizeEvent& event );
  protected:
    // ChatPanel variables
  
    void _SetChannel( Channel* channel );
    
    bool m_show_nick_list;      //!< If the nicklist should be shown or not.
  
    wxBoxSizer* m_main_sizer;   //!< Main sizer containing all other sizers.
    wxBoxSizer* m_chat_sizer;   //!< Sizer containing the chat messages, and send input and button.
    wxBoxSizer* m_say_sizer;    //!< Sizer containing send input and button.
    wxBoxSizer* m_nick_sizer;   //!< Sizer containing the nicklist.
  
    wxSplitterWindow* m_splitter; //!< The splitter.
    wxPanel* m_chat_panel;      //!< Panel containing the chat. Only used when nicklist is visible.
    wxPanel* m_nick_panel;      //!< Panel containing the nicklist.
  
    wxTextCtrl* m_chatlog_text; //!< The chat log textcontrol.
    wxTextCtrl* m_say_text;     //!< The say textcontrol.
  
    NickListCtrl* m_nicklist;   //!< The nicklist.
    wxComboBox* m_nick_filter;  //!< The filter combo.
  
    wxButton* m_say_button;     //!< The say button.
  
    Channel* m_channel;         //!< Channel object.
    Server* m_server;          //!< Server object.
    User* m_user;            //!< User object.

    ChatPanelType m_type;       //!< Channel object.

    void LogTime();
    void _CreateControls( );
  
    DECLARE_EVENT_TABLE()
};


enum
{
    CHAT_SEND = wxID_HIGHEST,
    CHAT_TEXT,
};

#endif  //_CHATPANEL_H_

