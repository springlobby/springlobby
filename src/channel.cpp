//
// Class: Channel
//

#include "channel.h"
#include "ui.h"
#include "server.h"
#include "user.h"

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
  m_ui.OnChannelSaid( *this , who, message );
}


void Channel::Say( const std::string& message )
{
  debug_func( "" );
  m_serv.SayChannel( m_name, message );
}


void Channel::DidAction( User& who, const std::string& action )
{
}


void Channel::DoAction( const std::string& action )
{
}


void Channel::Left( User& who, const std::string& reason )
{
  RemoveUser( who.GetNick() );
  m_ui.OnUserLeftChannel( *this, who, reason );
}


void Channel::Leave()
{
  m_serv.PartChannel( m_name );
}

 
  
void Channel::Joined( User& who )
{
  AddUser( who );
  m_ui.OnUserJoinedChannel( *this, who );
}


void Channel::SetTopic( const std::string& topic, const std::string& who )
{
  m_topic = topic;
  m_topic_nick = who;
  
  m_ui.OnChannelTopic( *this, who, topic );
}

std::string Channel::GetTopicSetBy()
{
  return m_topic_nick;
}


std::string Channel::GetTopic()
{
  return m_topic;
}


void Channel::AddUser( User& user )
{
  UserList::AddUser( user );
}


void Channel::RemoveUser( const std::string& nick )
{
  UserList::RemoveUser( nick );
}



