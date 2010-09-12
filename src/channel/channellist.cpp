/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
#include <iterator>
#include <stdexcept>
#include <wx/log.h>

#include "channellist.h"
#include "channel.h"
#include "../utils/debug.h"
#include "../utils/conversion.h"

const UserList::user_map_t::size_type SEEKPOS_INVALID = UserList::user_map_t::size_type(-1);

ChannelList::ChannelList() : m_seek(m_chans.end()), m_seekpos(SEEKPOS_INVALID)
{ }

void ChannelList::AddChannel( Channel& channel )
{
  m_chans[channel.GetName()] = &channel;
  m_seekpos = SEEKPOS_INVALID;
}

void ChannelList::RemoveChannel( const wxString& name )
{
  m_chans.erase( name );
  m_seekpos = SEEKPOS_INVALID;
}

Channel& ChannelList::GetChannel( const wxString& name )
{
  channel_iter_t u = m_chans.find(name);
  ASSERT_LOGIC( u != m_chans.end(), _T("ChannelList::GetChannel(\"") + name + _T("\"): no such channel"));
  return *u->second;
}

Channel& ChannelList::GetChannel( channel_map_t::size_type index )
{
  if (m_seekpos == SEEKPOS_INVALID || m_seekpos > index) {
    m_seek = m_chans.begin();
    m_seekpos = 0;
  }
  std::advance( m_seek, index - m_seekpos );
  m_seekpos = index;
  return *m_seek->second;
}

bool ChannelList::ChannelExists( const wxString& name ) const
{
  return m_chans.find( name ) != m_chans.end();
}

channel_map_t::size_type ChannelList::GetNumChannels() const
{
  return m_chans.size();
}

