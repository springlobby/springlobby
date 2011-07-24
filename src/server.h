#ifndef SPRINGLOBBY_HEADERGUARD_SERVER_H
#define SPRINGLOBBY_HEADERGUARD_SERVER_H

#include <wx/string.h>
#include <wx/arrstr.h>

#include "channel/channellist.h"
#include "userlist.h"
#include "battlelist.h"
#include "inetclass.h"
#include "utils/mixins.hh"

class ServerEvents;
class SimpleServerEvents;
class Channel;
class Ui;
struct BattleOptions;
class User;
struct UserBattleStatus;
class ChatPanel;
class wxString;
typedef int ServerError;
class wxColour;


typedef int HostInfo;


//! @brief Abstract baseclass that is used to implement a server protocol.
class Server : public iNetClass, public SL::NonCopyable
{
  public:
	friend class ServerEvents;
	friend class SimpleServerEvents;

    enum PortTestCode {
      porttest_pass_WX26    = 0,
      porttest_pass         = 1,
      porttest_timeout      = 2,
      porttest_socketNotOk  = 3,
      porttest_socketError  = 4,
      porttest_unreachable  = 5
    };

    struct UiServerData {
      UiServerData(): panel(0) {}
      ChatPanel* panel;
    };

    UiServerData uidata;


    Server();
    virtual ~Server( );

    // Server interface

    virtual bool ExecuteSayCommand( const wxString& cmd ) = 0;

    virtual bool Register( const wxString& addr, const int port, const wxString& nick, const wxString& password,wxString& reason ) = 0;
    virtual void AcceptAgreement() = 0;

    virtual void Connect( const wxString& servername, const wxString& addr, const int port ) = 0;
    virtual void Disconnect() = 0;
    virtual bool IsConnected() = 0;

    virtual void Login() = 0;
    virtual void Logout() = 0;
    virtual bool IsOnline()  const = 0;

    virtual void Update( int mselapsed ) = 0;

    virtual void JoinChannel( const wxString& channel, const wxString& key ) = 0;
    virtual void PartChannel( const wxString& channel ) = 0;

    virtual void DoActionChannel( const wxString& channel, const wxString& msg ) = 0;
    virtual void SayChannel( const wxString& channel, const wxString& msg ) = 0;

    virtual void SayPrivate( const wxString& nick, const wxString& msg ) = 0;
    virtual void DoActionPrivate( const wxString& nick, const wxString& msg ) = 0;

    virtual void SayBattle( int battleid, const wxString& msg ) = 0;
    virtual void DoActionBattle( int battleid, const wxString& msg ) = 0;

    virtual void Ring( const wxString& nick ) = 0;

    virtual void ModeratorSetChannelTopic( const wxString& channel, const wxString& topic ) = 0;
    virtual void ModeratorSetChannelKey( const wxString& channel, const wxString& key ) = 0;
    virtual void ModeratorMute( const wxString& channel, const wxString& nick, int duration, bool byip ) = 0;
    virtual void ModeratorUnmute( const wxString& channel, const wxString& nick ) = 0;
    virtual void ModeratorKick( const wxString& channel, const wxString& reason ) = 0;
    virtual void ModeratorBan( const wxString& nick, bool byip ) = 0;
    virtual void ModeratorUnban( const wxString& nick ) = 0;
    virtual void ModeratorGetIP( const wxString& nick ) = 0;
    virtual void ModeratorGetLastLogin( const wxString& nick ) = 0;
    virtual void ModeratorGetLastIP( const wxString& nick ) = 0;
    virtual void ModeratorFindByIP( const wxString& ipadress ) = 0;

    virtual void AdminGetAccountAccess( const wxString& nick ) = 0;
    virtual void AdminChangeAccountAccess( const wxString& nick, const wxString& accesscode ) = 0;
    virtual void AdminSetBotMode( const wxString& nick, bool isbot ) = 0;

    virtual void HostBattle( BattleOptions bo, const wxString& password = _T("") ) = 0;
    virtual void JoinBattle( const int& battleid, const wxString& password = _T("") ) = 0;
    virtual void LeaveBattle( const int& battleid ) = 0;
    virtual void StartHostedBattle() = 0;

