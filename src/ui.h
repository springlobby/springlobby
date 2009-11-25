#ifndef SPRINGLOBBY_HEADERGUARD_UI_H
#define SPRINGLOBBY_HEADERGUARD_UI_H

class Server;
class TASServer;
class ConnectWindow;
class Spring;
class MainWindow;
class wxString;
class Channel;
class User;
class wxTimer;
class wxTimerEvent;
class IBattle;
class Battle;
class SinglePlayerBattle;
class OfflineBattle;
class ChatPanel;

#include <wx/event.h>

typedef int AlertEventType;

extern const wxEventType torrentSystemStatusUpdateEvt;


//! @brief UI main class
class Ui : public wxEvtHandler
{
  public:

    Ui();
    ~Ui();

    enum PlaybackEnum {
        ReplayPlayback,
        SavegamePlayback
    };

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

    void ConnectionFailurePrompt();
    void SwitchToNextServer();

    bool DoRegister( const wxString& servername, const wxString& username, const wxString& password, wxString& reason );

    bool IsConnected() const;
    void JoinChannel( const wxString& name, const wxString& password );

    bool IsSpringCompatible();

    bool IsSpringRunning();

    void Quit();

    void DownloadMap( const wxString& hash, const wxString& name );
    void DownloadMod( const wxString& hash, const wxString& name );

    bool Ask( const wxString& heading, const wxString& question );
    bool AskText( const wxString& heading, const wxString& question, wxString& answer, long style = wxOK | wxCANCEL | wxCENTRE );
    bool AskPassword( const wxString& heading, const wxString& message, wxString& password );
    void ShowMessage( const wxString& heading, const wxString& message );
    //void OnAlertEvent( AlertEventType ); //TODO alert system

    MainWindow& mw();

    bool IsMainWindowCreated();

    void OnUpdate( wxTimerEvent& event );

    void OnConnected( Server& server, const wxString& server_name, const wxString& server_ver, bool supported );
    void OnLoggedIn( );
    void OnDisconnected( Server& server, bool wasonline );

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

    void OnBattleOpened( IBattle& battle );
    void OnBattleClosed( IBattle& battle );
    void OnUserJoinedBattle( IBattle& battle, User& user );
    void OnUserLeftBattle( IBattle& battle, User& user );
    void OnBattleInfoUpdated( IBattle& battle );
    void OnBattleInfoUpdated( IBattle& battle, const wxString& Tag );
    void OnBattleStarted( Battle& battle );

    void OnJoinedBattle( Battle& battle );
    void OnHostedBattle( Battle& battle );
    void OnUserBattleStatus( IBattle& battle, User& user );
    void OnRequestBattleStatus( IBattle& battle );

    void OnSaidBattle( IBattle& battle, const wxString& nick, const wxString& msg );
    void OnBattleAction( IBattle& battle, const wxString& nick, const wxString& msg );

    void OnSpringStarting();
    void OnSpringTerminated( long exit_code );

    void OnAcceptAgreement( const wxString& agreement );

    void OnMainWindowDestruct();

    void OnRing( const wxString& from );

    //! ask to download missing map, return true if download attempted
    bool OnPresetRequiringMap( const wxString& mapname );

    bool IsThisMe(User& other);
    bool IsThisMe(User* other);
    bool IsThisMe(const wxString& other);

    int TestHostPort( unsigned int port );

    void ReloadPresetList();

    void OpenFileInEditor( const wxString& filepath );

    //! the welcome box, should be called in all code paths directly after MainWindow might be shown for the first time
    void FirstRunWelcome();

    void StartUpdateTimer();


  protected:
    Server* m_serv;
    MainWindow* m_main_win;
    ConnectWindow* m_con_win;

    wxString m_last_used_backup_server;

    unsigned int m_upd_counter_torrent;

    bool m_first_update_trigger;

    bool m_ingame;
    wxTimer* m_timer;

    //! does actual work, called from downloadmap/mod
    void DownloadFileP2P( const wxString& hash, const wxString& name );

    private:
        Ui( const Ui& );

    DECLARE_EVENT_TABLE()

};

Ui& ui();


#endif // SPRINGLOBBY_HEADERGUARD_UI_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-09

    springsettings is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

