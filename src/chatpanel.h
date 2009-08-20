#ifndef SPRINGLOBBY_HEADERGUARD_CHATPANEL_H
#define SPRINGLOBBY_HEADERGUARD_CHATPANEL_H

#include <wx/panel.h>
#include <wx/textctrl.h>
#include <vector>

#include "usermenu.h"
#include "chatlog.h"
#include "Helper/TextCompletionDatabase.hpp"

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
class ChatPanel : public wxPanel
{
  public:

    ChatPanel( wxWindow* parent, Channel& chan, wxImageList* imaglist );
    ChatPanel( wxWindow* parent, const User& user, wxImageList* imaglist  );
    ChatPanel( wxWindow* parent, Server& serv, wxImageList* imaglist  );
    ChatPanel( wxWindow* parent, Battle& battle );
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
    int GetPanelType() const;

    void Say( const wxString& message );
    void Part();
    void FocusInputBox();

    wxString GetChatTypeStr()  const;

    size_t GetIconIndex()  const { return m_icon_index; }
    void SetIconIndex( size_t index ) { m_icon_index = index; }

    const User& GetMe()  const;
    const User* GetSelectedUser() const;

    bool IsOk() const;

    void OnUserDisconnected();
    void OnUserConnected();

    void OnChanOpts( wxCommandEvent& event );
    void OnSay( wxCommandEvent& event );
    void OnPaste( wxClipboardTextEvent& event );

    void OnLinkEvent( wxTextUrlEvent& event );
    void OnMouseDown( wxMouseEvent& event );

    void OnMenuToggleAppend( wxCommandEvent& event );

    void OnChannelMenuLeave( wxCommandEvent& event );
    void OnChannelMenuDisplayJoinLeave( wxCommandEvent& event );
    void OnChannelAutoJoin( wxCommandEvent& event );
    void OnChannelMenuInfo( wxCommandEvent& event );
    void OnChannelMenuTopic( wxCommandEvent& event );
    void OnChannelMenuMessage( wxCommandEvent& event );
    void OnChannelMenuLock( wxCommandEvent& event );
    void OnChannelMenuUnlock( wxCommandEvent& event );
    void OnChannelMenuRegister( wxCommandEvent& event );
    void OnChannelMenuUnregister( wxCommandEvent& event );
    void OnChannelMenuSpamOn( wxCommandEvent& event );
    void OnChannelMenuSpanOff( wxCommandEvent& event );
    void OnChannelMenuSpamIsOn( wxCommandEvent& event );
    void OnChannelMenuShowMutelist( wxCommandEvent& event );

    void OnServerMenuDisconnect( wxCommandEvent& event );
    void OnServerMenuReconnect( wxCommandEvent& event );
    void OnServerMenuRemove( wxCommandEvent& event );
    void OnServerMenuChangePassword( wxCommandEvent& event );
    void OnServerMenuSetAccess( wxCommandEvent& event );
    void OnServerMenuBroadcast( wxCommandEvent& event );

    void OnUserMenuOpenChat( wxCommandEvent& event );
    void OnUserMenuJoinSame( wxCommandEvent& event );
    void OnUserMenuSlap( wxCommandEvent& event );
    void OnUserMenuMute( wxCommandEvent& event );
    void OnUserMenuUnmute( wxCommandEvent& event );
    void OnUserMenuKick( wxCommandEvent& event );
    void OnUserMenuOp( wxCommandEvent& event );
    void OnUserMenuDeop( wxCommandEvent& event );
    void OnUserMenuModeratorIngame( wxCommandEvent& event );
    void OnUserMenuModeratorCurrentIP( wxCommandEvent& event );
    void OnUserMenuModeratorKick( wxCommandEvent& event );
    void OnUserMenuModeratorBan( wxCommandEvent& event );
    void OnUserMenuModeratorUnban( wxCommandEvent& event );
    void OnUserMenuModeratorMute( wxCommandEvent& event );
    void OnUserMenuModeratorMute5( wxCommandEvent& event );
    void OnUserMenuModeratorMute10( wxCommandEvent& event );
    void OnUserMenuModeratorMute30( wxCommandEvent& event );
    void OnUserMenuModeratorMute120( wxCommandEvent& event );
    void OnUserMenuModeratorMute1440( wxCommandEvent& event );
    void OnUserMenuModeratorUnmute( wxCommandEvent& event );
    void OnUserMenuModeratorRing( wxCommandEvent& event );

