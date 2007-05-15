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
// Class: Server
// Created on: Fri Apr 27 16:23:28 2007
//

#ifndef _SERVER_H_
#define _SERVER_H_

#include <string>
#include "user.h"

#define PING_TIMEOUT 30

using namespace std;

class ServerEvents;
class Socket;


typedef int Servererror;

#define PE_NONE 0


//! @brief Abstract baseclass that is used to implement a server protocol.
class Server
{
  public:
    Server();
    virtual ~Server();
  
    // Server interface
  
    virtual void SetSocket( Socket* sock );
    virtual Socket* GetSocket( );
    virtual void SetServerEvents( ServerEvents* ui );
    virtual ServerEvents* GetServerEvents( );
  
    virtual void Connect( string addr, const int port ) = 0;
    virtual void Disconnect() = 0;
    virtual bool IsConnected() = 0;
  
    virtual void Login() = 0;
    virtual void Logout() = 0;
    virtual bool IsOnline() = 0;
  
    virtual void Update() = 0;
  
    virtual void JoinChannel( string channel, string key ) = 0;
    virtual void PartChannel( string channel ) = 0;
  
    virtual void SayChannel( string channel, string msg ) = 0;
    virtual void SayPrivate( string nick, string msg ) = 0;
  
    virtual void SetKeepaliveInterval( int seconds ) { m_keepalive = seconds; }
    virtual int GetKeepaliveInterval() { return m_keepalive; }
  
    virtual void SetUsername( const string username );
    virtual void SetPassword( const string password );
    
    virtual void Ping() = 0;
    /*
    virtual void on_connected( Socket* sock ) = 0;
    virtual void on_disconnected( Socket* sock ) = 0;
    virtual void on_data_recived( Socket* sock ) = 0;
    */
  protected:
    // Server variables
  
    Socket* m_sock;
    ServerEvents* m_ui;
    int m_keepalive;
    string m_user;
    string m_pass;

};


#endif  //_SERVER_H_

