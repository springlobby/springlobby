#ifndef SPRINGLOBBY_HEADERGUARD_CHATPANEL_H
#define SPRINGLOBBY_HEADERGUARD_CHATPANEL_H

#include <iostream>

#include <wx/panel.h>
#include <wx/event.h>
#include <wx/textctrl.h>
#include <wx/string.h>

#include "chatlog.h"
#include "Helper/TextCompletionDatabase.hpp"

class wxCommandEvent;
class wxSizeEvent;
class wxBoxSizer;
class wxSplitterWindow;
class wxTextCtrl;
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
class wxMenu;
class wxMouseEvent;

enum ChatPanelType {
  CPT_Channel,
  CPT_Server,
  CPT_User,
  CPT_Battle
};

/// This class implements GetDirty for autoscroll
/// Necessary to fix scrolling bug, when scrolling stops
/// if several lines are added between ui updates
/// GetDirty allows to detect this situation.
class MyTextCtrl: public wxTextCtrl{
  bool my_m_dirty;
  bool m_must_scroll;
  public:
  MyTextCtrl(wxWindow* parent, wxWindowID id, const wxString& value = _T(""), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = wxTextCtrlNameStr);
  void OnPaint(wxPaintEvent& event);
  void OnUpdateUI(wxUpdateUIEvent &event);
  void WriteText(const wxString&  text);
  bool GetDirty();
  void MakeDirty();
  bool GetMustScroll(){
    return m_must_scroll;
  }
  void SetMustScroll(bool v){
    m_must_scroll=v;
  }
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

    Channel& GetChannel();
    void SetChannel( Channel* chan );

    Server* GetServer();
    void SetServer( Server* serv );

    User* GetUser();
    void SetUser( User* usr );

    bool IsServerPanel();
    ChatPanelType GetPanelType();

    void Say( const wxString& message );
    void Part();

    wxString GetChatTypeStr();

    User& GetMe();

    bool IsOk();

    void CheckLength();

    void OnUserDisconnected();
    void OnUserConnected();

    void OnSay( wxCommandEvent& event );
    void OnResize( wxSizeEvent& event );
	void OnTextChanged_Say_Text( wxCommandEvent& event );

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


  protected:
    void _SetChannel( Channel* channel );
    void OutputLine( const wxString& message, const wxColour& col, const wxFont& fon );

    User* GetSelectedUser();

    bool m_show_nick_list;      //!< If the nicklist should be shown or not.

    wxBoxSizer* m_main_sizer;   //!< Main sizer containing all other sizers.
    wxBoxSizer* m_chat_sizer;   //!< Sizer containing the chat messages, and send input and button.
    wxBoxSizer* m_say_sizer;    //!< Sizer containing send input and button.
    wxBoxSizer* m_nick_sizer;   //!< Sizer containing the nicklist.

    wxSplitterWindow* m_splitter; //!< The splitter.
    wxPanel* m_chat_panel;      //!< Panel containing the chat. Only used when nicklist is visible.
    wxPanel* m_nick_panel;      //!< Panel containing the nicklist.

    MyTextCtrl* m_chatlog_text; //!< The chat log textcontrol.
    wxTextCtrl* m_say_text;     //!< The say textcontrol.

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

    void LogTime();
    void CreateControls( );
    void CreatePopup();
    wxMenu* CreateNickListMenu();


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
