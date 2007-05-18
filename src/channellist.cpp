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
// Class: ChannelList
//
#include "/home/tc/Projects/springlobby/trunk/src/channellist.h"


ChannelList::ChannelList()
{
  
}


ChannelList::~ChannelList()
{
  
}

void ChannelList::AddChannel( Channel* channel )
{
  assert( channel != NULL );
  m_chans[channel->GetName()] = channel;
}


void ChannelList::RemoveChannel( const string name )
{
  m_chans.erase( name );
}

  
Channel* ChannelList::GetChannel( const string name )
{
  if ( m_chans.count( name ) > 0 ) {
    channel_iter_t found = m_chans.find( name );
    return (*found).second;
  } else {
    return NULL;
  }
}


bool ChannelList::ChannelExists( const string name )
{
  return (m_chans.count( name ) > 0);
}

  
int ChannelList::GetNumChannels()
{
  return m_chans.size();
}

