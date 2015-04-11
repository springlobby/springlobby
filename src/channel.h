/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_CHANNEL_H
#define SPRINGLOBBY_HEADERGUARD_CHANNEL_H

#include "userlist.h"
#include <set>
#include <wx/regex.h>
#include "utils/mixins.h"

class IServer;
class ChatPanel;

struct UiChannelData
{
	UiChannelData()
	    : panel(0)
	{
	}

	ChatPanel* panel;
};

class Channel : public UserList, public SL::NonCopyable
{
public:
	UiChannelData uidata;

	//Channel(): m_serv(0),m_userdata(0) {}
	Channel(IServer& serv);
	virtual ~Channel();

	IServer& GetServer()
	{
		return m_serv;
	}

	void SetName(const std::string& name);
	std::string GetName() const;
	User& GetMe();

	// filtering functions
	void CheckBanned(const std::string& name);
	bool IsBanned(const std::string& name);

	// Channel Functions
	void Say(const std::string& message);
	void DoAction(const std::string& action);
	void Leave();
	void Rejoin();

	void Said(User& who, const std::string& message);

	void DidAction(User& who, const std::string& action);

	void Left(User& who, const std::string& reason);
	void Joined(User& who);

	void OnChannelJoin(User& who);

	void SetTopic(const std::string& topic, const std::string& who);
	std::string GetTopic();
	std::string GetTopicSetBy();

	bool ExecuteSayCommand(const std::string& in);

	std::string GetPassword() const;
	void SetPassword(const std::string& pw);
	bool IsSubscribed(); //subscribed to channel history?

private:
	IServer& m_serv;

	std::set<std::string> m_banned_users;

	bool m_do_ban_regex;
	wxRegEx m_ban_regex;

	bool m_do_unban_regex;
	wxRegEx m_unban_regex;

	std::string m_ban_regex_msg;

	std::string m_topic;
	std::string m_topic_nick;
	std::string m_name;

	std::string m_password;

	void AddUser(User& user);
	void RemoveUser(const std::string& nick);
};

#endif // SPRINGLOBBY_HEADERGUARD_CHANNEL_H
