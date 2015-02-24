/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_SERVER_H
#define SPRINGLOBBY_HEADERGUARD_SERVER_H

#include <string>

#include "channellist.h"
#include "userlist.h"
#include "battlelist.h"
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

typedef int ServerError;
namespace LSL {
	class lslColor;
}

typedef int HostInfo;


//! @brief Abstract baseclass that is used to implement a server protocol.
class IServer : public SL::NonCopyable
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

    virtual bool ExecuteSayCommand( const std::string& /*cmd*/ ) {return true;};

    virtual void Register(const std::string& /*servername*/, const std::string& /*host*/, const int /*port*/, const std::string& /*user*/, const std::string& /*pass*/) {};
    virtual void AcceptAgreement() {};

    virtual void Connect( const std::string& /*servername*/, const std::string& /*addr*/, const int /*port*/ ) {};
    virtual void Disconnect() {};
    virtual bool IsConnected() {return true;};

    virtual void Login() {};
    virtual void Logout() {};
    virtual bool IsOnline()  const {return true;};

    virtual void JoinChannel( const std::string& /*channel*/, const std::string& /*key*/ ) {};
    virtual void PartChannel( const std::string& /*channel*/ ) {};

    virtual void DoActionChannel( const std::string& /*channel*/, const std::string& /*msg*/ ) {};
    virtual void SayChannel( const std::string& /*channel*/, const std::string& /*msg*/ ) {};

    virtual void SayPrivate( const std::string& /*nick*/, const std::string& /*msg*/ ) {};
    virtual void DoActionPrivate( const std::string& /*nick*/, const std::string& /*msg*/ ) {};

    virtual void SayBattle( int /*battleid*/, const std::string& /*msg*/ ) {};
    virtual void DoActionBattle( int /*battleid*/, const std::string& /*msg*/ ) {};

    virtual void Ring( const std::string& /*nick*/ ) {};

    virtual void ModeratorSetChannelTopic( const std::string& /*channel*/, const std::string& /*topic*/ ) {};
    virtual void ModeratorSetChannelKey( const std::string& /*channel*/, const std::string& /*key*/ ) {};
    virtual void ModeratorMute( const std::string& /*channel*/, const std::string& /*nick*/, int /*duration*/, bool /*byip*/ ) {};
    virtual void ModeratorUnmute( const std::string& /*channel*/, const std::string& /*nick*/) {};
    virtual void ModeratorKick( const std::string& /*channel*/, const std::string& /*reason*/ ) {};
    virtual void ModeratorBan( const std::string& /*nick*/, bool /*byip*/ ) {};
    virtual void ModeratorUnban( const std::string& /*nick*/) {};
    virtual void ModeratorGetIP( const std::string& /*nick*/) {};
    virtual void ModeratorGetLastLogin( const std::string& /*nick*/) {};
    virtual void ModeratorGetLastIP( const std::string& /*nick*/) {};
    virtual void ModeratorFindByIP( const std::string& /*ipadress*/ ) {};

    virtual void AdminGetAccountAccess( const std::string& /*nick*/) {};
    virtual void AdminChangeAccountAccess( const std::string& /*nick*/, const std::string& /*accesscode*/ ) {};
    virtual void AdminSetBotMode( const std::string& /*nick*/, bool /*isbot*/ ) {};

    virtual void HostBattle( BattleOptions /*bo*/, const std::string& /*password*/ = "" ) {};
    virtual void JoinBattle( const int& /*battleid*/, const std::string& /*password*/ = "" ) {};
    virtual void LeaveBattle( const int& /*battleid*/ ) {};
    virtual void StartHostedBattle() {};

    virtual void ForceSide( int /*battleid*/, User& /*user*/, int /*side*/ ) {};
    virtual void ForceTeam( int /*battleid*/, User& /*user*/, int /*team*/ ) {};
    virtual void ForceAlly( int /*battleid*/, User& /*user*/, int /*ally*/ ) {};
    virtual void ForceColour( int /*battleid*/, User& /*user*/, const LSL::lslColor& /*col*/ ) {};
    virtual void ForceSpectator( int /*battleid*/, User& /*user*/, bool /*spectator*/ ) {};
    virtual void BattleKickPlayer( int /*battleid*/, User& /*user*/ ) {};
    virtual void SetHandicap( int /*battleid*/, User& /*user*/, int /*handicap*/) {};


    virtual void AddBot( int /*battleid*/, const std::string& /*nick*/, UserBattleStatus& /*status*/ ) {};
    virtual void RemoveBot( int /*battleid*/, User& /*user*/ ) {};
    virtual void UpdateBot( int /*battleid*/, User& /*user*/, UserBattleStatus& /*status*/) {};

    virtual void SendHostInfo( HostInfo /*update*/ ) {};
    virtual void SendHostInfo( const std::string& /*Tag*/ ) {};
    virtual void SendRaw( const std::string& /*raw*/ ) {};
    virtual void SendUserPosition( const User& /*usr*/ ) {};

    virtual void RequestInGameTime( const std::string& /*nick*/) {};

    virtual IBattle* GetCurrentBattle() {return NULL;};

    virtual void RequestChannels() {};

    virtual void SendMyBattleStatus( UserBattleStatus& /*bs*/ ) {};
    virtual void SendMyUserStatus(const UserStatus& /*us*/) {};

    virtual void SetUsername( const std::string& username ) { m_user = username; }
	virtual const std::string& GetUserName() const {return m_user; }
    virtual void SetPassword( const std::string& password ) { m_pass = password; }
	virtual const std::string& GetPassword() const {return m_pass; }
    virtual bool IsPasswordHash( const std::string& /*pass*/ ) const {return true;};
    virtual std::string GetPasswordHash( const std::string& /*pass*/ ) const {return "";};

    std::string GetRequiredSpring() const { return m_required_spring_ver; }

    void SetRequiredSpring( const std::string& version ) { m_required_spring_ver = version; }

    BattleList_Iter* const battles_iter;

    virtual const User& GetMe() const {return GetUser(m_user);};
    virtual User& GetMe() {return GetUser(m_user);};
    User& GetUser( const std::string& nickname ) const;
    bool UserExists( const std::string& nickname ) const;

    Channel& GetChannel( const std::string& name );
    int GetNumChannels() const;
    Channel& GetChannel( const int& index );
    bool ChannelExists( const std::string& name ) const;

    IBattle& GetBattle( const int& /*battleid*/ );
    bool BattleExists( const int& /*battleid*/ ) const;

    virtual int TestOpenPort( unsigned int /*port*/ ) const { return 0;};

    virtual void SendScriptToProxy( const std::string& /*script*/ ) {};

    virtual void SendScriptToClients( const std::string& /*script*/ ) {};

    std::map<std::string,std::string> m_channel_pw;  /// /*channel*/ name -> password, filled on /*channel*/ join

    ///used to fill /*user*/list in groupuserdialog
    const UserList& GetUserList() const {return m_users;}

    unsigned int GetNumUsers() const { return m_users.GetNumUsers(); }

    std::string GetServerName() const { return m_server_name; }

	virtual void SetRelayIngamePassword( const User& /*user*/ ) {};

	virtual LSL::StringVector GetRelayHostList() { return LSL::StringVector(); }
	virtual LSL::StringSet GetSubscriptions() { return LSL::StringSet(); }

	void Reset();

protected:
    std::string m_server_name;

private:
    std::string m_user;
    std::string m_pass;

    bool m_pass_hash;
    std::string m_required_spring_ver;
    UserList m_users;
    ChannelList m_channels;
    BattleList m_battles;

    User& _AddUser( const std::string& /*user*/ );
    void _RemoveUser( const std::string& nickname );

    Channel& _AddChannel( const std::string& chan );
    void _RemoveChannel( const std::string& name );

    IBattle& _AddBattle( const int& id );
    void _RemoveBattle( const int& id );

    virtual void SendCmd( const std::string& /*command*/, const std::string& /*param*/ ) {};
    virtual void RelayCmd( const std::string& /*command*/, const std::string& /*param*/ ) {};

};

#endif // SPRINGLOBBY_HEADERGUARD_SERVER_H
