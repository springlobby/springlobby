#ifndef SPRINGLOBBY_HEADERGUARD_UI_H
#define SPRINGLOBBY_HEADERGUARD_UI_H

class Server;
class TASServer;
class ConnectWindow;
class Spring;
class MainWindow;
class Channel;
class User;
class IBattle;
class Battle;
class SinglePlayerBattle;
class OfflineBattle;
class ChatPanel;
class ReconnectDialog;

//this removes the necessity to drag wx/event.h into almost every other file for a single type
//if it's too "hackish" for someone's taste, just include that header again and remove this (koshi)
#ifndef wxEventType
typedef int wxEventType;
#endif

typedef int AlertEventType;

extern const wxEventType torrentSystemStatusUpdateEvt;

#include "utils/battleevents.h"
#include <wx/string.h>
#include <wx/timer.h>
#include "utils/mixins.hh"

//! @brief UI main class
class Ui : public wxEvtHandler, public GlobalEvent, public SL::NonCopyable
{
  public:

    Ui();
    ~Ui();

    enum PlaybackEnum {
        ReplayPlayback,
        SavegamePlayback
    };

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
	void AddServerWindow( const wxString& servername );
	void ReopenServerTab();

    void ConnectionFailurePrompt();
	wxString GetNextServer();

    bool DoRegister( const wxString& servername, const wxString& username, const wxString& password, wxString& reason );

	bool IsConnecting() const;
    bool IsConnected() const;
    void JoinChannel( const wxString& name, const wxString& password );

    bool IsSpringCompatible(const wxString& engine, const wxString& version);

    bool IsSpringRunning() const;

    void Quit();

	void Download( const wxString& category, const wxString& name, const wxString& hash );

	bool Ask( const wxString& heading, const wxString& question ) const;
	bool AskText( const wxString& heading, const wxString& question, wxString& answer, bool multiline = false);
	bool AskPassword( const wxString& heading, const wxString& message, wxString& password );
	void ShowMessage( const wxString& heading, const wxString& message ) const;
    //void OnAlertEvent( AlertEventType ); //TODO alert system

    MainWindow& mw();
	const MainWindow& mw() const;

    bool IsMainWindowCreated() const;

    void OnUpdate( int mselapsed );

    void OnConnected( Server& server, const wxString& server_name, const wxString& server_ver, bool supported );
    void OnLoggedIn( );
    void OnDisconnected( Server& server, bool wasonline );

    void OnJoinedChannelSuccessful( Channel& chan );
	void OnJoinedChannelSuccessful( Channel& chan, bool focusTab );
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
	void OnUserSaidEx( User& user, const wxString& action, bool me = false );

    void OnUnknownCommand( Server& server, const wxString& command, const wxString& params );
    void OnMotd( Server& server, const wxString& message );
	void OnServerBroadcast( Server& server, const wxString& message );
    void OnServerMessage( Server& server, const wxString& message );

    void OnBattleOpened( IBattle& battle );
    void OnBattleClosed( IBattle& battle );
    void OnUserJoinedBattle( IBattle& battle, User& user );
    void OnUserLeftBattle( IBattle& battle, User& user, bool isbot );
	void OnBattleInfoUpdated( BattleEvents::BattleEventData data );
//    void OnBattleInfoUpdated( IBattle& battle, const wxString& Tag );
    void OnBattleStarted( Battle& battle );

    void OnJoinedBattle( Battle& battle );
    void OnHostedBattle( Battle& battle );
    void OnUserBattleStatus( IBattle& battle, User& user );
    void OnRequestBattleStatus( IBattle& battle );

    void OnSaidBattle( IBattle& battle, const wxString& nick, const wxString& msg );
//    void OnBattleAction( IBattle& battle, const wxString& nick, const wxString& msg );

    void OnSpringTerminated( wxCommandEvent& data);

    void OnAcceptAgreement( const wxString& agreement );

    void OnMainWindowDestruct();

    void OnRing( const wxString& from );
	void OnQuit(wxCommandEvent& data);

    //! ask to download missing map, return true if download attempted
    bool OnPresetRequiringMap( const wxString& mapname );

    bool IsThisMe(User& other) const;
    bool IsThisMe(User* other) const;
    bool IsThisMe(const wxString& other) const;

    int TestHostPort( unsigned int port ) const;

    void ReloadPresetList();

    void OpenFileInEditor( const wxString& filepath );

    //! the welcome box, should be called in all code paths directly after MainWindow might be shown for the first time
    void FirstRunWelcome();
    void CheckForUpdates();

  protected:
    Server* m_serv;
    MainWindow* m_main_win;
    ConnectWindow* m_con_win;
	wxTimer m_reconnect_delay_timer;
	ReconnectDialog* m_reconnect_dialog;

    wxString m_last_used_backup_server;

    unsigned int m_upd_counter_torrent;

    bool m_first_update_trigger;

	bool m_recconecting_wait;
	bool m_disable_autoconnect;

	EventReceiverFunc<Ui, BattleEvents::BattleEventData, &Ui::OnBattleInfoUpdated>
		m_battle_info_updatedSink;
};

Ui& ui();

#endif // SPRINGLOBBY_HEADERGUARD_UI_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2011

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    SpringLobby is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

