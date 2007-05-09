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

#define PING_TIMEOUT 30

using namespace std;

class ServerEvents;
class Socket;

//! @brief Struct used to store a client's status.
struct Clientstatus {
  bool in_game;
  bool away;
  int rank;
  bool moderator;
  bool bot;
};


typedef int Servererror;

#define PE_NONE 0

#define RANK_1 100
#define RANK_2 200
#define RANK_3 300
#define RANK_4 400

//! @brief Abstract baseclass that is used to implement a server protocol.
class Server
{
  public:
    Server();
    virtual ~Server();
  
    // Server interface
  
    virtual void set_socket( Socket* sock );
    virtual Socket* get_socket( );
    virtual void set_uicontrol( ServerEvents* ui );
    virtual ServerEvents* get_uicontrol( );
  
    virtual void connect( string addr, const int port ) = 0;
    virtual void disconnect() = 0;
    virtual bool is_connected() = 0;
  
    virtual void login( string username, string password ) = 0;
    virtual void logout() = 0;
    virtual bool is_online() = 0;
  
    virtual void update() = 0;
  
    virtual void join_channel( string channel, string key ) = 0;
    virtual void part_channel( string channel ) = 0;
  
    virtual void say_channel( string channel, string msg ) = 0;
    virtual void say_private( string nick, string msg ) = 0;
  
    virtual void set_keepalive_interval( int seconds ) { m_keepalive = seconds; }
    virtual int get_keepalive_interval() { return m_keepalive; }
  
    virtual void ping() = 0;
  protected:
    // Server variables
  
    Socket* m_sock;
    ServerEvents* m_ui;
    int m_keepalive;

};


#endif  //_SERVER_H_

