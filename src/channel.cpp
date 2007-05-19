/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */


//
// Class: Channel
//
#include "channel.h"
#include "ui.h"

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


void Channel::SetName( const string& name )
{
  m_name = name;
}


string Channel::GetName()
{
  return m_name;
}


void Channel::Said( User& who, const string& message )
{
  ui().OnChannelSaid( *this , who, message );
}


void Channel::Say( const string& message )
{
}


void Channel::DidAction( User& who, const string& action )
{
}


void Channel::DoAction( const string& action )
{
}


void Channel::Left( User& who, const string& reason )
{
  assert( GetUser( who.GetNick() ) != NULL );
  RemoveUser( who.GetNick() );
  ui().OnUserLeftChannel( *this, who, reason );
}


void Channel::Leave()
{
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


void Channel::SetTopic( const string& topic, User& who )
{
  m_topic = topic;
  m_topic_nick = who.GetNick();
  
  ui().OnChannelTopic( *this, who, topic );
}

string Channel::GetTopicSetBy()
{
  return m_topic_nick;
}


string Channel::GetTopic()
{
  return m_topic;
}


void Channel::AddUser( User* user )
{
  UserList::AddUser( user );
}


void Channel::RemoveUser( const string nick )
{
  UserList::RemoveUser( nick );
}



