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
#ifndef _CHANNELLIST_H_
#define _CHANNELLIST_H_

#include <map>
#include <string>
#include <assert.h>
#include "channel.h"

using namespace std;


//! @brief Used internally by ChannelList in its std::map<> lists.
struct ChannelListMapCompare
{
  bool operator()(const string s1, const string s2) const
  {
    return s1.compare(s2) < 0;
  }
};


//! @brief std::map<> list that stores Channel pointers.
typedef map<string, Channel*, ChannelListMapCompare> channel_map_t;
//! @brief channel_map_t iterator.
typedef channel_map_t::iterator channel_iter_t;


class ChannelList
{
  public:
    ChannelList();
    virtual ~ChannelList();
  
    // ChannelList interface
  
    virtual void AddChannel( Channel* channel );
    virtual void RemoveChannel( const string name );
  
    virtual Channel* GetChannel( const string name );
    virtual bool ChannelExists( const string name );
  
    virtual int GetNumChannels();
  
  protected:
    // ChannelList variables
  
    channel_map_t m_chans;
  
};


#endif  //_CHANNELLIST_H_

