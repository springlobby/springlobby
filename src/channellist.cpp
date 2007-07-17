#include <iterator>
#include <stdexcept>

#include "channellist.h"
#include "channel.h"

// FIXME make this the max value of the type it is used for
#define SEEK_INVALID (-1)

ChannelList::ChannelList() : m_seek(m_chans.end()), m_seekpos(-1)
{ }

void ChannelList::AddChannel( Channel& channel )
{
  m_chans[channel.GetName()] = &channel;
  m_seekpos = SEEK_INVALID;
}

void ChannelList::RemoveChannel( const std::string& name )
{
  m_chans.erase( name );
  m_seekpos = SEEK_INVALID;
}

Channel& ChannelList::GetChannel( const std::string& name )
{
  channel_iter_t u = m_chans.find(name);
  if (u == m_chans.end())
    throw std::logic_error("ChannelList::GetChannel(\"" + name + "\"): no such channel");
  return *u->second;
}

Channel& ChannelList::GetChannel( channel_map_t::size_type index )
{
  if (m_seekpos == SEEK_INVALID || m_seekpos > index) {
    m_seek = m_chans.begin();
    m_seekpos = 0;
  }
  std::advance( m_seek, index - m_seekpos );
  m_seekpos = index;
  return *m_seek->second;
}

bool ChannelList::ChannelExists( const std::string& name )
{
  return m_chans.find( name ) != m_chans.end();
}

channel_map_t::size_type ChannelList::GetNumChannels()
{
  return m_chans.size();
}

