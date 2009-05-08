/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: Server
//

#include <stdexcept>
#include <wx/log.h>
#include <wx/thread.h>

#include "server.h"
#include "socket.h"
#include "battle.h"
#include "channel/channel.h"
#include "user.h"
#include "utils.h"
#include "chatpanel.h"

Server::Server():
battles_iter(new BattleList_Iter(&m_battles)),
m_keepalive(15),
m_ping_msg(wxEmptyString),
m_ping_int(0),
m_ping_t(0)
{
	SetSocket( new Socket( *this, false ) );
}

Server::~Server()
{
	_EnablePingThread( false );
	delete battles_iter;
  if(uidata.panel)uidata.panel->SetServer(NULL);
  delete GetSocket();
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


int Server::GetNumChannels()
{
  return m_channels.GetNumChannels();
}


Channel& Server::GetChannel( const int& index )
{
  return m_channels.GetChannel( index );
}


bool Server::ChannelExists( const wxString& name )
{
  return m_channels.ChannelExists( name );
}


Battle& Server::GetBattle( const int& battleid )
{
  return battles_iter->GetBattle( battleid );
}


bool Server::BattleExists( const int& battleid )
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
  Channel* c = new Channel( *this, ui() );
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
	_EnablePingThread( false );
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


//! @brief Set ping info to be used by the ping thread.
//! @note Set interval to 0 to turn off the ping thread.
void Server::SetPingInfo( const wxString& msg, unsigned int interval )
{
  m_ping_msg = msg;
  m_ping_int = interval;
  _EnablePingThread( _ShouldEnablePingThread() );
}


void Server::_EnablePingThread( bool enable )
{

  if ( !enable )
  {
    if ( m_ping_t )
    {

      // Reset values to be sure.
      m_ping_int = 0;
      m_ping_msg = wxEmptyString;

      m_ping_t->Wait();
      delete m_ping_t;

      m_ping_t = 0;
    }
  }
  else
  {
    if ( !m_ping_t )
    {
      m_ping_t = new PingThread( *this );
      m_ping_t->Init();
    }
  }
}


//! @brief Check if we should enable or dsable the ping htread.
//! @see Server::_EnablePingThread
bool Server::_ShouldEnablePingThread()
{
  return ( (m_ping_int != 0) );
}




PingThread::PingThread( Server& server ):
  m_server(server)
{
}


void PingThread::Init()
{
  Create();
  SetPriority( WXTHREAD_MAX_PRIORITY );
  Run();
}


void* PingThread::Entry()
{
  int milliseconds = m_server.GetPingInterval();

  while ( !TestDestroy() )
  {
    if ( !m_server.GetPingEnabled() ) break;
    m_server.Ping();
    // break if woken
    if(!Sleep(milliseconds))break;
  }
  return 0;
}

void PingThread::OnExit()
{
}
