//
// Class: System
//

#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include <wx/intl.h>
#include "server.h"
#include "socket.h"
#include "channellist.h"


class System : public ChannelList, public UserList
{
  public:
    System(): m_serv(NULL) { }
    ~System() {}
  
    // System interface
  
    void Connect( const std::string& servername, const std::string& username, const std::string& password );
    void Disconnect();
  
    Server* serv() { return m_serv; }
    void SetServer( Server* serv ) { m_serv = serv; }
  
  protected:
    // System variables
  
    Server* m_serv;
  
};

System& sys();

#endif  //_SYSTEM_H_

