#ifndef SPRINGLOBBY_HEADERGUARD_CHATPANEL_H
#define SPRINGLOBBY_HEADERGUARD_CHATPANEL_H

#include <wx/panel.h>
#include <wx/textctrl.h>
#include <vector>

#include "chatlog.h"
#include "helper/TextCompletionDatabase.hpp"
#include "utils/mixins.hh"
#include "utils/globalevents.h"

class wxCommandEvent;
class wxSizeEvent;
class wxBoxSizer;
class wxSplitterWindow;
class wxTextCtrl;
class wxTextCtrlHist;
class wxTextUrlEvent;
class wxComboBox;
class wxButton;
class wxBitmapButton;
class NickListCtrl;
class Channel;
class User;
class ChatLog;
class Server;
class Battle;
class Ui;
class wxStaticText;

class wxMouseEvent;
class SLNotebook;
class wxNotebook;
class wxImageList;
class ChatPanelMenu;

enum ChatPanelType {
  CPT_Channel,
  CPT_Server,
  CPT_User,
  CPT_Battle
};

enum HighlightType
{
  highlight_say,
  highlight_join_leave,
  highlight_important
};

struct ChatLine
{
  wxString chat;
  wxString time;
  wxTextAttr timestyle;
  wxTextAttr chatstyle;
};

/*! @brief wxPanel that contains a chat.
 *
 * This panel contains a chat with a multiline TextCtrl for the messages, a
 * signle line TextCtrl for messages to send, a send button and a nick list.
 *
 * The nick list is optional and can be removed by setting show_nick_list in the
 * constructor to false.
 */
class ChatPanel : public wxPanel, public GlobalEvent, public SL::NonCopyable
{
public:

	ChatPanel( wxWindow* parent, Channel& chan, wxImageList* imaglist );
	ChatPanel( wxWindow* parent, const User& user, wxImageList* imaglist  );
	ChatPanel( wxWindow* parent, Server& serv, wxImageList* imaglist  );
	ChatPanel( wxWindow* parent, Battle* battle );
	~ChatPanel();

	void Said( const wxString& who, const wxString& message );
	void DidAction( const wxString& who, const wxString& action );
	void Motd( const wxString& message );
	void StatusMessage( const wxString& message );
	void ClientMessage( const wxString& message );

	void UnknownCommand( const wxString& command, const wxString& params );

	void Joined( User& who );
	void Parted( User& who, const wxString& message );
	void SetTopic( const wxString& who, const wxString& message );
	void UserStatusUpdated( User& who );
	void OnChannelJoin( User& who );

	const Channel* GetChannel() const;
	void SetChannel( Channel* chan );

	const Server* GetServer()  const;
	void SetServer( Server* serv );

	const User* GetUser() const ;
	void SetUser( const User* usr );

	bool IsServerPanel() const;
	ChatPanelType GetPanelType() const;

	void SetBattle( Battle* battle );

	void Part();
	void FocusInputBox();


	size_t GetIconIndex()  const { return m_icon_index; }
	void SetIconIndex( size_t index ) { m_icon_index = index; }

	void OnUserDisconnected();
	void OnUserConnected();

	void UpdateNicklistHighlights();

private:
	//! @returns true on success ( blank line ), false otherwise
	bool Say( const wxString& message );

	wxString GetChatTypeStr()  const;

	const User& GetMe()  const;
	const User* GetSelectedUser() const;

	void SortNickList();

	void OnChanOpts( wxCommandEvent& event );
	void OnSay( wxCommandEvent& event );
	void OnPaste( wxClipboardTextEvent& event );

	void OnLinkEvent( wxTextUrlEvent& event );
	void OnMouseDown( wxMouseEvent& event );

	void OnKeyPressed( wxKeyEvent& keyevent );
	void OnKeyReleased( wxKeyEvent& keyevent );

	bool IsOk() const;
	void OnMenuItem( wxCommandEvent& event );

	void OnLogin( wxCommandEvent& data );

	void _SetChannel( Channel* channel );
	void OutputLine( const wxString& message, const wxColour& col, const wxFont& fon );
	void OutputLine( const ChatLine& line );
	void SetIconHighlight( HighlightType highlight );
	wxString FindUrl( const long pos ) const ;

	bool ContainsWordToHighlight( const wxString& message ) const;
	bool m_show_nick_list;      //!< If the nicklist should be shown or not.

	wxBoxSizer* m_main_sizer;   //!< Main sizer containing all other sizers.
	wxBoxSizer* m_chat_sizer;   //!< Sizer containing the chat messages, and send input and button.
	wxBoxSizer* m_say_sizer;    //!< Sizer containing send input and button.
	wxBoxSizer* m_nick_sizer;   //!< Sizer containing the nicklist.

	wxSplitterWindow* m_splitter; //!< The splitter.
	wxPanel* m_chat_panel;      //!< Panel containing the chat. Only used when nicklist is visible.
	wxPanel* m_nick_panel;      //!< Panel containing the nicklist.

	wxTextCtrl* m_chatlog_text; //!< The chat log textcontrol.
	wxTextCtrlHist* m_say_text;     //!< The say textcontrol.
	wxBitmapButton* m_chan_opts_button; //!< The channel options button.

	NickListCtrl* m_nicklist;   //!< The nicklist.
	wxComboBox* m_nick_filter;  //!< The filter combo.

	wxButton* m_say_button;     //!< The say button.
	SLNotebook* m_chat_tabs;

	Channel* m_channel;         //!< Channel object.
	Server* m_server;           //!< Server object.
	const User* m_user;               //!< User object.
	Battle* m_battle;           //!< User object.

	wxStaticText* m_usercount_label;

	const ChatPanelType m_type;       //!< Channel object.

	wxString m_chan_pass;

	ChatPanelMenu* m_popup_menu;

	ChatLog m_chat_log;

	void LogTime();
	void CreateControls( );
	void CreatePopup();
	void LoadLastLines();

	size_t m_icon_index;

	wxImageList* m_imagelist;

	static const int m_groupMenu_baseID = 6798;
	TextCompletionDatabase textcompletiondatabase;

	std::vector<ChatLine> m_buffer;
	bool m_disable_append;

	wxString m_url_at_pos; //! the mouse event sink sets this

	friend class ChatPanelMenu; //menu needs access to members

	//used to avoid marking channel as changed when it's just been created.
	bool m_topic_set;

	DECLARE_EVENT_TABLE()
};

enum
{
	CHAT_SEND = wxID_HIGHEST,
	CHAT_TEXT,
	CHAT_LOG,
	CHAT_CHAN_OPTS
};
#endif // SPRINGLOBBY_HEADERGUARD_CHATPANEL_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2011

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    SpringLobby is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

