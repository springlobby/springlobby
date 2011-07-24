#ifndef SPRINGLOBBY_HEADERGUARD_CHANNELLIST_H
#define SPRINGLOBBY_HEADERGUARD_CHANNELLIST_H

#include <map>
#include <wx/string.h>

class Channel;

//! @brief mapping from channel name to channel object
typedef std::map<wxString, Channel*> channel_map_t;
//! @brief iterator for channel map
typedef channel_map_t::iterator channel_iter_t;

//! @brief List of Channel objects
class ChannelList
{
  public:
    ChannelList();
    void AddChannel( Channel& channel );
    void RemoveChannel( const wxString& name );
    Channel& GetChannel( const wxString& name );
    Channel& GetChannel( channel_map_t::size_type index );
    bool ChannelExists( const wxString& name ) const;
    channel_map_t::size_type GetNumChannels() const;

  private:
    channel_map_t m_chans;
    // The following are used as internal cache to speed up random access:
    mutable channel_iter_t m_seek;
    mutable channel_map_t::size_type m_seekpos;
};

#endif // SPRINGLOBBY_HEADERGUARD_CHANNELLIST_H


/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2011

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    SpringLobby is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

