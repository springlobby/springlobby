/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "serverselector.h"
#include <assert.h>
#include <lslutils/globalsmanager.h>

ServerSelector::ServerSelector()
	: m_serv(0)
{}

bool ServerSelector::IsServerAvailible() const
{
	return (bool)(m_serv);
}

IServer& ServerSelector::GetServer()
{
	assert( m_serv != 0);
	return *m_serv;
}

const IServer& ServerSelector::GetServer() const
{
	assert( m_serv != 0);
	return *m_serv;
}

void ServerSelector::SetCurrentServer(IServer* server)
{
	m_serv = server;
	assert( m_serv != 0);
}

ServerSelector& serverSelector()
{
	static LSL::Util::LineInfo<ServerSelector> m( AT );
	static LSL::Util::GlobalObjectHolder<ServerSelector, LSL::Util::LineInfo<ServerSelector> > m_selector( m );
	return m_selector;
}
