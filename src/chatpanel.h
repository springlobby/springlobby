#ifndef SPRINGLOBBY_HEADERGUARD_CHATPANEL_H
#define SPRINGLOBBY_HEADERGUARD_CHATPANEL_H

#include <iostream>

#include <wx/panel.h>
#include <wx/event.h>
#include <wx/string.h>
#include <wx/menu.h>

#include "chatlog.h"
#include "usermenu.h"
#include "Helper/TextCompletionDatabase.hpp"

class wxCommandEvent;
class wxSizeEvent;
class wxBoxSizer;
class wxSplitterWindow;
class wxTextCtrl;
class wxTextCtrlHist;
class wxRichTextCtrl;
class wxTextUrlEvent;
class wxComboBox;
class wxButton;
class wxNotebook;
class NickListCtrl;
class Channel;
class User;
class Server;
class Battle;
class Ui;

class wxFocusEvent;
class wxMouseEvent;

enum ChatPanelType {
  CPT_Channel,
  CPT_Server,
  CPT_User,
  CPT_Battle
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

    ChatPanel( wxWindow* parent, Ui& ui, Channel& chan );
    ChatPanel( wxWindow* parent, Ui& ui, User& user );
    ChatPanel( wxWindow* parent, Ui& ui, Server& serv );
    ChatPanel( wxWindow* parent, Ui& ui, Battle& battle );
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

    Channel* GetChannel();
    void SetChannel( Channel* chan );

    Server* GetServer();
    void SetServer( Server* serv );

    User* GetUser();
    void SetUser( User* usr );

    bool IsServerPanel();
    ChatPanelType GetPanelType();

    void Say( const wxString& message );
    void Part();
    void FocusInputBox();

    wxString GetChatTypeStr();

    User& GetMe();
    User* GetSelectedUser();

    bool IsOk();

    void CheckLength();

    void OnUserDisconnected();
    void OnUserConnected();

    void OnSay( wxCommandEvent& event );
    void OnResize( wxSizeEvent& event );

    void OnLinkEvent( wxTextUrlEvent& event );
    void OnMouseDown( wxMouseEvent& event );

    void OnMenuSelectAll( wxCommandEvent& event );
    void OnMenuCopy( wxCommandEvent& event );
    void OnChannelMenuClear ( wxCommandEvent& event );

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

	void OnKeyPressed( wxKeyEvent& keyevent );
	void OnKeyReleased( wxKeyEvent& keyevent );

	void OnUserMenuAddToGroup( wxCommandEvent& event );
	void OnUserMenuDeleteFromGroup( wxCommandEvent& event );
	void OnUserMenuCreateGroup( wxCommandEvent& event );
	void UpdateNicklistHighlights();

  protected:
    void _SetChannel( Channel* channel );
    void OutputLine( const wxString& message, const wxColour& col, const wxFont& fon );

    bool ContainsWordToHighlight( const wxString& message );

    bool m_show_nick_list;      //!< If the nicklist should be shown or not.

    wxBoxSizer* m_main_sizer;   //!< Main sizer containing all other sizers.
    wxBoxSizer* m_chat_sizer;   //!< Sizer containing the chat messages, and send input and button.
    wxBoxSizer* m_say_sizer;    //!< Sizer containing send input and button.
    wxBoxSizer* m_nick_sizer;   //!< Sizer containing the nicklist.

    wxSplitterWindow* m_splitter; //!< The splitter.
    wxPanel* m_chat_panel;      //!< Panel containing the chat. Only used when nicklist is visible.
    wxPanel* m_nick_panel;      //!< Panel containing the nicklist.

    #ifndef NO_RICHTEXT_CHAT
    wxRichTextCtrl* m_chatlog_text; //!< The chat log textcontrol.
    #else
    wxTextCtrl* m_chatlog_text; //!< The chat log textcontrol.
    #endif
    wxTextCtrlHist* m_say_text;     //!< The say textcontrol.

    NickListCtrl* m_nicklist;   //!< The nicklist.
    wxComboBox* m_nick_filter;  //!< The filter combo.

    wxButton* m_say_button;     //!< The say button.
    wxNotebook* m_chat_tabs;
    Ui& m_ui;
    Channel* m_channel;         //!< Channel object.
    Server* m_server;           //!< Server object.
    User* m_user;               //!< User object.
    Battle* m_battle;           //!< User object.

    ChatPanelType m_type;       //!< Channel object.

    wxString m_chan_pass;


    wxMenu* m_popup_menu;
    wxMenuItem* m_autorejoin;
    ChatLog* m_chat_log;
    wxMenuItem* displayjoinitem;
    SL_GENERIC::UserMenu<ChatPanel>* m_usermenu;

    void LogTime();
    void CreateControls( );
    void CreatePopup();
    SL_GENERIC::UserMenu<ChatPanel>* CreateNickListMenu();

    static const int m_groupMenu_baseID = 6798;
	TextCompletionDatabase textcompletiondatabase;

    DECLARE_EVENT_TABLE();
};

enum
{
    CHAT_SEND = wxID_HIGHEST,
    CHAT_TEXT,
    CHAT_LOG,

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
    CHAT_MENU_US_MODERATOR_RING
};


#endif // SPRINGLOBBY_HEADERGUARD_CHATPANEL_H
