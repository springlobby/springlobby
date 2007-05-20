//
// Class: Channel
//

#include "channel.h"
#include "ui.h"
#include "system.h"

Channel::Channel()
{
  m_topic = "";
  m_topic_nick = "";
  m_name = "";
  m_userdata = NULL;
}


Channel::~Channel()
{

}


void Channel::SetName( const std::string& name )
{
  m_name = name;
}


std::string Channel::GetName()
{
  return m_name;
}


void Channel::Said( User& who, const std::string& message )
{
  ui().OnChannelSaid( *this , who, message );
}


void Channel::Say( const std::string& message )
{
}


void Channel::DidAction( User& who, const std::string& action )
{
}


void Channel::DoAction( const std::string& action )
{
}


void Channel::Left( User& who, const std::string& reason )
{
  assert( GetUser( who.GetNick() ) != NULL );
  RemoveUser( who.GetNick() );
  ui().OnUserLeftChannel( *this, who, reason );
}


void Channel::Leave()
{
  assert( sys().serv() != NULL );
  sys().serv()->PartChannel( m_name );
}

 
  
void Channel::Joined( User& who )
{
  assert( GetUser( who.GetNick() ) == NULL );
  AddUser( &who );
  ui().OnUserJoinedChannel( *this, who );
}


void Channel::SetUserData( void* userdata )
{
  m_userdata = userdata;
}


void* Channel::GetUserData()
{
  return m_userdata;
}


void Channel::SetTopic( const std::string& topic, const std::string& who )
{
  m_topic = topic;
  m_topic_nick = who;
  
  ui().OnChannelTopic( *this, who, topic );
}

std::string Channel::GetTopicSetBy()
{
  return m_topic_nick;
}


std::string Channel::GetTopic()
{
  return m_topic;
}


void Channel::AddUser( User* user )
{
  UserList::AddUser( user );
}


void Channel::RemoveUser( const std::string& nick )
{
  UserList::RemoveUser( nick );
}