    void OnUserMenuCopyLink( wxCommandEvent& event );

    void OnChatMenuOpenLog( wxCommandEvent& event );

    void OnKeyPressed( wxKeyEvent& keyevent );
    void OnKeyReleased( wxKeyEvent& keyevent );

    void OnUserMenuAddToGroup( wxCommandEvent& event );
    void OnUserMenuDeleteFromGroup( wxCommandEvent& event );
    void OnUserMenuCreateGroup( wxCommandEvent& event );
    void UpdateNicklistHighlights();

    void SortNickList();

    void ClearContents( wxCommandEvent& event );

  protected:

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

    int m_type;       //!< Channel object.

    wxString m_chan_pass;

    wxMenu* m_popup_menu;
    wxMenuItem* m_autorejoin;
    wxMenuItem* m_append_menu;
    ChatLog m_chat_log;
    wxMenuItem* displayjoinitem;
    typedef SL_GENERIC::UserMenu<ChatPanel> UserMenu;
    UserMenu* m_usermenu;

    void LogTime();
    void CreateControls( );
    void CreatePopup();

    size_t m_icon_index;

    wxImageList* m_imagelist;

    UserMenu* CreateNickListMenu();


    static const int m_groupMenu_baseID = 6798;
	TextCompletionDatabase textcompletiondatabase;

    std::vector<ChatLine> m_buffer;
    bool m_disable_append;

    wxString m_url_at_pos; //! the mouse event sink sets this

    DECLARE_EVENT_TABLE();
};

enum
{
    CHAT_SEND = wxID_HIGHEST,
    CHAT_TEXT,
    CHAT_LOG,
    CHAT_CHAN_OPTS,

    CHAT_MENU_DISABLE_APPEND,

    CHAT_MENU_CH_CLEAR,
    CHAT_MENU_CH_LEAVE,
    CHAT_MENU_CH_DISPLAYJOIN,
    CHAT_MENU_CH_AUTOJOIN,
    CHAT_MENU_CH_INFO,
    CHAT_MENU_CH_TOPIC,
    CHAT_MENU_CH_MSG,
    CHAT_MENU_CH_LOCK,
    CHAT_MENU_CH_UNLOCK,
    CHAT_MENU_CH_REG,
    CHAT_MENU_CH_UNREG,
    CHAT_MENU_CH_SPAM_ON,
    CHAT_MENU_CH_SPAM_OFF,
    CHAT_MENU_CH_SPAM_ISON,

    CHAT_MENU_SV_DISCON,
    CHAT_MENU_SV_RECON,
    CHAT_MENU_SV_REMOVE,
    CHAT_MENU_SV_CHPWD,
    CHAT_MENU_SV_ACCESS,
    CHAT_MENU_SV_BROADCAST,

    CHAT_MENU_US_CHAT,
    CHAT_MENU_US_JOIN,
    CHAT_MENU_US_SLAP,
    CHAT_MENU_US_MUTE,
    CHAT_MENU_US_UNMUTE,
    CHAT_MENU_US_KICK,
    CHAT_MENU_US_ADD_TO_GROUP,
    CHAT_MENU_US_OP,
    CHAT_MENU_US_DEOP,
    CHAT_MENU_US_MODERATOR_INGAME,
    CHAT_MENU_US_MODERATOR_CURIP,
    CHAT_MENU_US_MODERATOR_KICK,
    CHAT_MENU_US_MODERATOR_BAN,
    CHAT_MENU_US_MODERATOR_UNBAN,
    CHAT_MENU_US_MODERATOR_MUTE,
    CHAT_MENU_US_MODERATOR_MUTE_5,
    CHAT_MENU_US_MODERATOR_MUTE_10,
    CHAT_MENU_US_MODERATOR_MUTE_30,
    CHAT_MENU_US_MODERATOR_MUTE_120,
    CHAT_MENU_US_MODERATOR_MUTE_1440,
    CHAT_MENU_US_MODERATOR_UNMUTE,
    CHAT_MENU_US_MODERATOR_RING,

    CHAT_MENU_COPYLINK,

    CHAT_MENU_SHOW_MUTELIST,

    CHAT_MENU_LOG_OPEN
};


#endif // SPRINGLOBBY_HEADERGUARD_CHATPANEL_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-09

    springsettings is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

