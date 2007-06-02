//
// Class: Server
//

#include <assert.h>
#include <iostream>
#include <stdexcept>
#include "server.h"
#include "socket.h"



User& Server::GetUser( const std::string& nickname )
{
  return m_users.GetUser( nickname );
}


bool Server::UserExists( const std::string& nickname )
{
  return m_users.UserExists( nickname );
}


Channel& Server::GetChannel( const std::string& name )
{
  return m_channels.GetChannel( name );
}


int Server::GetNumChannels()
{
  return m_channels.GetNumChannels();
}


Channel& Server::GetChannel( const int& index )
{
  m_channels.GetChannel( index );
}


bool Server::ChannelExists( const std::string& name )
{
  return m_channels.ChannelExists( name );
}


Battle& Server::GetBattle( const int& battleid )
{
  return m_battles.GetBattle( battleid );
}


bool Server::BattleExists( const int& battleid )
{
  return m_battles.BattleExists( battleid );
}



User& Server::_AddUser( const std::string& user )
{
  if ( m_users.UserExists( user ) ) return m_users.GetUser( user );
  User* u = new User( user );
  m_users.AddUser( *u );
  return *u;
}


void Server::_RemoveUser( const std::string& nickname )
{
  User* u = &m_users.GetUser( nickname );
  m_users.RemoveUser( nickname );
  if ( u == NULL ) throw std::logic_error("Server::_RemoveUser(\"" + nickname + "\"): GetUser returned NULL pointer");
  //delete u; //! @todo Fix memleak
}


Channel& Server::_AddChannel( const std::string& chan )
{
  if ( m_channels.ChannelExists( chan ) ) return m_channels.GetChannel( chan );
  Channel* c = new Channel( *this, m_ui );
  c->SetName( chan );

  m_channels.AddChannel( *c );
  return *c;
}


void Server::_RemoveChannel( const std::string& name )
{
  Channel* c = &m_channels.GetChannel( name );
  m_channels.RemoveChannel( name );
  if ( c == NULL ) throw std::logic_error("Server::_RemoveChannel(\"" + name + "\"): GetChannel returned NULL pointer");
  delete c;
}

Battle& Server::_AddBattle( const int& id )
{
  if ( m_battles.BattleExists( id ) ) return m_battles.GetBattle( id );
  Battle* b = new Battle( *this, m_ui, id );

  m_battles.AddBattle( *b );
  return *b;
}


void Server::_RemoveBattle( const int& id )
{
  Battle* b = &m_battles.GetBattle( id );
  m_battles.RemoveBattle( id );
  if ( b == NULL ) throw std::logic_error("Server::_RemoveBattle(): GetBattle returned NULL pointer");
  delete b;
}


