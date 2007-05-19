//
// Class: ChannelList
//

#ifndef _CHANNELLIST_H_
#define _CHANNELLIST_H_

#include <map>
#include <string>
#include <assert.h>
#include "channel.h"


<<<<<<< .mine
//! @brief Used internally by ChannelList in its std::map<> lists.
struct ChannelListMapCompare
{
  bool operator()(const std::string& s1, const std::string& s2) const
  {
    return s1.compare(s2) < 0;
  }
};


=======
>>>>>>> .r55
//! @brief std::map<> list that stores Channel pointers.
<<<<<<< .mine
typedef std::map<std::string, Channel*, ChannelListMapCompare> channel_map_t;
=======
typedef map<string, Channel*> channel_map_t;
>>>>>>> .r55
//! @brief channel_map_t iterator.
typedef channel_map_t::iterator channel_iter_t;


class ChannelList
{
  public:
    ChannelList();
    ~ChannelList();
  
    // ChannelList interface
  
    void AddChannel( Channel* channel );
    void RemoveChannel( const std::string& name );
  
    Channel* GetChannel( const std::string& name );
    bool ChannelExists( const std::string& name );
  
    int GetNumChannels();
  
  protected:
    // ChannelList variables
  
    channel_map_t m_chans;
  
};


#endif  //_CHANNELLIST_H_

