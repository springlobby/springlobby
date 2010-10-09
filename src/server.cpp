/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: Server
//

#include <stdexcept>
#include <wx/log.h>

#include "server.h"
#include "socket.h"
#include "battle.h"
#include "channel/channel.h"
#include "user.h"
#include "ui.h"
#include "utils/debug.h"
#include "utils/conversion.h"
#include "chatpanel.h"

Server::Server():
battles_iter(new BattleList_Iter(&m_battles)),
m_keepalive(15)
{
	m_sock = new Socket( *this, false, false );
}

Server::~Server()
{
	delete battles_iter;
  if(uidata.panel)uidata.panel->SetServer(NULL);
  delete m_sock;
}


User& Server::GetUser( const wxString& nickname ) const
{
  return m_users.GetUser( nickname );
}


bool Server::UserExists( const wxString& nickname ) const
{
  return m_users.UserExists( nickname );
}


Channel& Server::GetChannel( const wxString& name )
{
  return m_channels.GetChannel( name );
}


int Server::GetNumChannels() const
{
  return m_channels.GetNumChannels();
}


Channel& Server::GetChannel( const int& index )
{
  return m_channels.GetChannel( index );
}


bool Server::ChannelExists( const wxString& name ) const
{
  return m_channels.ChannelExists( name );
}


Battle& Server::GetBattle( const int& battleid )
{
  return battles_iter->GetBattle( battleid );
}


bool Server::BattleExists( const int& battleid ) const
{
  return battles_iter->BattleExists( battleid );
}



User& Server::_AddUser( const wxString& user )
{
  if ( m_users.UserExists( user ) ) return m_users.GetUser( user );
  User* u = new User( user, *this );
  m_users.AddUser( *u );
  return *u;
}


void Server::_RemoveUser( const wxString& nickname )
{
  try{
    User* u = &m_users.GetUser( nickname );
    m_users.RemoveUser( nickname );
    int numchannels = m_channels.GetNumChannels();
    for ( int i = 0; i < numchannels; i++ )
    {
    	Channel& chan = m_channels.GetChannel( i );
    	if ( chan.UserExists( nickname ) ) chan.Left( *u, _T("server idiocy") );
    }
    delete u;
  }catch(std::runtime_error){
  }
}


Channel& Server::_AddChannel( const wxString& chan )
{
  if ( m_channels.ChannelExists( chan ) ) return m_channels.GetChannel( chan );
  Channel* c = new Channel( *this );
  c->SetName( chan );

  m_channels.AddChannel( *c );
  return *c;
}


void Server::_RemoveChannel( const wxString& name )
{
  Channel* c = &m_channels.GetChannel( name );
  m_channels.RemoveChannel( name );
  ASSERT_LOGIC( c != 0, _T("Server::_RemoveChannel(\"") + name + _T("\"): GetChannel returned NULL pointer"));
  delete c;
}

Battle& Server::_AddBattle( const int& id )
{
  if ( battles_iter->BattleExists( id ) ) return battles_iter->GetBattle( id );
  Battle* b = new Battle( *this, id );

  m_battles.AddBattle( *b );
  return *b;
}


void Server::_RemoveBattle( const int& id )
{
  Battle* b = &battles_iter->GetBattle( id );
  m_battles.RemoveBattle( id );
  ASSERT_LOGIC( b != 0, _T("Server::_RemoveBattle(): GetBattle returned NULL pointer"));
  delete b;
}


void Server::OnDisconnected()
{
  while ( battles_iter->GetNumBattles() > 0 )
  {
    battles_iter->IteratorBegin();
    Battle* b = battles_iter->GetBattle();
    if (b!=0)
    {
        m_battles.RemoveBattle( b->GetBattleId() );
        delete b;
    }
  }
  while ( m_users.GetNumUsers() > 0 )
  {
    try
    {
			User* u = &m_users.GetUser( 0 );
			m_users.RemoveUser( u->GetNick() );
			delete u;
    }
    catch(std::runtime_error)
    {
    }
  }
  while ( m_channels.GetNumChannels() > 0 )
  {
    Channel* c = &m_channels.GetChannel( 0 );
    m_channels.RemoveChannel( c->GetName() );
    delete c;
  }

}

void Server::RequestSpringUpdate()
{
}

wxArrayString Server::GetRelayHostList()
{
	if ( UserExists( _T("RelayHostManagerList") ) ) SayPrivate( _T("RelayHostManagerList"), _T("!listmanagers") );
	wxArrayString ret;
	for ( unsigned int i = 0; i < m_relay_host_manager_list.GetCount(); i++ )
	{
		try
		{
			User& manager = GetUser( m_relay_host_manager_list[i] );
			if ( manager.Status().in_game ) continue; // skip the manager is not connected or reports it's ingame ( no slots available ), or it's away ( functionality disabled )
			if ( manager.Status().away ) continue;
			ret.Add( m_relay_host_manager_list[i] );
		}
		catch(...){}
	}
	return ret;
}
