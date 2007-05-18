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


Channel::Channel()
{
  
}


Channel::~Channel()
{
    // TODO: put destructor code here
}


void Channel::SetName( const string& name )
{
  m_name = name;
}


string Channel::GetName()
{
  return m_name;
}


void Channel::Said( const string& who, const string& message )
{
}


void Channel::Said( User& who, const string& message )
{
}


void Channel::Say( const string& message )
{
}


  
void Channel::DidAction( const string& who, const string& action )
{
}


void Channel::DidAction( User& who, const string& action )
{
}


void Channel::DoAction( const string& action )
{
}


  
void Channel::Left( const string& who )
{
}


void Channel::Left( User& who )
{
}


void Channel::Leave()
{
}


  
void Channel::Joined( const string& who )
{
}


  
void Channel::SetUserData( void* userdata )
{
}


void* Channel::GetUserData()
{
}



void Channel::SetOnSaidCallback( channel_msg_callback callback )
{
}


channel_msg_callback Channel::GetOnSaidCallback()
{
}



void Channel::SetOnDidActionCallback( channel_msg_callback callback )
{
}


channel_msg_callback Channel::GetOnDidActionCallback()
{
}



void Channel::SetOnLeaveCallback( channel_cmd_callback callback )
{
}


channel_cmd_callback Channel::GetOnLeaveCallback()
{
}



void Channel::SetOnLeftCallback( channel_whocmd_callback callback )
{
}


channel_whocmd_callback Channel::GetOnLeftCallback()
{
}



void Channel::SetOnJoinedCallback( channel_whocmd_callback callback )
{
}


channel_whocmd_callback Channel::GetOnJoinedCallback()
{
}



void Channel::AddUser( User* user )
{
}


void Channel::RemoveUser( const string nick )
{
}



