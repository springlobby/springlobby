/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

/**
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

DO NOT CHANGE THIS FILE!

this file is deprecated and will be replaced with

lsl/networking/tasserver.h

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
**/


#ifndef SPRINGLOBBY_HEADERGUARD_TASSERVER_H
#define SPRINGLOBBY_HEADERGUARD_TASSERVER_H

#include <string>
#include <wx/timer.h>
#include <list>

#include "iserver.h"
#include "inetclass.h"
#include "utils/crc.h"

const unsigned int FIRST_UDP_SOURCEPORT = 8300;

class Ui;
class Socket;
class User;
struct UserBattleStatus;
class IServerEvents;
class PingThread;

//! @brief TASServer protocol implementation.
class TASServer : public IServer, public iNetClass, public wxTimer
{
public:
	TASServer();
	~TASServer();

	// Overloaded functions from Server
	bool ExecuteSayCommand(const std::string& cmd);

	void Register(const std::string& servername, const std::string& host, const int port, const std::string& nick, const std::string& password);
	void AcceptAgreement();

	void Connect(const std::string& servername, const std::string& addr, const int port);
	void Disconnect();
	bool IsConnected();

	void Login();
	void Logout();
	bool IsOnline() const;

	void Ping();

	const User& GetMe() const;
	User& GetMe();

	void JoinChannel(const std::string& channel, const std::string& key);
	void PartChannel(const std::string& channel);

	void DoActionChannel(const std::string& channel, const std::string& msg);
	void SayChannel(const std::string& channel, const std::string& msg);

	void DoActionPrivate(const std::string& nick, const std::string& msg);
	void SayPrivate(const std::string& nick, const std::string& msg);

	void SayBattle(int battleid, const std::string& msg);
	void DoActionBattle(int battleid, const std::string& msg);

	void Ring(const std::string& nick);

	void ModeratorSetChannelTopic(const std::string& channel, const std::string& topic);
	void ModeratorSetChannelKey(const std::string& channel, const std::string& key);
	void ModeratorMute(const std::string& channel, const std::string& nick, int duration, bool byip);
	void ModeratorUnmute(const std::string& channel, const std::string& nick);
	void ModeratorKick(const std::string& channel, const std::string& reason);
	void ModeratorBan(const std::string& nick, bool byip);
	void ModeratorUnban(const std::string& nick);
	void ModeratorGetIP(const std::string& nick);
	void ModeratorGetLastLogin(const std::string& nick);
	void ModeratorGetLastIP(const std::string& nick);
	void ModeratorFindByIP(const std::string& ipadress);

	void AdminGetAccountAccess(const std::string& nick);
	void AdminChangeAccountAccess(const std::string& nick, const std::string& accesscode);
	void AdminSetBotMode(const std::string& nick, bool isbot);

	void HostBattle(const BattleOptions& bo, const std::string& password = "");
	void JoinBattle(const int& battleid, const std::string& password = "");
	void LeaveBattle(const int& battleid);
	void SendMyBattleStatus(UserBattleStatus& bs);
	void SendMyUserStatus(const UserStatus& us);

	void ForceSide(int battleid, User& user, int side);
	void ForceTeam(int battleid, User& user, int team);
	void ForceAlly(int battleid, User& user, int ally);
	void ForceColour(int battleid, User& user, const LSL::lslColor& col);
	void ForceSpectator(int battleid, User& user, bool spectator);
	void BattleKickPlayer(int battleid, User& user);
	void SetHandicap(int battleid, User& user, int handicap);

	void AddBot(int battleid, const std::string& nick, UserBattleStatus& status);
	void RemoveBot(int battleid, User& bot);
	void UpdateBot(int battleid, User& bot, UserBattleStatus& status);

	void StartHostedBattle();
	void SendHostInfo(HostInfo update);
	void SendHostInfo(const std::string& Tag);
	void SendUserPosition(const User& user);

	void RequestInGameTime(const std::string& nick);

	void SendUdpSourcePort(int udpport);
	void SendNATHelperInfos(const std::string& username, const std::string& ip, int port);

	IBattle* GetCurrentBattle();

	void RequestChannels();
	// TASServer specific functions
	void ExecuteCommand(const std::string& in);
	void ExecuteCommand(const std::string& cmd, const std::string& inparams, int replyid = -1);

	void HandlePong(int replyid);

	void OnError(const std::string& err);

	bool IsPasswordHash(const std::string& pass) const;
	std::string GetPasswordHash(const std::string& pass) const;

	int TestOpenPort(unsigned int port) const;

	void SetRelayIngamePassword(const User& user);

	LSL::StringVector GetRelayHostList();
	virtual const ServerEvents* serverEvents() const
	{
		return m_se;
	}
	LSL::StringSet GetSubscriptions()
	{
		return m_subscriptions;
	}

private:
	void OnConnected();
	void OnDataReceived();
	void OnDisconnected();

	void UDPPing(); /// used for nat travelsal
	/// generic udp "ping" function
	/// return value: actual source port which was used. May differ from src_port
	/// 0 if udp ping failed
	unsigned int UdpPing(unsigned int src_port, const std::string& target, unsigned int target_port, const std::string& message);
	/// specialized udp ping functions
	void UdpPingTheServer(const std::string& message); /// used for nat travelsal. pings the server.
	void UdpPingAllClients();			   /// used when hosting with nat holepunching
	void FinalizeJoinBattle();

	void SendCmd(const std::string& command, const std::string& param)
	{
		SendCmd(command, param, false);
	};
	void SendCmd(const std::string& command)
	{
		SendCmd(command, "", false);
	}
	virtual void SendCmd(const std::string& command, const std::string& param, bool relay);


	void RelayCmd(const std::string& command, const std::string& param = "");
	void Notify();

	//! @brief Struct used internally by the TASServer class to calculate ping roundtimes.
	struct TASPingListItem
	{
		int id;
		wxLongLong t;
	};

	CRC m_crc;

	ServerEvents* m_se;
	Socket* m_sock;

	double m_ser_ver;
	LSL::StringVector m_relay_host_manager_list;
	LSL::StringSet m_subscriptions;

	std::string m_last_denied;
	bool m_connected;
	bool m_online;
	bool m_debug_dont_catch;
	bool m_id_transmission;
	bool m_redirecting;
	std::string m_buffer;
	int m_last_udp_ping;
	int m_last_ping;       //time last ping was sent
	int m_last_net_packet; //time last packet was received
	wxLongLong m_lastnotify;
	unsigned int m_last_id;

	std::list<TASPingListItem> m_pinglist;

	unsigned long m_udp_private_port;
	unsigned long m_nat_helper_port;

	int m_battle_id;

	bool m_server_lanmode;
	unsigned int m_account_id_count;

	std::string m_agreement;

	std::string m_addr;
	std::string m_delayed_open_command;

	bool m_do_finalize_join_battle;
	int m_finalize_join_battle_id;
	std::string m_finalize_join_battle_pw;

	std::string m_current_chan_name_mutelist;
	std::string m_relay_host_bot;
	std::string m_relay_host_manager;
};

#endif // SPRINGLOBBY_HEADERGUARD_TASSERVER_H
