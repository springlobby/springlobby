#ifndef SPRINGLOBBY_HEADERGUARD_MAINCHATTAB_H
#define SPRINGLOBBY_HEADERGUARD_MAINCHATTAB_H

#include <wx/panel.h>
#include <wx/string.h>

class Ui;
class ChatPanel;
class Server;
class Channel;
class User;
class wxNotebookEvent;
class wxNotebook;
class wxBoxSizer;
class wxImageList;

//! @brief The main chat tab.
class MainChatTab : public wxPanel
{
  public:
    MainChatTab( wxWindow* parent, Ui& ui );
    ~MainChatTab();

    ChatPanel& ServerChat();

    ChatPanel* AddChatPannel( Channel& channel );
    ChatPanel* AddChatPannel( Server& server, const wxString& name );
    ChatPanel* AddChatPannel( User& user );

    void OnTabsChanged( wxNotebookEvent& event );

  protected:

    Ui& m_ui;

    wxWindow* m_close_window;
    wxNotebook* m_chat_tabs;
    wxBoxSizer* m_main_sizer;
    wxImageList* m_imagelist;
    ChatPanel* m_server_chat;
    ChatPanel* m_main_chat;
    int m_newtab_sel;

    DECLARE_EVENT_TABLE()
};

enum
{
    CHAT_TABS = wxID_HIGHEST
};

#endif // SPRINGLOBBY_HEADERGUARD_MAINCHATTAB_H
