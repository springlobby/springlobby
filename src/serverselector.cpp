/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "serverselector.h"

#include <assert.h>
#include <lslutils/globalsmanager.h>

ServerSelector::ServerSelector()
    : m_serv(NULL)
{
}

ServerSelector::~ServerSelector()
{
	m_serv = NULL;
}

bool ServerSelector::IsServerAvailible() const
{
	return m_serv != NULL;
}

IServer& ServerSelector::GetServer()
{
	assert(m_serv != 0);
	return *m_serv;
}

const IServer& ServerSelector::GetServer() const
{
	assert(m_serv != 0);
	return *m_serv;
}

void ServerSelector::SetCurrentServer(IServer* server)
{
	assert(server != 0);
	m_serv = server;
}

ServerSelector& serverSelector()
{
	static LSL::Util::LineInfo<ServerSelector> m(AT);
	static LSL::Util::GlobalObjectHolder<ServerSelector, LSL::Util::LineInfo<ServerSelector> > m_selector(m);
	return m_selector;
}
