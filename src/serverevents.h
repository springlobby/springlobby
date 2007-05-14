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
#include "server.h"
#include "chatlist.h"

typedef int Protocolerror;

//! @brief Class that implements server event behaviour.
class ServerEvents
{
  public:
    ServerEvents();
    virtual ~ServerEvents();
  
  // Uicontrol interface
  
    virtual void on_connected( string server_ver, bool supported ) = 0;
    virtual void on_disconnected() = 0;
      
    virtual void on_login() = 0;
    virtual void on_login_info_complete() = 0;
    virtual void on_logout() = 0;
      
    virtual void on_unknown_command( string command, string params ) = 0;
    virtual void on_socket_error( const Sockerror error ) = 0;
    virtual void on_protocol_error( const Protocolerror error ) = 0;
    virtual void on_motd( string msg ) = 0;
    virtual void on_pong( int ping_time ) = 0;
      
    virtual void on_new_user( string nick, string contry, int cpu ) = 0;
    virtual void on_user_status( string nick, Clientstatus status ) = 0;
    virtual void on_user_quit( string nick ) = 0;
    
    virtual void on_battle_opened( int id, bool replay, int nat, string nick, 
                                   string host, int port, int maxplayers, 
                                   bool haspass, int rank, int hash, string map, 
                                   string title, string mod ) = 0;

    virtual void on_user_joined_battle( int battleid, string nick ) = 0;
    virtual void on_user_left_battle( int battleid, string nick ) = 0;
    virtual void on_battleinfo_updated( int battleid, int spectators, bool locked, int maphash, string map ) = 0;
    virtual void on_battle_closed( int battleid ) = 0;
    
    virtual void on_join_channel_result( bool success, string channel, string reason ) = 0;
    virtual void on_channel_said( string channel, string who, string message ) = 0;
    virtual void on_channel_join( string channel, string who ) = 0;
    virtual void on_channel_part( string channel, string who, string message ) = 0;
    virtual void on_channel_topic( string channel, string who, string message ) = 0;
  protected:
  // Uicontrol variables
    
};


#endif  //_UICONTROL_H_

