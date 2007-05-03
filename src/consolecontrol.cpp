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
// Class: ConsoleControl
//
// Created on: Sat Apr 28 06:44:53 2007
//

#include <iostream>
#include "consolecontrol.h"


ConsoleControl::ConsoleControl() : Uicontrol()
{
  
}


ConsoleControl::~ConsoleControl()
{
  
}

string ConsoleControl::status2string( Clientstatus stat )
{
  string str = "";
  if ( stat.in_game )
    str += "G";
  else
    str += "g";

  if ( stat.away )
    str += "A";
  else
    str += "a";

  if ( stat.moderator )
    str += "M";
  else
    str += "m";
  
  if ( stat.moderator )
    str += "B";
  else
    str += "b";
  
  if ( stat.rank <= RANK_1 )
    str += " r:1";
  else if ( stat.rank <= RANK_2 )
    str += " r:2";
  else if ( stat.rank <= RANK_3 )
    str += " r:3";
  else
    str += " r:4";
  
  return str;
}

void ConsoleControl::on_connected( string server_ver, bool supported )
{
  cout << "--- Connected to server version: " << server_ver.c_str() << endl;
  if ( !supported ) {
    cout << "!!! Version " << server_ver.c_str() << " is not yet supported by this application yet so you might run into problems... Press any key to continue." << endl;
    char ch;
    cin >> ch;
  }
  std::cout << "*** Logging in." << std::endl;
  _proto->login( "tkall", "Passw0rd" );
}

void ConsoleControl::on_disconnected()
{
  cout << "!!! Disconnected." << endl;
  exit(1);
}

void ConsoleControl::on_login()
{
  //cout << "--- Logged in." << endl;
}

void ConsoleControl::on_login_info_complete()
{
  cout << "--- All login info receved." << endl;
}


void ConsoleControl::on_logout()
{
  cout << "--- Logged out." << endl;
}

void ConsoleControl::on_unknown_command( string command, string params )
{
  cout << "!!! Unknown command: " << command.c_str() << " params: " << params.c_str() << endl;
}

void ConsoleControl::on_socket_error( const Sockerror error )
{
  cout << "!!! Socket error: " << error << endl;
}


void ConsoleControl::on_protocol_error( const Protocolerror error )
{
  cout << "!!! Protocol error: " << error << endl;
}

void ConsoleControl::on_motd( string msg )
{
  cout << msg.c_str() << endl;
}

void ConsoleControl::on_pong( int ping_time )
{
  cout << "--- Ping? Pong! took <" << ping_time + 1 << "s" << endl;
}

void ConsoleControl::on_new_user( string nick, string contry, int cpu )
{
  //cout << "New user: " << nick.c_str() << " from: " << contry.c_str() << " cpu: " << cpu << endl;
}

void ConsoleControl::on_user_status( string nick, Clientstatus status )
{
  //cout << "Userstatus: " << nick.c_str() << " Status: " << status2string( status ).c_str() << endl;
}

void ConsoleControl::on_user_quit( string nick )
{
  //cout << "User quit: " << nick.c_str() << endl;
}

void ConsoleControl::on_battle_opened( int id, bool replay, int nat, string nick, 
                       string host, int port, int maxplayers, 
                       bool haspass, int rank, int hash, string map, 
                       string title, string mod )
{
  //cout << "Battle opened: #" << id << " \"" << title.c_str() << "\" by " << nick.c_str() << endl;
}

void ConsoleControl::on_user_joined_battle( int battleid, string nick )
{
  //cout << nick.c_str() << " joined battle: #" << battleid << endl;
}

void ConsoleControl::on_user_left_battle( int battleid, string nick )
{
  //cout << nick.c_str() << " left battle: #" << battleid << endl;
}

void ConsoleControl::on_battleinfo_updated( int battleid, int spectators, bool locked, int maphash, string map )
{
  //cout << "Battle updated: #" << battleid << " specs: " << spectators << " map: " << map.c_str() << endl;
}

void ConsoleControl::on_battle_closed( int battleid )
{
  //cout << "Battle closed: #" << battleid << endl;
}
