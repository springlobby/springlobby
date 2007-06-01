//
// Class: Ui
//

#ifndef _UI_H_
#define _UI_H_

#include <wx/msgdlg.h>
#include <wx/textdlg.h>
#include "mainwindow.h"
#include "connectwindow.h"
#include "utils.h"
#include "server.h"

class TASServer;


//! @brief UI main class
class Ui
{
  public:
    Ui(): m_serv(NULL), m_con_win(NULL) { m_main_win = new MainWindow( *this ); }
    virtual ~Ui() { m_main_win->Destroy(); }
  
    // Ui interface
  
    void ShowMainWindow();
    void ShowConnectWindow();
    void Connect();
    void Disconnect() {}
    void DoConnect( const wxString& servername, const wxString& username, const wxString& password );
    
    bool IsConnected() const {
      if ( m_serv != NULL ) return m_serv->IsConnected();
      return false;
    }
    
    void JoinChannel( const wxString& name, const wxString& password ) {
      if ( m_serv != NULL ) m_serv->JoinChannel( STL_STRING(name), STL_STRING(password) );
    }
    
    void Quit();
  
    static bool Ask( const wxString& heading, const wxString& question );
    static bool AskText( const wxString& heading, const wxString& question, wxString& answer );
    static void ShowMessage( const wxString& heading, const wxString& message );
    
    MainWindow& mw();
  
    void OnUpdate();
    
    void OnConnected( Server& server, const std::string& server_name, const std::string& server_ver, bool supported );
  
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

    void OnUnknownCommand( Server& server, const std::string& command, const std::string& params );
    void OnMotd( Server& server, const std::string& message );
  
    void OnBattleOpened( Battle& battle );
    void OnBattleClosed( Battle& battle );
    void OnUserJoinedBattle( Battle& battle, User& user );
    void OnUserLeftBattle( Battle& battle, User& user );
    void OnBattleInfoUpdated( Battle& battle );
    
  protected:
    // Ui variables
  
    Server* m_serv;
    MainWindow* m_main_win;
    ConnectWindow* m_con_win;
};


#endif  //_UI_H_

