#ifndef SPRINGLOBBY_HEADERGUARD_CHATPANEL_H
#define SPRINGLOBBY_HEADERGUARD_CHATPANEL_H

#include <wx/panel.h>
#include <wx/string.h>

class wxCommandEvent;
class wxSizeEvent;
class wxBoxSizer;
class wxSplitterWindow;
class wxTextCtrl;
class wxComboBox;
class wxButton;
class NickListCtrl;
class Channel;
class User;
class Server;
class Battle;
class Ui;

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
    //ChatPanel( wxWindow* parent, bool show_nick_list );
    ChatPanel( wxWindow* parent, Ui& ui, Channel& chan );
    ChatPanel( wxWindow* parent, Ui& ui, User& user );
    ChatPanel( wxWindow* parent, Ui& ui, Server& serv );
    ChatPanel( wxWindow* parent, Ui& ui, Battle& battle );
    ~ChatPanel();

    void Said( const wxString& who, const wxString& message );
    void DidAction( const wxString& who, const wxString& action );
    void Motd( const wxString& message );
    void StatusMessage( const wxString& message );

    void UnknownCommand( const wxString& command, const wxString& params );

    void Joined( User& who );
    void Parted( User& who, const wxString& message );
    void SetTopic( const wxString& who, const wxString& message );
    void UserStatusUpdated( User& who );

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

    void OnSay( wxCommandEvent& event );
    void OnResize( wxSizeEvent& event );

    wxString GetChatTypeStr();

    User& GetMe();

    bool IsOk();

  protected:
    void _SetChannel( Channel* channel );
    void _OutputLine( const wxString& message, const wxColour& col );

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

    Ui& m_ui;
    Channel* m_channel;         //!< Channel object.
    Server* m_server;           //!< Server object.
    User* m_user;               //!< User object.
    Battle* m_battle;           //!< User object.

    ChatPanelType m_type;       //!< Channel object.

    wxString m_chan_pass;

    void LogTime();
    void _CreateControls( );

    DECLARE_EVENT_TABLE();
};

enum
{
    CHAT_SEND = wxID_HIGHEST,
    CHAT_TEXT,
};

#endif // SPRINGLOBBY_HEADERGUARD_CHATPANEL_H
