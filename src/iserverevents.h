#ifndef ISERVEREVENTS_H
#define ISERVEREVENTS_H

//almost only needed for NAtType enum def
#include "battle.h"

struct MessageSpamCheck
{
  time_t lastmessage;
  unsigned int count;
};

class Server;
class wxString;
struct UserStatus;

// FIXME this is defined elsewhere, should use a different kind of type so we could use forward decl
typedef int Sockerror;

typedef int Protocolerror;

class IServerEvents
{
  public:
	IServerEvents( ) {}
	virtual ~IServerEvents() {}
	enum ServerEventsMode {
		normal = 0,
		simple = 1
	};
	static IServerEvents* getInstance( Server& server, ServerEventsMode mode = normal );
  // Uicontrol interface

	virtual void OnConnected( const wxString& server_name, const wxString& server_ver, bool supported, const wxString& server_spring_ver, bool lanmode ) = 0;
	virtual void OnDisconnected( bool wasonline ) = 0;

	virtual void OnLogin() = 0;
	virtual void OnLoginInfoComplete() = 0;
	virtual void OnLogout() = 0;

	virtual void OnUnknownCommand( const wxString& command, const wxString& params ) = 0;
	virtual void OnSocketError( const Sockerror& error ) = 0;
	virtual void OnProtocolError( const Protocolerror error ) = 0;
	virtual void OnMotd( const wxString& msg ) = 0;
	virtual void OnPong( wxLongLong ping_time ) = 0;

	virtual void OnNewUser( const wxString& nick, const wxString& conutry, int cpu, const wxString& id ) = 0;
	virtual void OnUserStatus( const wxString& nick, UserStatus status ) = 0;
	virtual void OnUserQuit( const wxString& nick ) = 0;

	virtual void OnBattleOpened( int id, BattleType type, NatType nat, const wxString& nick,
						 const wxString& host, int port, int maxplayers,
						 bool haspass, int rank, const wxString& maphash, const wxString& map,
						 const wxString& title, const wxString& mod ) = 0;

	virtual void OnUserJoinedBattle( int battleid, const wxString& nick, const wxString& userScriptPassword ) = 0;
	virtual void OnUserLeftBattle( int battleid, const wxString& nick ) = 0;
	virtual void OnBattleInfoUpdated( int battleid, int spectators, bool locked, const wxString& maphash, const wxString& map ) = 0;
	virtual void OnSetBattleInfo( int battleid, const wxString& param, const wxString& value ) = 0;
	virtual void OnBattleInfoUpdated( int battleid ) = 0;
	virtual void OnBattleClosed( int battleid ) = 0;

	virtual void OnJoinedBattle( int battleid, const wxString& hash ) = 0;
	virtual void OnHostedBattle( int battleid ) = 0;

	virtual void OnStartHostedBattle( int battleid ) = 0;
	virtual void OnClientBattleStatus( int battleid, const wxString& nick, UserBattleStatus status ) = 0;

	virtual void OnBattleStartRectAdd( int battleid, int allyno, int left, int top, int right, int bottom ) = 0;
	virtual void OnBattleStartRectRemove( int battleid, int allyno ) = 0;

	virtual void OnBattleAddBot( int battleid, const wxString& nick, UserBattleStatus status ) = 0;
	virtual void OnBattleUpdateBot( int battleid, const wxString& nick, UserBattleStatus status ) = 0;
	virtual void OnBattleRemoveBot( int battleid, const wxString& nick ) = 0;

	virtual void OnBattleDisableUnit( int battleid, const wxString& unitname, int count = 0 ) = 0;
	virtual void OnBattleEnableUnit( int battleid, const wxString& unitname ) = 0;
	virtual void OnBattleEnableAllUnits( int battleid ) = 0;

	virtual void OnJoinChannelResult( bool success, const wxString& channel, const wxString& reason ) = 0;

	virtual void OnChannelSaid( const wxString& channel, const wxString& who, const wxString& message ) = 0;
	virtual void OnChannelJoin( const wxString& channel, const wxString& who ) = 0;
	virtual void OnChannelPart( const wxString& channel, const wxString& who, const wxString& message ) = 0;
	virtual void OnChannelTopic( const wxString& channel, const wxString& who, const wxString& message, int when ) = 0;
	virtual void OnChannelAction( const wxString& channel, const wxString& who, const wxString& action ) = 0;
	virtual void OnChannelList( const wxString& channel, const int& numusers, const wxString& topic ) = 0;
	virtual void OnUserJoinChannel( const wxString& channel, const wxString& who ) = 0;

	virtual void OnPrivateMessage( const wxString& user, const wxString& message, bool fromme = false ) = 0;
	virtual void OnPrivateMessageEx( const wxString& user, const wxString& action, bool fromme = false ) = 0;

	virtual void OnRequestBattleStatus( int battleid ) = 0;
	virtual void OnSaidBattle( int battleid, const wxString& nick, const wxString& msg ) = 0;
	virtual void OnBattleAction( int battleid, const wxString& nick, const wxString& msg ) = 0;

	virtual void OnAcceptAgreement( const wxString& agreement ) = 0;

	virtual void OnRing( const wxString& from ) = 0;

	virtual void OnServerMessage( const wxString& message ) = 0;
	virtual void OnServerBroadcast( const wxString& message ) = 0;
	virtual void OnServerMessageBox( const wxString& message ) = 0;
	virtual void OnChannelMessage( const wxString& channel, const wxString& msg ) = 0;

	virtual void OnHostExternalUdpPort( const unsigned int udpport ) = 0;

	virtual void OnMyExternalUdpSourcePort( const unsigned int udpport ) = 0;
	virtual void OnMyInternalUdpSourcePort( const unsigned int udpport ) = 0;

	virtual void OnClientIPPort( const wxString &username, const wxString &ip, unsigned int udpport ) = 0;

	virtual void OnKickedFromBattle() = 0;

	virtual void OnRedirect( const wxString& address,  unsigned int port, const wxString& CurrentNick, const wxString& CurrentPassword ) = 0;

	/// use this function to check spam from clients and autokick from the battle
	virtual void AutoCheckCommandSpam( Battle& battle, User& nick ) = 0;

	virtual void OnMutelistBegin( const wxString& channel ) = 0;
	virtual void OnMutelistItem( const wxString& channel, const wxString& mutee, const wxString& description ) = 0;
	virtual void OnMutelistEnd( const wxString& channel ) = 0;

	virtual void OnScriptStart( int battleid ) = 0;
	virtual void OnScriptLine( int battleid, const wxString& line ) = 0;
	virtual void OnScriptEnd( int battleid ) = 0;

	virtual void OnFileDownload( bool autolaunch, bool autoclose, bool disconnectonrefuse, const wxString& FileName, const wxString& url, const wxString& description ) = 0;
	virtual void OnSpringDownloadEvent( wxCommandEvent& event ) = 0;

};

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


#endif // ISERVEREVENTS_H
