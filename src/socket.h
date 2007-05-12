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
// Class: Socket
// Created on: Fri Apr 27 16:26:26 2007
//

#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <string>

using namespace std;

typedef int Sockstate;

#define SS_CLOSED 0
#define SS_CONNECTING 1
#define SS_OPEN 2

typedef int Sockerror;

#define SE_NO_ERROR 0
#define SE_NOT_CONNECTED 1
#define SE_RESOLVE_HOST_FAILED 2
#define SE_CONNECT_TO_HOST_FAILED 3

//! @brief Class that implements a TCP client socket.
class Socket
{
  public:
    Socket();
    virtual ~Socket();
  
    // Socket interface
  
    virtual void Connect( const string addr, const int port );
    virtual void Disconnect( );
  
    virtual bool Send( string data );
    virtual bool Recive( string& data );
  
    virtual Sockstate State( );
    virtual Sockerror Error( );
  
  protected:
  // Socket variables
  
};


#endif  //_SOCKET_H_

