//
// Class: System
//

#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include <wx/intl.h>
#include "server.h"
#include "socket.h"
#include "chatlist.h"
#include "channellist.h"


class System : public ChannelList, public UserList
{
  public:
    System() {}
    ~System() {}
  
    // System interface
  
    void Connect( const string servername, const string username, const string password );
    void Disconnect();
  
    Server* serv();
    void SetServer( Server* serv );
  
  protected:
    // System variables
  
    Server* m_serv;
  
};

System& sys();

#endif  //_SYSTEM_H_

