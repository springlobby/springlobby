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
// Class: Uicontrol
// Created on: Fri Apr 27 16:29:51 2007
//

#ifndef _UICONTROL_H_
#define _UICONTROL_H_

#include "socket.h"
#include "protocol.h"

typedef int Protocolerror;

class Uicontrol
{
  public:
    Uicontrol();
    virtual ~Uicontrol();
  
  // Uicontrol interface
  
    virtual void on_connected( string server_ver, bool supported ) {}
    virtual void on_disconnected() {}
      
    virtual void on_login() {}
    virtual void on_login_info_complete() {}
    virtual void on_logout() {}
      
    virtual void on_unknown_command( string command, string params ) {}
    virtual void on_socket_error( const Sockerror error ) {}
    virtual void on_protocol_error( const Protocolerror error ) {}
    virtual void on_motd( string msg ) {}
    virtual void on_pong( int ping_time ) {}
      
    virtual void on_new_user( string nick, string contry, int cpu ) {}
    virtual void on_user_status( string nick, Clientstatus status ) {}
    virtual void on_user_quit( string nick ) {}
      
    virtual void on_battle_opened( int id, bool replay, int nat, string nick, 
                                   string host, int port, int maxplayers, 
                                   bool haspass, int rank, int hash, string map, 
                                   string title, string mod ) {}

    virtual void on_user_joined_battle( int battleid, string nick ) {}
    virtual void on_user_left_battle( int battleid, string nick ) {}
    virtual void on_battleinfo_updated( int battleid, int spectators, bool locked, int maphash, string map ) {}
    virtual void on_battle_closed( int battleid ) {}
      
  protected:
  // Uicontrol variables
  
};


#endif  //_UICONTROL_H_

