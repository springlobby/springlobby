/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef ISERVEREVENTS_H
#define ISERVEREVENTS_H


/**
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

DO NOT CHANGE THIS FILE!

this file is deprecated and will be replaced with

lsl/networking/iserver.h

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
**/


//almost only needed for NAtType enum def
#include "battle.h"
#include <string>

struct MessageSpamCheck
{
  time_t lastmessage;
  unsigned int count;
};

class IServer;
struct UserStatus;

// FIXME this is defined elsewhere, should use a different kind of type so we could use forward decl
typedef int Sockerror;

typedef int Protocolerror;

class IServerEvents
{
  public:
	IServerEvents( ) {}
	virtual ~IServerEvents() {}
  // Uicontrol interface

	virtual void OnConnected( const std::string& server_name, const std::string& server_ver, bool supported, const std::string& server_spring_ver, bool lanmode ) = 0;
	virtual void OnDisconnected( bool wasonline ) = 0;

	virtual void OnLogin() = 0;
	virtual void OnLoginInfoComplete() = 0;

	virtual void OnUnknownCommand( const std::string& command, const std::string& params ) = 0;
	virtual void OnSocketError( const Sockerror& error ) = 0;
	virtual void OnProtocolError( const Protocolerror error ) = 0;
	virtual void OnMotd( const std::string& msg ) = 0;
	virtual void OnPong( wxLongLong ping_time ) = 0;

	virtual void OnNewUser( const std::string& nick, const std::string& conutry, int cpu, const int id ) = 0;
	virtual void OnUserStatus( const std::string& nick, UserStatus status ) = 0;
	virtual void OnUserQuit( const std::string& nick ) = 0;

	virtual void OnBattleOpened( int id, BattleType type, NatType nat, const std::string& nick,
						 const std::string& host, int port, int maxplayers,
						 bool haspass, int rank, const std::string& maphash, const std::string& engineVersion, const std::string& engineName, const std::string& map,
						 const std::string& title, const std::string& mod ) = 0;

	virtual void OnUserJoinedBattle( int battleid, const std::string& nick, const std::string& userScriptPassword ) = 0;
	virtual void OnUserLeftBattle( int battleid, const std::string& nick ) = 0;
	virtual void OnBattleInfoUpdated( int battleid, int spectators, bool locked, const std::string& maphash, const std::string& map ) = 0;
	virtual void OnSetBattleInfo( int battleid, const std::string& param, const std::string& value ) = 0;
	virtual void OnUnsetBattleInfo( int battleid, const std::string& param) = 0;
	virtual void OnBattleInfoUpdated( int battleid ) = 0;
	virtual void OnBattleClosed( int battleid ) = 0;

	virtual void OnJoinedBattle( int battleid, const std::string& hash ) = 0;
	virtual void OnHostedBattle( int battleid ) = 0;

	virtual void OnStartHostedBattle( int battleid ) = 0;
	virtual void OnClientBattleStatus( int battleid, const std::string& nick, UserBattleStatus status ) = 0;

	virtual void OnBattleStartRectAdd( int battleid, int allyno, int left, int top, int right, int bottom ) = 0;
	virtual void OnBattleStartRectRemove( int battleid, int allyno ) = 0;

	virtual void OnBattleAddBot( int battleid, const std::string& nick, UserBattleStatus status ) = 0;
	virtual void OnBattleUpdateBot( int battleid, const std::string& nick, UserBattleStatus status ) = 0;
	virtual void OnBattleRemoveBot( int battleid, const std::string& nick ) = 0;

	virtual void OnBattleDisableUnit( int battleid, const std::string& unitname, int count = 0 ) = 0;
	virtual void OnBattleEnableUnit( int battleid, const std::string& unitname ) = 0;
	virtual void OnBattleEnableAllUnits( int battleid ) = 0;

	virtual void OnJoinChannelResult( bool success, const std::string& channel, const std::string& reason ) = 0;

	virtual void OnChannelSaid( const std::string& channel, const std::string& who, const std::string& message ) = 0;
	virtual void OnChannelJoin( const std::string& channel, const std::string& who ) = 0;
	virtual void OnChannelPart( const std::string& channel, const std::string& who, const std::string& message ) = 0;
	virtual void OnChannelTopic( const std::string& channel, const std::string& who, const std::string& message, int when ) = 0;
	virtual void OnChannelAction( const std::string& channel, const std::string& who, const std::string& action ) = 0;
	virtual void OnChannelList( const std::string& channel, const int& numusers, const std::string& topic ) = 0;
	virtual void OnUserJoinChannel( const std::string& channel, const std::string& who ) = 0;

	virtual void OnPrivateMessage( const std::string& user, const std::string& message, bool fromme = false ) = 0;
	virtual void OnPrivateMessageEx( const std::string& user, const std::string& action, bool fromme = false ) = 0;

	virtual void OnRequestBattleStatus( int battleid ) = 0;
	virtual void OnSaidBattle( int battleid, const std::string& nick, const std::string& msg ) = 0;
	virtual void OnBattleAction( int battleid, const std::string& nick, const std::string& msg ) = 0;

	virtual void OnAcceptAgreement( const std::string& agreement ) = 0;

	virtual void OnRing( const std::string& from ) = 0;

	virtual void OnServerMessage( const std::string& message ) = 0;
	virtual void OnServerBroadcast( const std::string& message ) = 0;
	virtual void OnServerMessageBox( const std::string& message ) = 0;
	virtual void OnChannelMessage( const std::string& channel, const std::string& msg ) = 0;

	virtual void OnHostExternalUdpPort( const unsigned int udpport ) = 0;

	virtual void OnMyExternalUdpSourcePort( const unsigned int udpport ) = 0;
	virtual void OnMyInternalUdpSourcePort( const unsigned int udpport ) = 0;

	virtual void OnClientIPPort( const std::string &username, const std::string &ip, unsigned int udpport ) = 0;

	virtual void OnKickedFromBattle() = 0;

	virtual void OnRedirect( const std::string& address,  unsigned int port, const std::string& CurrentNick, const std::string& CurrentPassword ) = 0;

	/// use this function to check spam from clients and autokick from the battle
	virtual void AutoCheckCommandSpam( Battle& battle, User& nick ) = 0;

	virtual void OnMutelistBegin( const std::string& channel ) = 0;
	virtual void OnMutelistItem( const std::string& channel, const std::string& mutee, const std::string& description ) = 0;
	virtual void OnMutelistEnd( const std::string& channel ) = 0;

	virtual void OnScriptStart( int battleid ) = 0;
	virtual void OnScriptLine( int battleid, const std::string& line ) = 0;
	virtual void OnScriptEnd( int battleid ) = 0;

	virtual void OnForceJoinBattle( int battleid, const std::string& line ) = 0;
	virtual void RegistrationAccepted(const std::string& /*user*/, const std::string& /*pass*/) = 0;
	virtual void RegistrationDenied(const std::string& /*reason*/) = 0;
	virtual void OnLoginDenied(const std::string& /*reason*/) = 0;
};

#endif // ISERVEREVENTS_H
