/* This file is part of the Springlobby (GPL v2 or later), see COPYING */
//
// Class: Channel
//

#include "channel.h"
#include "gui/ui.h"
#include "iserver.h"
#include "user.h"
#include "utils/tasutil.h"
#include "utils/version.h"
#include <wx/regex.h>
#include <wx/log.h>
#include "gui/chatpanel.h"
#include "utils/conversion.h"
#include "log.h"

Channel::Channel(IServer& serv)
    : uidata(nullptr)
    , m_serv(serv)
    , m_do_ban_regex(false)
    , m_do_unban_regex(false)
{
}

Channel::~Channel()
{
	if (uidata) {
		uidata->SetChannel(nullptr);
		uidata = nullptr;
	}
}

void Channel::SetName(const std::string& name)
{
	m_name = name;
}


std::string Channel::GetName() const
{
	return m_name;
}


User& Channel::GetMe()
{
	return m_serv.GetMe();
}


void Channel::Said(User& who, const std::string& message)
{
	slLogDebugFunc("");
	if (uidata == nullptr) {
		wxLogError(_T("OnChannelSaid: ud->panel NULL"));
		return;
	}
	uidata->Said(TowxString(who.GetNick()), TowxString(message));
}


void Channel::Say(const std::string& message)
{
	slLogDebugFunc("");
	m_serv.SayChannel(m_name, message);
}


void Channel::DidAction(User& who, const std::string& action)
{
	slLogDebugFunc("");
	if (uidata == nullptr) {
		wxLogError(_T("OnChannelDidAction: ud->panel NULL"));
		return;
	}
	uidata->DidAction(TowxString(who.GetNick()), TowxString(action));
}


void Channel::DoAction(const std::string& action)
{
	slLogDebugFunc("");
	m_serv.DoActionChannel(m_name, action);
}


void Channel::Left(User& who, const std::string& reason)
{
	RemoveUser(who.GetNick());
	//wxLogDebugFunc( wxEmptyString );
	if (uidata == nullptr) {
		wxLogWarning(_T("OnUserLeftChannel: ud->panel NULL"));
		return;
	}
	uidata->Parted(who, TowxString(reason));
}


void Channel::Leave()
{
	m_serv.PartChannel(m_name);
}

void Channel::Rejoin()
{
	m_serv.JoinChannel(m_name, m_password);
}


void Channel::Joined(User& who)
{
	AddUser(who);
	if (uidata == nullptr) {
		wxLogError(_T("OnUserJoinedChannel: ud->panel NULL"));
		return;
	}
	uidata->Joined(who);
}


void Channel::OnChannelJoin(User& who)
{
	AddUser(who);
	//wxLogDebugFunc( wxEmptyString );
	if (uidata == 0) {
		wxLogError(_T("OnChannelJoin: ud->panel NULL"));
		return;
	}
	uidata->OnChannelJoin(who);
}


void Channel::SetTopic(const std::string& topic, const std::string& who)
{
	m_topic = topic;
	m_topic_nick = who;

	slLogDebugFunc("");
	if (uidata == nullptr) {
		wxLogError(_T("OnChannelTopic: ud->panel NULL"));
		return;
	}
	uidata->SetTopic(TowxString(who), TowxString(topic));
}

std::string Channel::GetTopicSetBy()
{
	return m_topic_nick;
}


std::string Channel::GetTopic()
{
	return m_topic;
}


void Channel::AddUser(User& user)
{
	UserList::AddUser(user);
	CheckBanned(user.GetNick());
}

void Channel::CheckBanned(const std::string& name)
{
	if (name == "ChanServ")
		return;
	if (m_banned_users.count(name) > 0) {
		m_serv.SayPrivate("ChanServ", "!kick #" + GetName() + std::string(" ") + name);
	}
	if (m_do_ban_regex && m_ban_regex.IsValid()) {
		if (m_ban_regex.Matches(TowxString(name)) && !(m_do_unban_regex && m_unban_regex.IsValid() && m_unban_regex.Matches(TowxString(name)))) {
			m_serv.SayPrivate("ChanServ", "!kick #" + GetName() + std::string(" ") + name);
			if (!m_ban_regex_msg.empty())
				m_serv.SayPrivate(name, m_ban_regex_msg);
		}
	}
}

