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
// Class: System
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
    System();
    virtual ~System();
  
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

