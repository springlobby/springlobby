#ifndef SPRINGLOBBY_HEADERGUARD_SERVER_H
#define SPRINGLOBBY_HEADERGUARD_SERVER_H

#include <wx/string.h>

#include "channellist.h"
#include "userlist.h"
#include "battlelist.h"
#include "inetclass.h"

#define PING_TIMEOUT 30

class ServerEvents;
class Channel;
class Ui;
class Battle;
class User;
struct UserBattleStatus;
class ChatPanel;
struct BattleOptions;
class wxString;
typedef int ServerError;
class wxColour;

#define PE_NONE 0


#define HI_None 0
#define HI_Map 1
#define HI_Locked 2
#define HI_Spectators 4
#define HI_StartResources 8
#define HI_MaxUnits 16
#define HI_StartType 32
#define HI_GameType 64
#define HI_Options 128
#define HI_StartRects 256
#define HI_Restrictions 512
#define HI_Map_Changed 1024
#define HI_Mod_Changed 2048
#define HI_Send_All_opts 4076

typedef int HostInfo;

struct UiServerData {
  UiServerData(): panel(0) {}
  ChatPanel* panel;
};

//! @brief Abstract baseclass that is used to implement a server protocol.
class Server : public iNetClass
{
  public:
    friend class ServerEvents;


    UiServerData uidata;


    Server( Ui& ui): battles_iter(new BattleList_Iter(&m_battles)),m_sock(0), m_ui(ui),m_keepalive(15) {  }
    virtual ~Server( );

    // Server interface

    virtual bool ExecuteSayCommand( const wxString& cmd ) = 0;

    virtual void SetSocket( Socket* sock );
    virtual Socket* GetSocket( ) { return m_sock; }

    virtual bool Register( const wxString& addr, const int port, const wxString& nick, const wxString& password,wxString& reason ) = 0;
    virtual void AcceptAgreement() = 0;

    virtual void Connect( const wxString& addr, const int port ) = 0;
    virtual void Disconnect() = 0;
    virtual bool IsConnected() = 0;

    virtual void Login() = 0;
    virtual void Logout() = 0;
    virtual bool IsOnline() = 0;

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

    virtual void ForceSide( int battleid, const wxString& nick, int side ) = 0;
    virtual void ForceTeam( int battleid, const wxString& nick, int team ) = 0;
    virtual void ForceAlly( int battleid, const wxString& nick, int ally ) = 0;
    virtual void ForceColour( int battleid, const wxString& nick, const wxColour& col ) = 0;
    virtual void ForceSpectator( int battleid, const wxString& nick, bool spectator ) = 0;
    virtual void BattleKickPlayer( int battleid, const wxString& nick ) = 0;
    virtual void SetHandicap( int battleid, const wxString& nick, int handicap) = 0;


    virtual void AddBot( int battleid, const wxString& nick, const wxString& owner, UserBattleStatus status, const wxString& aidll ) = 0;
    virtual void RemoveBot( int battleid, const wxString& nick ) = 0;
    virtual void UpdateBot( int battleid, const wxString& nick, UserBattleStatus status ) = 0;

    virtual void SendHostInfo( HostInfo update ) = 0;
    virtual void SendHostInfo( const wxString& Tag ) = 0;
    virtual void SendRaw( const wxString& raw ) = 0;

    virtual void RequestInGameTime( const wxString& nick ) = 0;

    virtual Battle* GetCurrentBattle() = 0;

    virtual void RequestChannels() = 0;

    virtual void SendMyBattleStatus( UserBattleStatus& bs ) = 0;
    virtual void SendMyUserStatus() = 0;

    virtual void SetKeepaliveInterval( int seconds ) { m_keepalive = seconds; }
    virtual int GetKeepaliveInterval() { return m_keepalive; }

    virtual void SetUsername( const wxString& username ) { m_user = username; }
    virtual void SetPassword( const wxString& password ) { m_pass = password; }
    virtual bool IsPasswordHash( const wxString& pass ) = 0;
    virtual wxString GetPasswordHash( const wxString& pass ) = 0;

    wxString GetRequiredSpring() { return m_required_spring_ver; }

    void SetRequiredSpring( const wxString& version ) { m_required_spring_ver = version; }

    virtual void Ping() = 0;

    virtual void OnConnected( Socket* sock ) = 0;
    virtual void OnDisconnected( Socket* sock ) = 0;
    virtual void OnDataReceived( Socket* sock ) = 0;

    BattleList_Iter* const battles_iter;

    virtual User& GetMe() = 0;
    User& GetUser( const wxString& nickname );
    bool UserExists( const wxString& nickname );

    Channel& GetChannel( const wxString& name );
    int GetNumChannels();
    Channel& GetChannel( const int& index );
    bool ChannelExists( const wxString& name );

    Battle& GetBattle( const int& battleid );
    bool BattleExists( const int& battleid );

    virtual bool TestOpenPort( unsigned int port ) = 0;

  protected:
    Socket* m_sock;
    Ui& m_ui;
    int m_keepalive;
    wxString m_user;
    wxString m_pass;
    bool m_pass_hash;
    wxString m_required_spring_ver;

    ChannelList m_channels;
    UserList m_users;
    BattleList m_battles;

    User& _AddUser( const wxString& user );
    void _RemoveUser( const wxString& nickname );

    Channel& _AddChannel( const wxString& chan );
    void _RemoveChannel( const wxString& name );

    Battle& _AddBattle( const int& id );
    void _RemoveBattle( const int& id );
};

#endif // SPRINGLOBBY_HEADERGUARD_SERVER_H
