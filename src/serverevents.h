/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_SERVEREVENTS_H
#define SPRINGLOBBY_HEADERGUARD_SERVEREVENTS_H

#include <wx/longlong.h>
#include "ibattle.h"

struct UserStatus;
struct UserBattleStatus;
class IServer;
typedef int Sockerror;
typedef int Protocolerror;

struct MessageSpamCheck
{
	time_t lastmessage;
	unsigned int count;
};

//! @brief Class that implements server event behaviour.
class ServerEvents
{
public:
	ServerEvents(IServer& serv)
		: m_serv(serv)
	{
	}
	~ServerEvents()
	{
	}

	// Uicontrol interface

	void OnConnected(const std::string& server_name, const std::string& server_ver, bool supported, const std::string& server_spring_ver, bool lanmode);
	void OnDisconnected(bool wasonline);

	void OnLogin();
	void OnLoginInfoComplete();

	void OnUnknownCommand(const std::string& command, const std::string& params);
	void OnSocketError(const Sockerror& error);
	void OnProtocolError(const Protocolerror error);
	void OnMotd(const std::string& msg);
	void OnPong(wxLongLong ping_time);

	void OnNewUser(const std::string& nick, const std::string& country, int id, const std::string& clientAgent);
	void OnUserStatus(const std::string& nick, UserStatus status);
	void OnUserQuit(const std::string& nick);

	void OnBattleOpened(int id, BattleType type, NatType nat, const std::string& nick,
				const std::string& host, int port, int maxplayers,
				bool haspass, int rank, const std::string& maphash, const std::string& engineName, const std::string& engineVersion, const std::string& map,
				const std::string& title, const std::string& mod,
				const std::string& channelName);

	void OnUserJoinedBattle(int battleid, const std::string& nick, const std::string& userScriptPassword);
	void OnUserLeftBattle(int battleid, const std::string& nick);
	void OnBattleInfoUpdated(int battleid, int spectators, bool locked, const std::string& maphash, const std::string& map);
	void OnSetBattleInfo(int battleid, const std::string& param, const std::string& value);
	void OnUnsetBattleInfo(int battleid, const std::string& param);
	void OnBattleInfoUpdated(int battleid);
	void OnBattleClosed(int battleid);

	void OnJoinedBattle(int battleid, const std::string& hash);
	void OnHostedBattle(int battleid);

	void OnStartHostedBattle(int battleid);
	void OnClientBattleStatus(int battleid, const std::string& nick, UserBattleStatus status);

	void OnBattleStartRectAdd(int battleid, int allyno, int left, int top, int right, int bottom);
	void OnBattleStartRectRemove(int battleid, int allyno);

	void OnBattleAddBot(int battleid, const std::string& nick, UserBattleStatus status);
	void OnBattleUpdateBot(int battleid, const std::string& nick, UserBattleStatus status);
	void OnBattleRemoveBot(int battleid, const std::string& nick);

	void OnBattleDisableUnit(int battleid, const std::string& unitname, int count = 0);
	void OnBattleEnableUnit(int battleid, const std::string& unitname);
	void OnBattleEnableAllUnits(int battleid);

	void OnJoinChannelResult(bool success, const std::string& channel, const std::string& reason);

	void OnChannelSaid(const std::string& channel, const std::string& who, const std::string& message);
	void OnChannelJoin(const std::string& channel, const std::string& who);
	void OnChannelPart(const std::string& channel, const std::string& who, const std::string& message);
	void OnChannelTopic(const std::string& channel, const std::string& who, const std::string& message, int when);
	void OnChannelAction(const std::string& channel, const std::string& who, const std::string& action);
	void OnChannelList(const std::string& channel, const int& numusers, const std::string& topic);
	void OnUserJoinChannel(const std::string& channel, const std::string& who);

	void OnPrivateMessage(User& chan, User& who, const std::string& message);
	void OnPrivateMessageEx(User& chan, User& who, const std::string& action);

	void OnRequestBattleStatus(int battleid);
	void OnBattleSaid(int battleid, const std::string& nick, const std::string& msg);
	void OnBattleAction(int battleid, const std::string& nick, const std::string& msg);

	void OnAcceptAgreement(const std::string& agreement);

	void OnRing(const std::string& from);

	void OnServerMessage(const std::string& message);
	void OnServerBroadcast(const std::string& message);
	void OnServerMessageBox(const std::string& message);
	void OnChannelMessage(const std::string& channel, const std::string& msg);

	void OnJoinedFrom(const std::string& channel, const std::string& nick);
	void OnLeftFrom(const std::string& channel, const std::string& nick);
	void OnSaidFrom(const std::string& channel, const std::string& nick, const std::string& msg);


	void OnHostExternalUdpPort(const unsigned int udpport);

	void OnMyExternalUdpSourcePort(const unsigned int udpport);
	void OnMyInternalUdpSourcePort(const unsigned int udpport);

	void OnClientIPPort(const std::string& username, const std::string& ip, unsigned int udpport);

	void OnKickedFromBattle();

	void OnRedirect(const std::string& address, unsigned int port, const std::string& CurrentNick, const std::string& CurrentPassword);

	void OnMutelistBegin(const std::string& channel);
	void OnMutelistItem(const std::string& channel, const std::string& mutee, const std::string& description);
	void OnMutelistEnd(const std::string& channel);

	void OnScriptStart(int battleid);
	void OnScriptLine(int battleid, const std::string& line);
	void OnScriptEnd(int battleid);

	void OnForceJoinBattle(int battleid, const std::string& line);
	void RegistrationAccepted(const std::string& user, const std::string& pass);
	void RegistrationDenied(const std::string& reason);
	void OnLoginDenied(const std::string& reason);
	void OnInvalidFingerprintReceived(const std::string& fingerprint, const std::string& expected_fingerprint);

	/// use this function to check spam from clients and autokick from the battle
	void AutoCheckCommandSpam(IBattle& battle, User& nick);

private:
	IServer& m_serv;
	std::map<std::string, MessageSpamCheck> m_spam_check;
	std::string m_savepath;
};

#endif // SPRINGLOBBY_HEADERGUARD_SERVEREVENTS_H
