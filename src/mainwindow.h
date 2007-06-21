//
// Class: MainWindow
//

#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_


#include <wx/menu.h>
#include <wx/listbook.h>
#include <wx/frame.h>

#include "mainchattab.h"
#include "mainjoinbattletab.h"

class Ui;

  
//! @brief wxFrame that contains the main window of the client.
class MainWindow : public wxFrame
{
  public:
    MainWindow( Ui& ui );
    virtual ~MainWindow();

    // MainWindow interface
    void OpenChannelChat( Channel& channel );
    void OpenPrivateChat( User& user );

    void CloseAllChats();

    void OnMenuJoin( wxCommandEvent& event );
    void OnMenuConnect( wxCommandEvent& event );
    void OnMenuDisconnect( wxCommandEvent& event );
    void OnMenuQuit( wxCommandEvent& event );

    MainChatTab& GetChatTab();
    MainJoinBattleTab& GetJoinTab() { assert( m_join_tab != NULL ); return *m_join_tab; }

  protected:
    // MainWindow variables
  
    Ui& m_ui;
    
    wxBoxSizer* m_main_sizer;
    wxListbook* m_func_tabs;
    wxNotebookPage* m_chat_page;

    MainChatTab* m_chat_tab;
    MainJoinBattleTab* m_join_tab;

    wxImageList* m_func_tab_images;

    DECLARE_EVENT_TABLE()
};


//ChatPanel& servwin();

// wxWidget IDs
enum
{
    MENU_ABOUT = wxID_ABOUT,
    MENU_QUIT = wxID_EXIT,

    MENU_CONNECT = wxID_HIGHEST,
    MENU_DISCONNECT,
    MENU_JOIN

};

#endif  //_MAINWINDOW_H_

