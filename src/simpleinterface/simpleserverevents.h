#ifndef SPRINGLOBBY_HEADERGUARD_SIMPLESERVEREVENTS_H
#define SPRINGLOBBY_HEADERGUARD_SIMPLESERVEREVENTS_H

#include "../iserverevents.h"
#include <wx/event.h>
#include <wx/longlong.h>

class Ui;
struct UserStatus;
struct UserBattleStatus;
class Battle;

//! @brief Class that implements server event behaviour.
class SimpleServerEvents : public IServerEvents, public wxEvtHandler
{
  public:
	SimpleServerEvents( Server& serv) : m_serv(serv), m_autolaunch(false), m_autoclose(false) {}
	~SimpleServerEvents() {}

  // Uicontrol interface

	void OnConnected( const wxString& server_name, const wxString& server_ver, bool supported, const wxString& server_spring_ver, bool lanmode );
	void OnDisconnected( bool wasonline );

	void OnLogin();
	void OnLoginInfoComplete();
	void OnLogout();

	void OnUnknownCommand( const wxString& command, const wxString& params );
	void OnSocketError( const Sockerror& error );
	void OnProtocolError( const Protocolerror error );
	void OnMotd( const wxString& msg );
	void OnPong( wxLongLong ping_time );

	void OnNewUser( const wxString& nick, const wxString& conutry, int cpu, const wxString& id );
	void OnUserStatus( const wxString& nick, UserStatus status );
	void OnUserQuit( const wxString& nick );

	void OnBattleOpened( int id, BattleType type, NatType nat, const wxString& nick,
						 const wxString& host, int port, int maxplayers,
						 bool haspass, int rank, const wxString& maphash, const wxString& map,
						 const wxString& title, const wxString& mod );

	void OnUserJoinedBattle( int battleid, const wxString& nick, const wxString& userScriptPassword );
	void OnUserLeftBattle( int battleid, const wxString& nick );
	void OnBattleInfoUpdated( int battleid, int spectators, bool locked, const wxString& maphash, const wxString& map );
	void OnSetBattleInfo( int battleid, const wxString& param, const wxString& value );
	void OnBattleInfoUpdated( int battleid );
	void OnBattleClosed( int battleid );

	void OnJoinedBattle( int battleid, const wxString& hash );
	void OnHostedBattle( int battleid );

	void OnStartHostedBattle( int battleid );
	void OnClientBattleStatus( int battleid, const wxString& nick, UserBattleStatus status );

	void OnBattleStartRectAdd( int battleid, int allyno, int left, int top, int right, int bottom );
	void OnBattleStartRectRemove( int battleid, int allyno );

	void OnBattleAddBot( int battleid, const wxString& nick, UserBattleStatus status );
	void OnBattleUpdateBot( int battleid, const wxString& nick, UserBattleStatus status );
	void OnBattleRemoveBot( int battleid, const wxString& nick );

	void OnBattleDisableUnit( int battleid, const wxString& unitname, int count = 0 );
	void OnBattleEnableUnit( int battleid, const wxString& unitname );
	void OnBattleEnableAllUnits( int battleid );

	void OnJoinChannelResult( bool success, const wxString& channel, const wxString& reason );

	void OnChannelSaid( const wxString& channel, const wxString& who, const wxString& message );
	void OnChannelJoin( const wxString& channel, const wxString& who );
	void OnChannelPart( const wxString& channel, const wxString& who, const wxString& message );
	void OnChannelTopic( const wxString& channel, const wxString& who, const wxString& message, int when );
	void OnChannelAction( const wxString& channel, const wxString& who, const wxString& action );
	void OnChannelList( const wxString& channel, const int& numusers, const wxString& topic );
	void OnUserJoinChannel( const wxString& channel, const wxString& who );

	void OnPrivateMessage( const wxString& user, const wxString& message, bool fromme = false );
	void OnPrivateMessageEx( const wxString& user, const wxString& action, bool fromme = false );

	void OnRequestBattleStatus( int battleid );
	void OnSaidBattle( int battleid, const wxString& nick, const wxString& msg );
	void OnBattleAction( int battleid, const wxString& nick, const wxString& msg );

	void OnAcceptAgreement( const wxString& agreement );

	void OnRing( const wxString& from );

	void OnServerMessage( const wxString& message );
	void OnServerBroadcast( const wxString& message );
	void OnServerMessageBox( const wxString& message );
	void OnChannelMessage( const wxString& channel, const wxString& msg );

	void OnHostExternalUdpPort( const unsigned int udpport );

	void OnMyExternalUdpSourcePort( const unsigned int udpport );
	void OnMyInternalUdpSourcePort( const unsigned int udpport );

	void OnClientIPPort( const wxString &username, const wxString &ip, unsigned int udpport );

	void OnKickedFromBattle();

	void OnRedirect( const wxString& address,  unsigned int port, const wxString& CurrentNick, const wxString& CurrentPassword );

	/// use this function to check spam from clients and autokick from the battle
	void AutoCheckCommandSpam( Battle& battle, User& nick );

	void OnMutelistBegin( const wxString& channel );
	void OnMutelistItem( const wxString& channel, const wxString& mutee, const wxString& description );
	void OnMutelistEnd( const wxString& channel );

	void OnScriptStart( int battleid );
	void OnScriptLine( int battleid, const wxString& line );
	void OnScriptEnd( int battleid );

	void OnFileDownload( bool autolaunch, bool autoclose, bool disconnectonrefuse, const wxString& FileName, const wxString& url, const wxString& description );
	void OnSpringDownloadEvent( wxCommandEvent& event );

  protected:
	Server& m_serv;
	std::map<wxString,MessageSpamCheck> m_spam_check;

	DECLARE_EVENT_TABLE()

		/// spring autoupdate stuff
	bool m_autolaunch;
	bool m_autoclose;
	wxString m_savepath;
};

#endif // SPRINGLOBBY_HEADERGUARD_SIMPLESERVEREVENTS_H

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

