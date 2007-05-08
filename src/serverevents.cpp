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
//
// Created on: Fri Apr 27 16:29:51 2007
//

#include "serverevents.h"


ServerEvents::ServerEvents()
{
  m_cl = NULL;
}


ServerEvents::~ServerEvents()
{

}


void ServerEvents::on_connected( string server_ver, bool supported )
{
  
}

void ServerEvents::on_disconnected()
{
}
      
void ServerEvents::on_login()
{
}

void ServerEvents::on_login_info_complete()
{
}

void ServerEvents::on_logout()
{
}
      
void ServerEvents::on_unknown_command( string command, string params )
{
}

void ServerEvents::on_socket_error( const Sockerror error )
{
}

void ServerEvents::on_protocol_error( const Protocolerror error )
{
}

void ServerEvents::on_motd( string msg )
{
}

void ServerEvents::on_pong( int ping_time )
{
}
      
void ServerEvents::on_new_user( string nick, string contry, int cpu )
{
}

void ServerEvents::on_user_status( string nick, Clientstatus status )
{
}

void ServerEvents::on_user_quit( string nick )
{
}
      
void ServerEvents::on_battle_opened( int id, bool replay, int nat, string nick, 
                       string host, int port, int maxplayers, 
                       bool haspass, int rank, int hash, string map, 
                       string title, string mod )
{
}

void ServerEvents::on_user_joined_battle( int battleid, string nick )
{
}

void ServerEvents::on_user_left_battle( int battleid, string nick )
{
}

void ServerEvents::on_battleinfo_updated( int battleid, int spectators, bool locked, int maphash, string map )
{
}

void ServerEvents::on_battle_closed( int battleid )
{
}


