#ifndef SPRINGLOBBY_HEADERGUARD_TASSERVER_H
#define SPRINGLOBBY_HEADERGUARD_TASSERVER_H

#include <wx/string.h>
#include <wx/longlong.h>
#include <list>

#include "server.h"
#include "crc.h"
#include "mutexwrapper.h"

const unsigned int FIRST_UDP_SOURCEPORT = 8300;

class Ui;
class Socket;
class User;
struct UserBattleStatus;
class IServerEvents;
class wxString;
class PingThread;

//! @brief TASServer protocol implementation.
class TASServer : public Server
{
  public:
	TASServer(int serverEventsMode = 0);
    ~TASServer();

    // Overloaded functions from Server
    bool ExecuteSayCommand( const wxString& cmd );

    void SetSocket( Socket* sock );

    bool Register( const wxString& addr, const int port, const wxString& nick, const wxString& password,wxString& reason );
    void AcceptAgreement();

    void Connect( const wxString& servername, const wxString& addr, const int port );
    void Disconnect();
    bool IsConnected();

    void Login();
    void Logout();
    bool IsOnline() const;

    void Update( int mselapsed );

	void Ping();

    void UDPPing();/// used for nat travelsal
    /// generic udp "ping" function
    /// return value: actual source port which was used. May differ from src_port
    /// 0 if udp ping failed
    unsigned int UdpPing(unsigned int src_port, const wxString &target, unsigned int target_port, const wxString &message);
    /// specialized udp ping functions
    void UdpPingTheServer( const wxString &message );/// used for nat travelsal. pings the server.
    void UdpPingAllClients();/// used when hosting with nat holepunching

    const User& GetMe() const;
    User& GetMe();

    void JoinChannel( const wxString& channel, const wxString& key );
    void PartChannel( const wxString& channel );

    void DoActionChannel( const wxString& channel, const wxString& msg );
    void SayChannel( const wxString& channel, const wxString& msg );

    void DoActionPrivate( const wxString& nick, const wxString& msg );
    void SayPrivate( const wxString& nick, const wxString& msg );

    void SayBattle( int battleid, const wxString& msg );
    void DoActionBattle( int battleid, const wxString& msg );

    void Ring( const wxString& nick );

    void ModeratorSetChannelTopic( const wxString& channel, const wxString& topic );
    void ModeratorSetChannelKey( const wxString& channel, const wxString& key );
    void ModeratorMute( const wxString& channel, const wxString& nick, int duration, bool byip );
    void ModeratorUnmute( const wxString& channel, const wxString& nick );
    void ModeratorKick( const wxString& channel, const wxString& reason );
    void ModeratorBan( const wxString& nick, bool byip );
    void ModeratorUnban( const wxString& nick );
    void ModeratorGetIP( const wxString& nick );
    void ModeratorGetLastLogin( const wxString& nick );
    void ModeratorGetLastIP( const wxString& nick );
    void ModeratorFindByIP( const wxString& ipadress );

    void AdminGetAccountAccess( const wxString& nick );
    void AdminChangeAccountAccess( const wxString& nick, const wxString& accesscode );
    void AdminSetBotMode( const wxString& nick, bool isbot );

    void HostBattle( BattleOptions bo, const wxString& password = _T("") );
    void JoinBattle( const int& battleid, const wxString& password = _T("") );
    void LeaveBattle( const int& battleid );
    void SendMyBattleStatus( UserBattleStatus& bs );
    void SendMyUserStatus();

    void ForceSide( int battleid, User& user, int side );
    void ForceTeam( int battleid, User& user, int team );
    void ForceAlly( int battleid, User& user, int ally );
    void ForceColour( int battleid, User& user, const wxColour& col );
    void ForceSpectator( int battleid, User& user, bool spectator );
    void BattleKickPlayer( int battleid, User& user );
    void SetHandicap( int battleid, User& user, int handicap);

    void AddBot( int battleid, const wxString& nick, UserBattleStatus& status );
    void RemoveBot( int battleid, User& bot );
    void UpdateBot( int battleid, User& bot, UserBattleStatus& status );

    void StartHostedBattle();
    void SendHostInfo( HostInfo update );
    void SendHostInfo( const wxString& Tag );
    void SendUserPosition( const User& user );

    void SendRaw( const wxString& raw );

    void RequestInGameTime( const wxString& nick );

    void SendUdpSourcePort( int udpport );
    void SendNATHelperInfos( const wxString& username, const wxString& ip, int port );

    Battle* GetCurrentBattle();

    void RequestChannels();
    // TASServer specific functions
    void ExecuteCommand( const wxString& in );
    void ExecuteCommand( const wxString& cmd, const wxString& inparams, int replyid = -1 );

    void HandlePong( int replyid );

    void OnConnected( Socket* sock );
    void OnDisconnected( Socket* sock );
    void OnDataReceived( Socket* sock );

    bool IsPasswordHash( const wxString& pass )  const;
    wxString GetPasswordHash( const wxString& pass ) const;

    int TestOpenPort( unsigned int port ) const;

    void SendScriptToProxy( const wxString& script );

    void SendScriptToClients( const wxString& script );

	void SetRelayIngamePassword( const User& user );

    void RequestSpringUpdate();

    wxArrayString GetRelayHostList() ;

  protected:

    //! @brief Struct used internally by the TASServer class to calculate ping roundtimes.
    struct TASPingListItem {
      int id;
      wxLongLong t;
    };

	PingThread* m_ping_thread;
    CRC m_crc;

	IServerEvents* m_se;
    double m_ser_ver;

    wxString m_last_denied;
    bool m_connected;
    bool m_online;
    bool m_debug_dont_catch;
    bool m_id_transmission;
    bool m_redirecting;
    wxString m_buffer;
    time_t m_last_udp_ping;
    time_t m_last_net_packet;
	MutexWrapper<unsigned int> m_last_id;
	unsigned int& GetLastID()
	{
		ScopedLocker<unsigned int> l_last_id(m_last_id);
		return l_last_id.Get();
	}

	typedef std::list<TASPingListItem> PingList;
	MutexWrapper<PingList> m_pinglist;

	PingList& GetPingList()
	{
		ScopedLocker<PingList> l_pinglist(m_pinglist);
		return l_pinglist.Get();
	}

    unsigned long m_udp_private_port;
    unsigned long m_nat_helper_port;

    int m_battle_id;

    bool m_server_lanmode;
    unsigned int m_account_id_count;

    wxString m_agreement;

    wxString m_addr;
    wxString m_delayed_open_command;

    bool m_do_finalize_join_battle;
    int m_finalize_join_battle_id;
    wxString m_finalize_join_battle_pw;
    bool m_token_transmission;

    void FinalizeJoinBattle();

    void SendCmd( const wxString& command, const wxString& param = _T("") );
    void RelayCmd( const wxString& command, const wxString& param = _T("") );
		void FillAliasMap();

    wxString m_current_chan_name_mutelist;

    wxArrayString m_relay_host_manager_list;
};

#endif // SPRINGLOBBY_HEADERGUARD_TASSERVER_H

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

