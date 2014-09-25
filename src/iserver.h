/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_SERVER_H
#define SPRINGLOBBY_HEADERGUARD_SERVER_H

#include <wx/string.h>
#include <wx/arrstr.h>

#include "channellist.h"
#include "userlist.h"
#include "battlelist.h"
#include "inetclass.h"
#include "utils/mixins.h"
#include "ibattle.h"

class ServerEvents;
class SimpleServerEvents;
class IServerEvents;
class Channel;
class Ui;
struct BattleOptions;
class User;
struct UserBattleStatus;
class ChatPanel;
class wxString;
typedef int ServerError;
namespace LSL {
	class lslColor;
}

typedef int HostInfo;


//! @brief Abstract baseclass that is used to implement a server protocol.
class IServer : public iNetClass, public SL::NonCopyable
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


    IServer();
    virtual ~IServer( );

    // Server interface

    virtual bool ExecuteSayCommand( const wxString& /*cmd*/ ) {return true;};

    virtual void Register(const wxString& /*servername*/, const wxString& /*host*/, const int /*port*/, const wxString& /*user*/, const wxString& /*pass*/) {};
    virtual void AcceptAgreement() {};

    virtual void Connect( const wxString& /*servername*/, const wxString& /*addr*/, const int /*port*/ ) {};
    virtual void Disconnect() {};
    virtual bool IsConnected() {return true;};

    virtual void Login() {};
    virtual void Logout() {};
    virtual bool IsOnline()  const {return true;};

    virtual void JoinChannel( const wxString& /*channel*/, const wxString& /*key*/ ) {};
    virtual void PartChannel( const wxString& /*channel*/ ) {};

    virtual void DoActionChannel( const wxString& /*channel*/, const wxString& /*msg*/ ) {};
    virtual void SayChannel( const wxString& /*channel*/, const wxString& /*msg*/ ) {};

    virtual void SayPrivate( const wxString& /*nick*/, const wxString& /*msg*/ ) {};
    virtual void DoActionPrivate( const wxString& /*nick*/, const wxString& /*msg*/ ) {};

    virtual void SayBattle( int /*battleid*/, const wxString& /*msg*/ ) {};
    virtual void DoActionBattle( int /*battleid*/, const wxString& /*msg*/ ) {};

    virtual void Ring( const wxString& /*nick*/ ) {};

    virtual void ModeratorSetChannelTopic( const wxString& /*channel*/, const wxString& /*topic*/ ) {};
    virtual void ModeratorSetChannelKey( const wxString& /*channel*/, const wxString& /*key*/ ) {};
    virtual void ModeratorMute( const wxString& /*channel*/, const wxString& /*nick*/, int /*duration*/, bool /*byip*/ ) {};
    virtual void ModeratorUnmute( const wxString& /*channel*/, const wxString& /*nick*/) {};
    virtual void ModeratorKick( const wxString& /*channel*/, const wxString& /*reason*/ ) {};
    virtual void ModeratorBan( const wxString& /*nick*/, bool /*byip*/ ) {};
    virtual void ModeratorUnban( const wxString& /*nick*/) {};
    virtual void ModeratorGetIP( const wxString& /*nick*/) {};
    virtual void ModeratorGetLastLogin( const wxString& /*nick*/) {};
    virtual void ModeratorGetLastIP( const wxString& /*nick*/) {};
    virtual void ModeratorFindByIP( const wxString& /*ipadress*/ ) {};

    virtual void AdminGetAccountAccess( const wxString& /*nick*/) {};
    virtual void AdminChangeAccountAccess( const wxString& /*nick*/, const wxString& /*accesscode*/ ) {};
    virtual void AdminSetBotMode( const wxString& /*nick*/, bool /*isbot*/ ) {};

    virtual void HostBattle( BattleOptions /*bo*/, const wxString& /*password*/ = wxEmptyString ) {};
    virtual void JoinBattle( const int& /*battleid*/, const wxString& /*password*/ = wxEmptyString ) {};
    virtual void LeaveBattle( const int& /*battleid*/ ) {};
    virtual void StartHostedBattle() {};

    virtual void ForceSide( int /*battleid*/, User& /*user*/, int /*side*/ ) {};
    virtual void ForceTeam( int /*battleid*/, User& /*user*/, int /*team*/ ) {};
    virtual void ForceAlly( int /*battleid*/, User& /*user*/, int /*ally*/ ) {};
    virtual void ForceColour( int /*battleid*/, User& /*user*/, const LSL::lslColor& /*col*/ ) {};
    virtual void ForceSpectator( int /*battleid*/, User& /*user*/, bool /*spectator*/ ) {};
    virtual void BattleKickPlayer( int /*battleid*/, User& /*user*/ ) {};
    virtual void SetHandicap( int /*battleid*/, User& /*user*/, int /*handicap*/) {};


    virtual void AddBot( int /*battleid*/, const wxString& /*nick*/, UserBattleStatus& /*status*/ ) {};
    virtual void RemoveBot( int /*battleid*/, User& /*user*/ ) {};
    virtual void UpdateBot( int /*battleid*/, User& /*user*/, UserBattleStatus& /*status*/) {};

    virtual void SendHostInfo( HostInfo /*update*/ ) {};
    virtual void SendHostInfo( const wxString& /*Tag*/ ) {};
    virtual void SendRaw( const wxString& /*raw*/ ) {};
    virtual void SendUserPosition( const User& /*usr*/ ) {};

    virtual void RequestInGameTime( const wxString& /*nick*/) {};

    virtual IBattle* GetCurrentBattle() {return NULL;};

    virtual void RequestChannels() {};

    virtual void SendMyBattleStatus( UserBattleStatus& /*bs*/ ) {};
    virtual void SendMyUserStatus() {};

    virtual void SetUsername( const wxString& username ) { m_user = username; }
	virtual const wxString& GetUserName() const {return m_user; }
    virtual void SetPassword( const wxString& password ) { m_pass = password; }
	virtual const wxString& GetPassword() const {return m_pass; }
    virtual bool IsPasswordHash( const wxString& /*pass*/ ) const {return true;};
    virtual wxString GetPasswordHash( const wxString& /*pass*/ ) const {return wxEmptyString;};

    wxString GetRequiredSpring() const { return m_required_spring_ver; }

    void SetRequiredSpring( const wxString& version ) { m_required_spring_ver = version; }

    virtual void OnConnected( Socket& /*sock*/ ) {};
    virtual void OnDisconnected( Socket& /*sock*/ ) {};
    virtual void OnDataReceived( Socket& /*sock*/ ) {};

    virtual void OnDisconnected();

    BattleList_Iter* const battles_iter;

    virtual const User& GetMe() const {return GetUser(m_user);};
    virtual User& GetMe() {return GetUser(m_user);};
    User& GetUser( const wxString& nickname ) const;
    bool UserExists( const wxString& nickname ) const;

    Channel& GetChannel( const wxString& name );
    int GetNumChannels() const;
    Channel& GetChannel( const int& index );
    bool ChannelExists( const wxString& name ) const;

    IBattle& GetBattle( const int& /*battleid*/ );
    bool BattleExists( const int& /*battleid*/ ) const;

    virtual int TestOpenPort( unsigned int /*port*/ ) const { return 0;};

    virtual void SendScriptToProxy( const wxString& /*script*/ ) {};

    virtual void SendScriptToClients( const wxString& /*script*/ ) {};

    std::map<wxString,wxString> m_channel_pw;  /// /*channel*/ name -> password, filled on /*channel*/ join

    ///used to fill /*user*/list in groupuserdialog
    const UserList& GetUserList() const {return m_users;}

    unsigned int GetNumUsers() const { return m_users.GetNumUsers(); }

    wxString GetServerName() const { return m_server_name; }

	virtual void SetRelayIngamePassword( const User& /*user*/ ) {};

	virtual wxArrayString GetRelayHostList();

protected:
	Socket* m_sock;
    wxString m_server_name;
    UserList m_users;

private:
    wxString m_user;
    wxString m_pass;

    bool m_pass_hash;
    wxString m_required_spring_ver;

    ChannelList m_channels;
    BattleList m_battles;


    wxArrayString m_relay_host_manager_list;

    User& _AddUser( const wxString& /*user*/ );
    void _RemoveUser( const wxString& nickname );

    Channel& _AddChannel( const wxString& chan );
    void _RemoveChannel( const wxString& name );

    IBattle& _AddBattle( const int& id );
    void _RemoveBattle( const int& id );

    virtual void SendCmd( const wxString& /*command*/, const wxString& /*param*/ ) {};
    virtual void RelayCmd( const wxString& /*command*/, const wxString& /*param*/ ) {};

};

#endif // SPRINGLOBBY_HEADERGUARD_SERVER_H
