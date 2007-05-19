//
// Class: ChannelList
//

#include "channellist.h"


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


void ChannelList::RemoveChannel( const std::string& name )
{
  m_chans.erase( name );
}

  
Channel* ChannelList::GetChannel( const std::string& name )
{
  if ( m_chans.count( name ) > 0 ) {
    channel_iter_t found = m_chans.find( name );
    return (*found).second;
  } else {
    return NULL;
  }
}


bool ChannelList::ChannelExists( const std::string& name )
{
  return (m_chans.count( name ) > 0);
}

  
int ChannelList::GetNumChannels()
{
  return m_chans.size();
}