    virtual void ForceSide( int battleid, User& user, int side ) = 0;
    virtual void ForceTeam( int battleid, User& user, int team ) = 0;
    virtual void ForceAlly( int battleid, User& user, int ally ) = 0;
    virtual void ForceColour( int battleid, User& user, const wxColour& col ) = 0;
    virtual void ForceSpectator( int battleid, User& user, bool spectator ) = 0;
    virtual void BattleKickPlayer( int battleid, User& user ) = 0;
    virtual void SetHandicap( int battleid, User& user, int handicap) = 0;


    virtual void AddBot( int battleid, const wxString& nick, UserBattleStatus& status ) = 0;
    virtual void RemoveBot( int battleid, User& user ) = 0;
    virtual void UpdateBot( int battleid, User& user, UserBattleStatus& status ) = 0;

    virtual void SendHostInfo( HostInfo update ) = 0;
    virtual void SendHostInfo( const wxString& Tag ) = 0;
    virtual void SendRaw( const wxString& raw ) = 0;
    virtual void SendUserPosition( const User& usr ) = 0;

    virtual void RequestInGameTime( const wxString& nick ) = 0;

    virtual Battle* GetCurrentBattle() = 0;

    virtual void RequestChannels() = 0;

    virtual void SendMyBattleStatus( UserBattleStatus& bs ) = 0;
    virtual void SendMyUserStatus() = 0;

    virtual void SetKeepaliveInterval( int seconds ) { m_keepalive = seconds; }
    virtual int GetKeepaliveInterval() { return m_keepalive; }

    virtual void SetUsername( const wxString& username ) { m_user = username; }
    virtual void SetPassword( const wxString& password ) { m_pass = password; }
    virtual bool IsPasswordHash( const wxString& pass ) const = 0;
    virtual wxString GetPasswordHash( const wxString& pass ) const = 0;

    wxString GetRequiredSpring() const { return m_required_spring_ver; }

    void SetRequiredSpring( const wxString& version ) { m_required_spring_ver = version; }

    virtual void OnConnected( Socket* sock ) = 0;
    virtual void OnDisconnected( Socket* sock ) = 0;
    virtual void OnDataReceived( Socket* sock ) = 0;

    virtual void OnDisconnected();

    BattleList_Iter* const battles_iter;

    virtual const User& GetMe() const = 0;
    virtual User& GetMe() = 0;
    User& GetUser( const wxString& nickname ) const;
    bool UserExists( const wxString& nickname ) const;

    Channel& GetChannel( const wxString& name );
    int GetNumChannels() const;
    Channel& GetChannel( const int& index );
    bool ChannelExists( const wxString& name ) const;

    Battle& GetBattle( const int& battleid );
    bool BattleExists( const int& battleid ) const;

    virtual int TestOpenPort( unsigned int port ) const = 0;

    virtual void SendScriptToProxy( const wxString& script ) = 0;

    virtual void SendScriptToClients( const wxString& script ) = 0;

    std::map<wxString,wxString> m_channel_pw;  /// channel name -> password, filled on channel join

    ///used to fill userlist in groupuserdialog
    const UserList& GetUserList() const {return m_users;}

    unsigned int GetNumUsers() const { return m_users.GetNumUsers(); }

    wxString GetServerName() const { return m_server_name; }

    virtual void RequestSpringUpdate();

	virtual void SetRelayIngamePassword( const User& user ) = 0;

    virtual wxArrayString GetRelayHostList() ;

  protected:
	Socket* m_sock;
    int m_keepalive;
    wxString m_user;
    wxString m_pass;
    wxString m_server_name;
    bool m_pass_hash;
    wxString m_required_spring_ver;

    ChannelList m_channels;
    UserList m_users;
    BattleList m_battles;

    wxString m_relay_host_bot;
    wxString m_relay_host_manager;

    wxArrayString m_relay_host_manager_list;

    User& _AddUser( const wxString& user );
    void _RemoveUser( const wxString& nickname );

    Channel& _AddChannel( const wxString& chan );
    void _RemoveChannel( const wxString& name );

    Battle& _AddBattle( const int& id );
    void _RemoveBattle( const int& id );

    static const unsigned int PING_TIMEOUT = 40;

    virtual void SendCmd( const wxString& command, const wxString& param ) = 0;
    virtual void RelayCmd( const wxString& command, const wxString& param ) = 0;

};

#endif // SPRINGLOBBY_HEADERGUARD_SERVER_H

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

