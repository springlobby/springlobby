/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

//
// Class: Server
//

#include <stdexcept>
#include <wx/log.h>

#include "iserver.h"
#include "socket.h"
#include "battle.h"
#include "channel/channel.h"
#include "user.h"
#include "utils/debug.h"
#include "utils/conversion.h"
#include "chatpanel.h"

IServer::IServer():
battles_iter(new BattleList_Iter(&m_battles)),
m_keepalive(15),
m_pass_hash(false)
{
	m_sock = new Socket( *this, false, false );
}

IServer::~IServer()
{
	delete battles_iter;
	if(uidata.panel)uidata.panel->SetServer(NULL);
	delete m_sock;
	m_sock = NULL;
}


User& IServer::GetUser( const wxString& nickname ) const
{
  return m_users.GetUser( STD_STRING(nickname));
}


bool IServer::UserExists( const wxString& nickname ) const
{
  return m_users.UserExists(STD_STRING(nickname));
}


Channel& IServer::GetChannel( const wxString& name )
{
  return m_channels.GetChannel( STD_STRING(name));
}


int IServer::GetNumChannels() const
{
  return m_channels.GetNumChannels();
}


Channel& IServer::GetChannel( const int& index )
{
  return m_channels.GetChannel( index );
}


bool IServer::ChannelExists( const wxString& name ) const
{
  return m_channels.ChannelExists( STD_STRING(name));
}


IBattle& IServer::GetBattle( const int& battleid )
{
  return battles_iter->GetBattle( battleid );
}


bool IServer::BattleExists( const int& battleid ) const
{
  return battles_iter->BattleExists( battleid );
}



User& IServer::_AddUser( const wxString& user )
{
  if ( m_users.UserExists(STD_STRING(user)) ) return m_users.GetUser(STD_STRING(user));
  User* u = new User( STD_STRING(user), *this );
  m_users.AddUser( *u );
  return *u;
}


void IServer::_RemoveUser( const wxString& nickname )
{
  try{
    User* u = &m_users.GetUser(STD_STRING(nickname));
    m_users.RemoveUser(STD_STRING(nickname));
    int numchannels = m_channels.GetNumChannels();
    for ( int i = 0; i < numchannels; i++ )
    {
    	Channel& chan = m_channels.GetChannel( i );
		if ( chan.UserExists( STD_STRING(nickname))) chan.Left( *u, "server idiocy");
    }
    delete u;
  }catch(std::runtime_error){
  }
}


Channel& IServer::_AddChannel( const wxString& chan )
{
  if ( m_channels.ChannelExists(STD_STRING(chan)) ) return m_channels.GetChannel(STD_STRING(chan));
  Channel* c = new Channel( *this );
  c->SetName( STD_STRING(chan) );

  m_channels.AddChannel( *c );
  return *c;
}


void IServer::_RemoveChannel( const wxString& name )
{
  Channel* c = &m_channels.GetChannel( STD_STRING(name));
  m_channels.RemoveChannel(STD_STRING(name));
  ASSERT_LOGIC( c != 0, _T("IServer::_RemoveChannel(\"") + name + _T("\"): GetChannel returned NULL pointer"));
  delete c;
}

IBattle& IServer::_AddBattle( const int& id )
{
  if ( battles_iter->BattleExists( id ) ) return battles_iter->GetBattle( id );
  IBattle* b = new Battle( *this, id );

  m_battles.AddBattle( *b );
  return *b;
}


void IServer::_RemoveBattle( const int& id )
{
  IBattle* b = &battles_iter->GetBattle( id );
  m_battles.RemoveBattle( id );
  ASSERT_LOGIC( b != 0, _T("IServer::_RemoveBattle(): GetBattle returned NULL pointer"));
  delete b;
}


void IServer::OnDisconnected()
{
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
  while ( battles_iter->GetNumBattles() > 0 )
  {
    battles_iter->IteratorBegin();
    IBattle* b = battles_iter->GetBattle();
    if (b!=0)
    {
        m_battles.RemoveBattle( b->GetBattleId() );
        delete b;
    }
  }
  while ( m_channels.GetNumChannels() > 0 )
  {
    Channel* c = &m_channels.GetChannel( 0 );
    m_channels.RemoveChannel( c->GetName() );
    delete c;
  }

}

wxArrayString IServer::GetRelayHostList()
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
