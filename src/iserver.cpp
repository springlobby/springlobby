/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

//
// Class: Server
//

#include <stdexcept>
#include <wx/log.h>

#include "iserver.h"
#include "battle.h"
#include "channel.h"
#include "user.h"
#include "utils/conversion.h"
#include "gui/chatpanel.h"
#include "log.h"

IServer::IServer()
	: panel(nullptr)
    , battles_iter(new BattleList_Iter(&m_battles))
{
}

IServer::~IServer()
{
	if (panel != nullptr) {
		ChatPanel* tmp = panel;
		panel = nullptr;
		tmp->SetServer(NULL);
	}
	Reset();
	delete battles_iter;
}


User& IServer::GetUser(const std::string& nickname) const
{
	ASSERT_EXCEPTION(!nickname.empty(), _T("GetUser with empty nickname called"));
	return m_users.GetUser(nickname);
}


bool IServer::UserExists(const std::string& nickname) const
{
	return m_users.UserExists(nickname);
}


Channel& IServer::GetChannel(const std::string& name)
{
	ASSERT_EXCEPTION(!name.empty(), _T("GetChannel with empty nickname called"));
	return m_channels.GetChannel(name);
}


int IServer::GetNumChannels() const
{
	return m_channels.GetNumChannels();
}


Channel& IServer::GetChannel(const int& index)
{
	return m_channels.GetChannel(index);
}


bool IServer::ChannelExists(const std::string& name) const
{
	return m_channels.ChannelExists(name);
}


IBattle& IServer::GetBattle(const int& battleid)
{
	ASSERT_EXCEPTION(BattleExists(battleid), _T("Battle doesn't exist!"));
	return battles_iter->GetBattle(battleid);
}


bool IServer::BattleExists(const int& battleid) const
{
	return battles_iter->BattleExists(battleid);
}


User& IServer::_AddUser(const std::string& user)
{
	if (m_users.UserExists(user))
		return m_users.GetUser(user);
	User* u = new User(user, *this);
	m_users.AddUser(*u);
	return *u;
}


void IServer::_RemoveUser(const std::string& nickname)
{
	try {
		User* u = &m_users.GetUser(nickname);
		m_users.RemoveUser(nickname);
		int numchannels = m_channels.GetNumChannels();
		for (int i = 0; i < numchannels; i++) {
			Channel& chan = m_channels.GetChannel(i);
			if (chan.UserExists(nickname))
				chan.Left(*u, "server idiocy");
		}
		delete u;
	} catch (std::runtime_error) {
	}
}


Channel& IServer::_AddChannel(const std::string& chan)
{
	if (m_channels.ChannelExists(chan))
		return m_channels.GetChannel(chan);
	Channel* c = new Channel(*this);
	c->SetName(chan);

	m_channels.AddChannel(*c);
	return *c;
}


/*
void IServer::_RemoveChannel(const std::string& name)
{
	Channel* c = &m_channels.GetChannel(name);
	m_channels.RemoveChannel(name);
	ASSERT_LOGIC(c != 0, "IServer::_RemoveChannel(\"" + name + "\"): GetChannel returned NULL pointer");
	delete c;
}
*/

IBattle& IServer::_AddBattle(const int& id)
{
	if (battles_iter->BattleExists(id))
		return battles_iter->GetBattle(id);
	IBattle* b = new Battle(*this, id);

	m_battles.AddBattle(*b);
	return *b;
}


void IServer::_RemoveBattle(const int& id)
{
	IBattle* b = &battles_iter->GetBattle(id);
	m_battles.RemoveBattle(id);
	ASSERT_LOGIC(b != 0, "IServer::_RemoveBattle(): GetBattle returned NULL pointer");
	delete b;
}


void IServer::Reset()
{
	m_users.Nullify();
	while (m_users.GetNumUsers() > 0) {
		try {
			User* u = &m_users.GetUser(0);
			m_users.RemoveUser(u->GetNick());
			delete u;
		} catch (std::runtime_error) {
		}
	}

	while (battles_iter->GetNumBattles() > 0) {
		battles_iter->IteratorBegin();
		IBattle* b = battles_iter->GetBattle();
		if (b != 0) {
			m_battles.RemoveBattle(b->GetBattleId());
			delete b;
		}
	}
	while (m_channels.GetNumChannels() > 0) {
		Channel* c = &m_channels.GetChannel(0);
		m_channels.RemoveChannel(c->GetName());
		delete c;
	}
}
