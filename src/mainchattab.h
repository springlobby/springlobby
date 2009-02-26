#ifndef SPRINGLOBBY_HEADERGUARD_MAINCHATTAB_H
#define SPRINGLOBBY_HEADERGUARD_MAINCHATTAB_H

#include <wx/scrolwin.h>

class Ui;
class ChatPanel;
class Server;
class Channel;
class User;
class wxNotebookEvent;
class wxNotebook;
class wxAuiNotebook;
class wxBoxSizer;
class wxImageList;
class wxString;

//! @brief The main chat tab.
class MainChatTab : public wxScrolledWindow
{
  public:
    MainChatTab( wxWindow* parent, Ui& ui );
    ~MainChatTab();

    ChatPanel* GetActiveChatPanel();
    ChatPanel* GetChannelChatPanel( const wxString& channel );
    ChatPanel* GetUserChatPanel( const wxString& user );

    ChatPanel& ServerChat();

    ChatPanel* AddChatPannel( Channel& channel );
    ChatPanel* AddChatPannel( Server& server, const wxString& name );
    ChatPanel* AddChatPannel( const User& user );
    /** \brief this is only used if channel is left via raw command in server tab */
    bool RemoveChatPanel( ChatPanel* panel );

    void RejoinChannels();

    void OnTabsChanged( wxAuiNotebookEvent& event );
    void OnTabClose( wxAuiNotebookEvent& event );
    void OnUserConnected( User& user );
    void OnUserDisconnected( User& user );

    void ChangeUnreadChannelColour( const wxColour& colour );
    void ChangeUnreadPMColour( const wxColour& colour );

    void UpdateNicklistHighlights();
    void Update();

    wxImage ReplaceChannelStatusColour( wxBitmap img, const wxColour& colour );

  protected:

    Ui& m_ui;

    wxWindow* m_close_window;
    wxAuiNotebook* m_chat_tabs;
    wxBoxSizer* m_main_sizer;
    wxImageList* m_imagelist;
    ChatPanel* m_server_chat;
    ChatPanel* m_main_chat;
    int m_newtab_sel;

    enum {
        CHAT_TABS = wxID_HIGHEST
    };

    DECLARE_EVENT_TABLE()
};



#endif // SPRINGLOBBY_HEADERGUARD_MAINCHATTAB_H
