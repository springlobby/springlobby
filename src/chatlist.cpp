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
// Class: ChatList
//
// Created on: Sat May  5 21:41:21 2007
//

#include "chatlist.h"


ChatList::ChatList()
{
  m_serv = NULL;
}


ChatList::~ChatList()
{

}

ChatPanel* ChatList::GetChannelPanel( string chan_name )
{
  if ( m_channels.count( chan_name ) > 0 ) {
    chatpanel_iter_t found = m_channels.find( chan_name );
    return (*found).second;
  } else {
    return NULL;
  }
}

void ChatList::SetChannelPanel( string chan_name, ChatPanel* window )
{
  m_channels[chan_name] = window;
}

void ChatList::RemoveChannelPanel( string chan_name )
{
  m_channels.erase( chan_name );
}

ChatPanel* ChatList::GetPrivChatPanel( string nick_name )
{
  if ( m_priv_chats.count( nick_name ) > 0 ) {
    chatpanel_iter_t found = m_priv_chats.find( nick_name );
    return (*found).second;
  } else {
    return NULL;
  }
}

void ChatList::SetPrivChatPanel( string nick_name, ChatPanel* window )
{
  m_priv_chats[nick_name] = window;
}

bool ChatList::ChannelExists( string chan_name )
{
  return (m_channels.count( chan_name ) > 0);
}

bool ChatList::PrivChatExists( string nick_name )
{
  return (m_priv_chats.count( nick_name ) > 0);
}

int ChatList::NumChanels()
{
  return m_channels.size();
}

int ChatList::NumPrivChat()
{
  return m_priv_chats.size();  
}