bool Channel::IsBanned(const std::string& name)
{
	if (name == "ChanServ")
		return false;
	if (m_banned_users.count(name) > 0)
		return true;
	if (m_do_ban_regex && m_ban_regex.IsValid()) {
		if (m_ban_regex.Matches(TowxString(name)) && !(m_do_unban_regex && m_unban_regex.IsValid() && m_unban_regex.Matches(TowxString(name))))
			return true;
	}
	return false;
}


void Channel::RemoveUser(const std::string& nick)
{
	UserList::RemoveUser(nick);
}


bool Channel::ExecuteSayCommand(const std::string& in)
{
	if (in.length() == 0)
		return true;

	if (in[0] != '/')
		return false;

	wxString subcmd = TowxString(in).BeforeFirst(' ').Lower();
	wxString params = TowxString(in).AfterFirst(' ');

	wxString cmdline = TowxString(in);
	wxString param = TowxString(GetWordParam(cmdline));
	if (param == _T("/me")) {
		DoAction(STD_STRING(cmdline));
		return true;
	} else if ((in == "/part") || (in == "/p")) {
		Leave();
		uidata = nullptr;
		return true;
	} else if (param == _T("/sayver")) {
		//!this instance is not replaced with GetAppname for sake of help/debug online
		DoAction("is using SpringLobby v" + getSpringlobbyVersion());
		return true;
	} else if (subcmd == _T("/userban")) {
		m_banned_users.insert(STD_STRING(params));
		m_serv.SayPrivate("ChanServ", stdprintf("!kick #%s %s", GetName().c_str(), STD_STRING(params).c_str()));
		return true;
	} else if (subcmd == _T("/userunban")) {
		m_banned_users.erase(STD_STRING(params));
		return true;
	} else if (subcmd == _T("/banregex")) {
		ui().OnChannelMessage(*this, "/banregex " + STD_STRING(params));
		m_do_ban_regex = !params.empty();
		if (m_do_ban_regex) {
#ifdef wxHAS_REGEX_ADVANCED
			m_ban_regex.Compile(params, wxRE_ADVANCED);
#else
			m_ban_regex.Compile(params, wxRE_EXTENDED);
#endif
			if (!m_ban_regex.IsValid())
				ui().OnChannelMessage(*this, "Invalid regular expression");
		}
		return true;
	} else if (subcmd == _T("/unbanregex")) {
		ui().OnChannelMessage(*this, "/unbanregex " + STD_STRING(params));
		m_do_unban_regex = !params.empty();
		if (m_do_unban_regex) {
#ifdef wxHAS_REGEX_ADVANCED
			m_unban_regex.Compile(params, wxRE_ADVANCED);
#else
			m_unban_regex.Compile(params, wxRE_EXTENDED);
#endif
			if (!m_unban_regex.IsValid())
				ui().OnChannelMessage(*this, "Invalid regular expression");
		}
		return true;
	} else if (subcmd == _T("/checkban")) {
		if (IsBanned(STD_STRING(params))) {
			ui().OnChannelMessage(*this, STD_STRING(params) + " is banned");
		} else {
			ui().OnChannelMessage(*this, STD_STRING(params) + " is not banned");
		}
		return true;
	}


	else if (subcmd == _T("/banregexmsg")) {
		ui().OnChannelMessage(*this, "/banregexmsg " + STD_STRING(params));
		m_ban_regex_msg = STD_STRING(params);
		return true;
	}

	return false;
}


std::string Channel::GetPassword() const
{
	return m_password;
}


void Channel::SetPassword(const std::string& pw)
{
	m_password = pw;
}

bool Channel::IsSubscribed()
{
	const LSL::StringSet subscriptions = m_serv.GetSubscriptions();
	return (subscriptions.find(m_name) != subscriptions.end());
}
