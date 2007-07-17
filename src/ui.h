//
// Class: Ui
//

#ifndef _UI_H_
#define _UI_H_

#include <string>

class Server;
class TASServer;
class ConnectWindow;
class Spring;
class MainWindow;
class wxString;
class Channel;
class User;
class Battle;

//! @brief UI main class
class Ui
{
  public:

    Ui();
    ~Ui();

    Server& GetServer();

    bool ExecuteSayCommand( const wxString& cmd );

    void ShowMainWindow();
    void ShowConnectWindow();
    void Connect();
    void Disconnect();
    void DoConnect( const wxString& servername, const wxString& username, const wxString& password );

    bool IsConnected() const;
    void JoinChannel( const wxString& name, const wxString& password );

    void Quit();

    static bool Ask( const wxString& heading, const wxString& question );
    static bool AskText( const wxString& heading, const wxString& question, wxString& answer );
    static void ShowMessage( const wxString& heading, const wxString& message );
    
    MainWindow& mw();
  
    void OnUpdate();
    
    void OnConnected( Server& server, const std::string& server_name, const std::string& server_ver, bool supported );
    void OnDisconnected( Server& server );

    void OnJoinedChannelSuccessful( Channel& chan );
    void OnUserJoinedChannel( Channel& chan, User& user );
    void OnUserLeftChannel( Channel& chan, User& user, const std::string& reason );
    
    void OnChannelTopic( Channel& channel , const std::string user, const std::string& topic );
    void OnChannelSaid( Channel& channel , User& user, const std::string& message );
    void OnChannelDidAction( Channel& channel , User& user, const std::string& action );
    void OnLeaveChannel( Channel& channel );
    void OnChannelList( const std::string& channel, const int& numusers );
    void OnUserOnline( User& user );
    void OnUserOffline( User& user );
    void OnUserStatusChanged( User& user );
    void OnUserSaid( User& user, const std::string message, bool me = false );

    void OnUnknownCommand( Server& server, const std::string& command, const std::string& params );
    void OnMotd( Server& server, const std::string& message );
  
    void OnBattleOpened( Battle& battle );
    void OnBattleClosed( Battle& battle );
    void OnUserJoinedBattle( Battle& battle, User& user );
    void OnUserLeftBattle( Battle& battle, User& user );
    void OnBattleInfoUpdated( Battle& battle );
    void OnBattleStarted( Battle& battle );
    void OnBattleStartRectsUpdated( Battle& battle );

    void OnBattleDisableUnit( Battle& battle, const std::string& unitname );
    void OnBattleEnableUnit( Battle& battle, const std::string& unitname );
    void OnBattleEnableAllUnits( Battle& battle );

    void OnJoinedBattle( Battle& battle );
    void OnUserBattleStatus( Battle& battle, User& user );
    void OnRequestBattleStatus( Battle& battle );
    
    void OnSaidBattle( Battle& battle, const std::string& nick, const std::string& msg );
    void OnBattleAction( Battle& battle, const std::string& nick, const std::string& msg );
    
    void OnSpringTerminated( bool success );

    void OnMainWindowDestruct() { m_main_win = 0; }

  protected:
    // Ui variables
  
    Spring* m_spring;

    Server* m_serv;
    MainWindow* m_main_win;
    ConnectWindow* m_con_win;
};


#endif  //_UI_H_

