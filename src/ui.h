#ifndef SPRINGLOBBY_HEADERGUARD_UI_H
#define SPRINGLOBBY_HEADERGUARD_UI_H

#include <wx/string.h>
#include <wx/thread.h>
#include <wx/event.h>
#include <map>
#include "useractions.hh"

class Server;
class TASServer;
class ConnectWindow;
class Spring;
class MainWindow;
class wxString;
class Channel;
class User;
class Battle;
class SinglePlayerBattle;
struct BattleBot;
class ChatPanel;
class UnitSyncThread;


typedef int HostInfo;

typedef int AlertEventType;

extern const wxEventType torrentSystemStatusUpdateEvt;

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
    bool    GetServerStatus();
    ChatPanel* GetActiveChatPanel();
    ChatPanel* GetChannelChatPanel( const wxString& channel );

    bool ExecuteSayCommand( const wxString& cmd );
    void ConsoleHelp( const wxString& topic );

    void ShowMainWindow();
    void ShowConnectWindow();
    void Connect();
    void Disconnect();
    void Reconnect();
    void DoConnect( const wxString& servername, const wxString& username, const wxString& password );

    bool DoRegister( const wxString& servername, const wxString& username, const wxString& password, wxString& reason );

    bool IsConnected() const;
    void JoinChannel( const wxString& name, const wxString& password );

    bool IsSpringCompatible();

    bool IsSpringRunning();

    void StartHostedBattle();
    void StartSinglePlayerGame( SinglePlayerBattle& battle );
    //void SendHostInfo( HostInfo update );

    void Quit();

    void ReloadUnitSync();

    void DownloadMap( const wxString& hash, const wxString& name );
    void DownloadMod( const wxString& hash, const wxString& name );

    void OpenWebBrowser( const wxString& url );

    bool Ask( const wxString& heading, const wxString& question );
    bool AskText( const wxString& heading, const wxString& question, wxString& answer, long style = wxOK | wxCANCEL | wxCENTRE );
    bool AskPassword( const wxString& heading, const wxString& message, wxString& password );
    void ShowMessage( const wxString& heading, const wxString& message );
    //void OnAlertEvent( AlertEventType ); //TODO alert system

    MainWindow& mw();

    bool IsMainWindowCreated();

    void OnUpdate( int mselapsed );

    void OnConnected( Server& server, const wxString& server_name, const wxString& server_ver, bool supported );
    void OnLoggedIn( );
    void OnDisconnected( Server& server );

    void OnJoinedChannelSuccessful( Channel& chan );
    void OnUserJoinedChannel( Channel& chan, User& user );
    void OnChannelJoin( Channel& chan, User& user );
    void OnUserLeftChannel( Channel& chan, User& user, const wxString& reason );

    void OnChannelTopic( Channel& channel , const wxString& user, const wxString& topic );
    void OnChannelSaid( Channel& channel , User& user, const wxString& message );
    void OnChannelDidAction( Channel& channel , User& user, const wxString& action );
    void OnChannelMessage( const wxString& channel, const wxString& msg );

    void OnLeaveChannel( wxString& name  );
    void OnChannelList( const wxString& channel, const int& numusers );
    void OnUserOnline( User& user );
    void OnUserOffline( User& user );
    void OnUserStatusChanged( User& user );
    void OnUserSaid( User& user, const wxString& message, bool me = false );

    void OnUnknownCommand( Server& server, const wxString& command, const wxString& params );
    void OnMotd( Server& server, const wxString& message );
    void OnServerMessage( Server& server, const wxString& message );

    void OnBattleOpened( Battle& battle );
    void OnBattleClosed( Battle& battle );
    void OnUserJoinedBattle( Battle& battle, User& user );
    void OnUserLeftBattle( Battle& battle, User& user );
    void OnBattleInfoUpdated( Battle& battle );
    void OnBattleInfoUpdated( Battle& battle, const wxString& Tag );
    void OnBattleStarted( Battle& battle );
    void OnBattleStartRectsUpdated( Battle& battle );
    void OnBattleMapChanged( Battle& battle );
    void OnBattleMapRefresh();

    void OnBattleBotAdded( Battle& battle, BattleBot& bot );
    void OnBattleBotRemoved( Battle& battle, BattleBot& bot );
    void OnBattleBotUpdated( Battle& battle, BattleBot& bot );

    void OnBattleDisableUnit( Battle& battle, const wxString& unitname );
    void OnBattleEnableUnit( Battle& battle, const wxString& unitname );
    void OnBattleEnableAllUnits( Battle& battle );

    void OnJoinedBattle( Battle& battle );
    void OnHostedBattle( Battle& battle );
    void OnUserBattleStatus( Battle& battle, User& user );
    void OnRequestBattleStatus( Battle& battle );

    void OnSaidBattle( Battle& battle, const wxString& nick, const wxString& msg );
    void OnBattleAction( Battle& battle, const wxString& nick, const wxString& msg );

    void OnSpringTerminated( bool success );

    void OnAcceptAgreement( const wxString& agreement );

    void OnMainWindowDestruct();

    void OnRing( const wxString& from );

    void OnMapInfoCached( const wxString& mapname );
    void OnMinimapCached( const wxString& mapname );
    void OnModUnitsCached( const wxString& modname );
    void OnCachedThreadTerminated();
    void OnCachedThreadStarted();

    bool IsThisMe(User& other);

    int TestHostPort( unsigned int port );

  protected:
    Spring* m_spring;

    UnitSyncThread* m_thread;
    wxCriticalSection m_thread_wait;

    Server* m_serv;
    MainWindow* m_main_win;
    ConnectWindow* m_con_win;

    unsigned int m_upd_intv_counter;

};

Ui& ui();


#endif // SPRINGLOBBY_HEADERGUARD_UI_H
