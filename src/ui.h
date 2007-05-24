//
// Class: Ui
//

#ifndef _UI_H_
#define _UI_H_

#include <wx/msgdlg.h>
#include "mainwindow.h"
#include "connectwindow.h"


struct UiChannelData {
  ChatPanel* panel;
};

struct UiUserData {
  ChatPanel* panel;  
};


//! @brief UI main class
class Ui
{
  public:
    Ui() { m_main_win = new MainWindow(); }
    virtual ~Ui() { m_main_win->Destroy(); }
  
    // Ui interface
  
    void ShowMainWindow();
    void ShowConnectWindow();
    void Connect();
  
    void Quit();
  
    bool Ask( const wxString& heading, const wxString& question );
  
    MainWindow& mw();
  
    void OnConnected( const std::string& server_name, const std::string& server_ver, bool supported );
  
    void OnJoinedChannelSuccessful( Channel& chan );
    void OnUserJoinedChannel( Channel& chan, User& user );
    void OnUserLeftChannel( Channel& chan, User& user, const std::string& reason );
    
    void OnChannelTopic( Channel& channel , const std::string user, const std::string& topic );
    void OnChannelSaid( Channel& channel , User& user, const std::string& message );
    void OnChannelDidAction( Channel& channel , User& user, const std::string& action );
    void OnLeaveChannel( Channel& channel );
  
    void OnUserOnline( User& user );
    void OnUserOffline( User& user );
    void OnUserStatusChanged( User& user );
    void OnUserSaid( User& user, const std::string message );

  protected:
    // Ui variables
  
    MainWindow* m_main_win;
    ConnectWindow* m_con_win;
};

Ui& ui();


#endif  //_UI_H_

