//
// Class: ChannelList
//

#ifndef _CHANNELLIST_H_
#define _CHANNELLIST_H_

#include <map>
#include <string>
#include <assert.h>
#include "channel.h"


//! @brief std::map<> list that stores Channel pointers.
typedef std::map<std::string, Channel> channel_map_t;
//! @brief channel_map_t iterator.
typedef channel_map_t::iterator channel_iter_t;


//! @brief List of Channel objects
class ChannelList
{
  public:
    // ChannelList interface
  
    void AddChannel( Channel& channel ) { m_chans[channel.GetName()] = channel; }
    void RemoveChannel( const std::string& name ) { m_chans.erase( name ); }
  
    Channel& GetChannel( const std::string& name ) {
      channel_iter_t u = m_chans.find(name);
      if (u == m_chans.end()) throw std::logic_error("ChannelList::GetChannel(\"" + name + "\"): no such channel");
      return u->second;
    }
      
    bool ChannelExists( const std::string& name ) {
      return m_chans.find( name ) != m_chans.end();
    }
  
    int GetNumChannels() { return m_chans.size(); }
  
  private:
    // ChannelList variables
  
    channel_map_t m_chans;
  
};


#endif  //_CHANNELLIST_H_

