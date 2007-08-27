#ifndef SPRINGLOBBY_HEADERGUARD_UI_H
#define SPRINGLOBBY_HEADERGUARD_UI_H

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
class BattleBot;

typedef int HostInfo;

typedef int AlertEventType;

#define AE_MESSAGE 1
#define AE_HIGHLIGHT_MESSAGE 2
#define AE_PM 4
#define AE_ERROR 8
#define AE_DISCONNECT 16
#define AE_BATTLE_MESSAGE 32


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

    bool DoRegister( const wxString& servername, const wxString& username, const wxString& password );

    bool IsConnected() const;
    void JoinChannel( const wxString& name, const wxString& password );

    void SetSupportedSpring(std::string spring_version) { m_server_spring_ver = spring_version; }

    bool IsSpringCompatible( );

    void StartHostedBattle();
    //void SendHostInfo( HostInfo update );

    void Quit();

    static bool Ask( const wxString& heading, const wxString& question );
    static bool AskText( const wxString& heading, const wxString& question, wxString& answer );
    static void ShowMessage( const wxString& heading, const wxString& message );

    //void OnAlertEvent( AlertEventType ); //TODO alert system

    MainWindow& mw();

    void OnUpdate();

    void OnConnected( Server& server, const std::string& server_name, const std::string& server_ver, bool supported);
    void OnLoggedIn( );
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

    void OnBattleBotAdded( Battle& battle, BattleBot& bot );
    void OnBattleBotRemoved( Battle& battle, BattleBot& bot );
    void OnBattleBotUpdated( Battle& battle, BattleBot& bot );

    void OnBattleDisableUnit( Battle& battle, const std::string& unitname );
    void OnBattleEnableUnit( Battle& battle, const std::string& unitname );
    void OnBattleEnableAllUnits( Battle& battle );

    void OnJoinedBattle( Battle& battle );
    void OnHostedBattle( Battle& battle );
    void OnUserBattleStatus( Battle& battle, User& user );
    void OnRequestBattleStatus( Battle& battle );

    void OnSaidBattle( Battle& battle, const std::string& nick, const std::string& msg );
    void OnBattleAction( Battle& battle, const std::string& nick, const std::string& msg );

    void OnSpringTerminated( bool success );

    void OnAcceptAgreement( const std::string& agreement );

    void OnMainWindowDestruct() { m_main_win = 0; }

    void OnRing( const std::string& from );

  protected:
    Spring* m_spring;

    std::string m_server_spring_ver;

    Server* m_serv;
    MainWindow* m_main_win;
    ConnectWindow* m_con_win;
};

#endif // SPRINGLOBBY_HEADERGUARD_UI_H
