/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

/**
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

TODO: remove all wx stuff & merge / move to

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
	bool ExecuteSayCommand(const std::string& cmd) override;

	void Register(const ServerLoginInfo& server) override;
	void AcceptAgreement() override;

	void Disconnect() override;
	bool IsConnected() override;

	void Login() override;
	void Logout() override;
	bool IsOnline() const override; //returns true when user is successfully logged in

	void Ping() override;

	const User& GetMe() const override;
	User& GetMe() override;

	void JoinChannel(const std::string& channel, const std::string& key) override;
	void PartChannel(const std::string& channel) override;

	void DoActionChannel(const std::string& channel, const std::string& msg) override;
	void SayChannel(const std::string& channel, const std::string& msg) override;

	void DoActionPrivate(const std::string& nick, const std::string& msg) override;
	void SayPrivate(const std::string& nick, const std::string& msg) override;

	void SayBattle(int battleid, const std::string& msg) override;
	void DoActionBattle(int battleid, const std::string& msg) override;

	void Ring(const std::string& nick) override;

	void ModeratorSetChannelTopic(const std::string& channel, const std::string& topic) override;
	void ModeratorSetChannelKey(const std::string& channel, const std::string& key) override;
	void ModeratorMute(const std::string& channel, const std::string& nick, int duration, bool byip) override;
	void ModeratorUnmute(const std::string& channel, const std::string& nick) override;
	void ModeratorKick(const std::string& channel, const std::string& reason) override;
	void ModeratorBan(const std::string& nick, bool byip) override;
	void ModeratorUnban(const std::string& nick) override;
	void ModeratorGetIP(const std::string& nick) override;
	void ModeratorGetLastLogin(const std::string& nick) override;
	void ModeratorGetLastIP(const std::string& nick) override;
	void ModeratorFindByIP(const std::string& ipadress) override;

	void AdminGetAccountAccess(const std::string& nick) override;
	void AdminChangeAccountAccess(const std::string& nick, const std::string& accesscode) override;
	void AdminSetBotMode(const std::string& nick, bool isbot) override;

	void HostBattle(const BattleOptions& bo, const std::string& password = "") override;
	void JoinBattle(const int& battleid, const std::string& password = "") override;
	void LeaveBattle(const int& battleid) override;
	void SendMyBattleStatus(UserBattleStatus& bs) override;
	void SendMyUserStatus(const UserStatus& us) override;

	void ForceSide(int battleid, User& user, int side) override;
	void ForceTeam(int battleid, User& user, int team) override;
	void ForceAlly(int battleid, User& user, int ally) override;
	void ForceColour(int battleid, User& user, const LSL::lslColor& col) override;
	void ForceSpectator(int battleid, User& user, bool spectator) override;
	void BattleKickPlayer(int battleid, User& user) override;
	void SetHandicap(int battleid, User& user, int handicap) override;

	void AddBot(int battleid, const std::string& nick, UserBattleStatus& status) override;
	void RemoveBot(int battleid, User& bot) override;
	void UpdateBot(int battleid, User& bot, UserBattleStatus& status) override;

	void StartHostedBattle() override;
	void SendHostInfo(HostInfo update) override;
	void SendHostInfo(const std::string& Tag) override;
	void SendUserPosition(const User& user) override;

	void RequestInGameTime(const std::string& nick) override;

	void RequestChannels() override;
	LSL::StringVector GetRelayHostList() override;

	virtual void ExecuteCommand(const std::string& in);

	void SendScriptToProxy(const std::string& script) override;

private:
	void Connect(const ServerLoginInfo& server) override;
	void SendUdpSourcePort(int udpport);
	void SendNATHelperInfos(const std::string& username, const std::string& ip, int port);

	IBattle* GetCurrentBattle() override;
	virtual const ServerEvents* serverEvents() const
	{
		return m_se;
	}
	// TASServer specific functions
	void ExecuteCommand(const std::string& cmd, const std::string& inparams, int replyid = -1);

	void HandlePong(int replyid);


	bool IsPasswordHash(const std::string& pass) const override;
	std::string GetPasswordHash(const std::string& pass) const override;

	void SetRelayIngamePassword(const User& user) override;

	void OnConnected() override;
	void OnDisconnected(wxSocketError err) override;
	void OnDataReceived() override;
	void OnError(const std::string& err) override;
	void OnInvalidFingerprintReceived(const std::string& fingerprint, const std::string& expected_fingerprint) override;

	void UDPPing(); /// used for nat travelsal
	/// generic udp "ping" function
	/// return value: actual source port which was used. May differ from src_port
	/// 0 if udp ping failed
	unsigned int UdpPing(unsigned int src_port, const std::string& target, unsigned int target_port, const std::string& message);
	/// specialized udp ping functions
	void UdpPingTheServer(const std::string& message); /// used for nat travelsal. pings the server.
	void UdpPingAllClients();			   /// used when hosting with nat holepunching
	void FinalizeJoinBattle();

	virtual void SendCmd(const std::string& command, const std::string& param) final
	{
		SendCmd(command, param, false);
	}
	virtual void SendCmd(const std::string& command) final
	{
		SendCmd(command, "", false);
	}
	virtual void SendCmd(const std::string& command, const std::string& param, bool relay);


	void RelayCmd(const std::string& command, const std::string& param = "") override;
	void Notify() override;
	bool IsCurrentBattle(int battle_id);
	void ParseJson(const std::string& jsonstr);

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

	std::string m_delayed_open_command;

	bool m_do_finalize_join_battle;
	bool m_do_register;
	int m_finalize_join_battle_id;
	std::string m_finalize_join_battle_pw;

	std::string m_current_chan_name_mutelist;
	std::string m_relay_host_bot;
	std::string m_relay_host_manager;
	std::string m_supported_spring_version;
};

#endif // SPRINGLOBBY_HEADERGUARD_TASSERVER_H
