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
// Class: TASServer
//
// Created on: Fri Apr 27 19:25:15 2007
//

#include <iostream>
#include <assert.h>
#include <stdio.h>
#include "tasserver.h"
#include "md5.h"
#include "base64.h"
#include "socket.h"
#include "serverevents.h"

TASServer::TASServer() : Server()
{
  m_connected = false;
  m_online = false;
  m_buffer = "";
  m_last_ping = 0;
  m_ping_id = 1000;
  m_ser_ver = SER_VER_UNKNOWN;
}


TASServer::~TASServer()
{

}


void TASServer::set_socket( Socket* sock )
{
  Server::set_socket( sock );
  if ( sock == NULL ) return;
    
  sock->set_connected_callback( on_connected );
  sock->set_disconnected_callback( on_disconnected );
  sock->set_data_recived_callback( on_data_recived );
  
  sock->set_userdata( (void*)this );
  
}

void TASServer::connect( string addr, const int port )
{
  assert( m_sock != NULL );
  m_sock->Connect( addr, port );
  if ( is_connected() ) {
    m_last_ping = time( NULL );
    m_connected = true;
  }
  m_online = false;
  
}

void TASServer::disconnect()
{
  assert( m_sock != NULL );
  m_sock->Disconnect();
  m_connected = false;
}

bool TASServer::is_connected()
{
  if ( m_sock == NULL ) return false;
  return (m_sock->State() == SS_OPEN);
}
  
void TASServer::login()
{
  cout << "** TASServer::login()" << endl;
  string password = m_pass;
  unsigned char output[16];
  unsigned char* input = new unsigned char[ password.length() ];
  for ( int i = 0; i < password.length(); i++ )
    input[i] = password[i];
  
  md5_csum( input, password.length(), &output[0] );
  password = base64_encode( &output[0] , 16 );
  //cout << password.c_str() << endl;
  
  string data = "LOGIN ";
  data += m_user;
  data += " ";
  data += password;
  data += " 2100 * TKALCL 0.001\n";
  m_sock->Send( data );
  
  delete input;
}

void TASServer::logout()
{
  cout << "** TASServer::logout()" << endl;
  disconnect();
}

bool TASServer::is_online()
{
  if ( !m_connected ) return false;
  return m_online;
}

void TASServer::update()
{
  assert( m_sock != NULL ); // TODO: This should be handled and generate an error in released code.
  string data = ".";
  
  if ( !m_connected ) { // We are not formally connected yet, but might be.
    if ( is_connected() ) {
      m_last_ping = time( NULL );
      m_connected = true;
    }
    return;
  } else { // We are connected allready.
    if ( !is_connected() ) {
      m_connected = false;
      m_online = false;
      m_ui->on_disconnected();
      return;
    }

    time_t now = time( NULL );
    if ( m_last_ping + m_keepalive < now ) { // Is it time for a keepalive PING?
      ping();
    }
    handle_pinglist();
  }
  
  while ( !data.empty() ) { // Go on until recive stops providing data.
    
    data = "";
    if ( m_sock->Recive( data ) ) {
      m_buffer += data;
      if ( m_buffer.find( "\n", 0 ) != string::npos ) {
        execute_command( m_buffer );
        m_buffer = "";
      }
    }
    
  }
}


void TASServer::_recive_and_execute()
{
  string data;
  
  do {
    
    data = "";
    if ( m_sock->Recive( data ) ) {
      m_buffer += data;
      if ( m_buffer.find( "\n", 0 ) != string::npos ) {
        execute_command( m_buffer );
        m_buffer = "";
      }
    }
    
  } while ( !data.empty() ); // Go on until recive stops providing data.
}


void TASServer::execute_command( string in )
{
  string cmd;
  int pos;
  int replyid = -1;
  
  if ( in.empty() ) return;
  
  //cout << "Debug: " << in << endl;
  if ( in[0] == '#' ) {
    cmd = in.substr( 1, in.find( " ", 0 ) );
    replyid = atoi( cmd.c_str() );
    in = in.substr( pos + 1 );
  }
    
  pos = in.find( "\n", 0 );
  if ( pos != string::npos ) {
    assert( pos < in.length() ); // We might be throwing away a second command following this one.
    in.replace( pos, in.length() - pos, "" );
  }
    
  pos = in.find( " ", 0 );
  if ( pos == string::npos ) {
    // Must be command without parameters.
    cmd = in;
    in = "";
  } else {
    cmd = in.substr( 0, pos );
    in = in.substr( pos + 1 );
  }
  
  execute_command( cmd, in );
}

void TASServer::execute_command( string cmd, string params, int replyid )
{
  int pos, cpu, status, id, nat, port, maxplayers, rank, hash, specs;
  bool replay, haspass;
  string nick, contry, host, map, title, mod;
  Clientstatus cstatus;
  UTASClientstatus tasstatus;
  
  if ( cmd == "TASServer") {
    mod = get_word_param( params );
    if ( mod == "0.32" )
      m_ser_ver = SER_VER_0_32;
    else if ( mod == "0.33" )
      m_ser_ver = SER_VER_0_32;
    else if ( mod == "0.34" )
      m_ser_ver = SER_VER_0_34;
    else
      m_ser_ver = SER_VER_BAD;
    m_ui->on_connected( mod, (m_ser_ver > 0) );
    
  } else if ( cmd == "ACCEPTED" ) {
    m_online = true;
    m_ui->on_login( );
  } else if ( cmd == "MOTD" ) {
    m_ui->on_motd( params );
  } else if ( cmd == "ADDUSER" ) {
    nick = get_word_param( params );
    contry = get_word_param( params );
    cpu = get_int_param( params );
    m_ui->on_new_user( nick, contry, cpu );
  } else if ( cmd == "CLIENTSTATUS" ) {
    nick = get_word_param( params );
    tasstatus.byte = get_int_param( params );
    cstatus = conv_tasclientstatus( tasstatus.tasdata );
    m_ui->on_user_status( nick, cstatus );
  } else if ( cmd == "BATTLEOPENED" ) {
    id = get_int_param( params );
    replay = get_int_param( params );
    nat = get_int_param( params );
    nick = get_word_param( params );
    host = get_word_param( params );
    port = get_int_param( params );
    maxplayers = get_int_param( params );
    haspass = get_int_param( params );
    rank = get_int_param( params );
    hash = get_int_param( params );
    map = get_sentence_param( params );
    title = get_sentence_param( params );
    mod = get_sentence_param( params );
    
    m_ui->on_battle_opened( id, replay, nat, nick, host, port, maxplayers, 
                            haspass, (rank + 1)*100, hash, map, title, mod );
    
  } else if ( cmd == "JOINEDBATTLE" ) {
    id = get_int_param( params );
    nick = get_word_param( params );
    m_ui->on_user_joined_battle( id, nick );
  } else if ( cmd == "UPDATEBATTLEINFO" ) {
    id = get_int_param( params );
    specs = get_int_param( params );
    haspass = get_int_param( params );
    hash = get_int_param( params );
    map = get_sentence_param( params );
    m_ui->on_battleinfo_updated( id, specs, haspass, hash, map );
  } else if ( cmd == "LOGININFOEND" ) {
    m_ui->on_login_info_complete();
  } else if ( cmd == "REMOVEUSER" ) {
    nick = get_word_param( params );
    m_ui->on_user_quit( nick );
  } else if ( cmd == "BATTLECLOSED" ) {
    id = get_int_param( params );
    m_ui->on_battle_closed( id );
  } else if ( cmd == "LEFTBATTLE" ) {
    id = get_int_param( params );
    nick = get_word_param( params );
    m_ui->on_user_left_battle( id, nick );
  } else if ( cmd == "PONG" ) {
    handle_pong( replyid );
  } else {
    cout << "??? Cmd: " << cmd.c_str() << " params: " << params.c_str() << endl;
  }
  
}

string TASServer::get_word_param( string& params )
{
  int pos;
  string param;
  
  pos = params.find( " ", 0 );
  if ( pos == string::npos ) {
    param = params;
    params = "";
    return param;
  } else {
    param = params.substr( 0, pos );
    params = params.substr( pos + 1 );
    return param;
  }
}

string TASServer::get_sentence_param( string& params )
{
  int pos;
  string param;
  
  pos = params.find( "\t", 0 );
  if ( pos == string::npos ) {
    param = params;
    params = "";
    return param;
  } else {
    param = params.substr( 0, pos );
    params = params.substr( pos + 1 );
    return param;
  }
}

int TASServer::get_int_param( string& params )
{
  int pos;
  string param;
  
  pos = params.find( " ", 0 );
  if ( pos == string::npos ) {
    param = params;
    params = "";
    return atoi( param.c_str() );
  } else {
    param = params.substr( 0, pos );
    params = params.substr( pos + 1 );
    return atoi( param.c_str() );
  }
}

void TASServer::ping()
{
  string cmd = "";

  m_ping_id++;
//  cout << ">>> PING #" << _ping_id << endl;
  if ( version_support_replyid( m_ser_ver ) ) {
    cmd += "#";
    cmd += m_ping_id;
    cmd += " PING\n";
  } else {
    cmd += "PING ";
    cmd += m_ping_id;
    cmd += "\n";
  }
  
  m_sock->Send( cmd );
  
  TASPingListItem pli;
  pli.id = m_ping_id;
  pli.t = time( NULL );
  m_pinglist.push_back ( pli );
  m_last_ping = time( NULL );
}

void TASServer::handle_pong( int replyid )
{
  list<TASPingListItem>::iterator it;
  
//  cout << ">>> PONG #" << replyid << endl;
  bool found = false;
  for ( it = m_pinglist.begin(); it != m_pinglist.end(); it++ ) {
    if (it->id == replyid ) {
      found = true;
      break;
    }
  }
  
  if ( found ) {
    m_ui->on_pong( (time( NULL ) - it->t) );
    m_pinglist.erase( it );
  } else {
    if ( !m_pinglist.empty() ) {
      m_ui->on_pong( (time( NULL ) - m_pinglist.begin()->t) );
      m_pinglist.pop_front();
    } else {
      m_ui->on_pong( -1 );
    }
  }
}

void TASServer::handle_pinglist()
{
  list<TASPingListItem>::iterator it;
  int now = time( NULL );
  while ( !m_pinglist.empty() ) {
    if ( m_pinglist.begin()->t + PING_TIMEOUT < now ) {
      // TODO: Fix Ping time!
      //cout << "!!! Ping Timeout, took more than " << PING_TIMEOUT << "s" << endl;
      //_ui->on_pong( PING_TIMEOUT );
      m_pinglist.pop_front();
    } else {
      break;
    }
  }
}


void TASServer::join_channel( string channel, string key )
{
  //JOIN channame [key]
  assert( is_online() );
  assert( m_sock != NULL );
  
  string cmd = "JOIN " + channel;
  if ( key != "" )
    cmd += " " + key;
  cmd += "\n";
  
  m_sock->Send( cmd );
}


void TASServer::part_channel( string channel )
{
  //LEAVE channame
  assert( is_online() );
  assert( m_sock != NULL );
  
  m_sock->Send( "LEAVE " + channel + "\n" );
  
}
 

void TASServer::say_channel( string channel, string msg )
{
  //SAY channame {message}
  assert( is_online() );
  assert( m_sock != NULL );
  
  m_sock->Send( "SAY " + channel + " " + msg + "\n" );
}


void TASServer::say_private( string nick, string msg )
{
  //SAYPRIVATE username {message}
  assert( is_online() );
  assert( m_sock != NULL );
  
  m_sock->Send( "SAYPRIVARE " + nick + " " + msg + "\n" );
}



void TASServer::on_connected( Socket* sock )
{
  TASServer* serv = (TASServer*)sock->get_userdata();
  serv->m_last_ping = time( NULL );
  serv->m_connected = true;
  serv->m_online = false;
  cout << "** TASServer::on_connected(): on_connected event." << endl;
}


void TASServer::on_disconnected( Socket* sock )
{
  TASServer* serv = (TASServer*)sock->get_userdata();
  serv->m_connected = false;
  serv->m_online = false;
  cout << "** TASServer::on_disconnected(): on_disconnected event." << endl;
}


void TASServer::on_data_recived( Socket* sock )
{
  TASServer* serv = (TASServer*)sock->get_userdata();
  cout << "** TASServer::on_data_recived(): on_data_recived event." << endl;
  serv->_recive_and_execute();
}


////////////////////////
// Utility functions
//////////////////////

Clientstatus TASServer::conv_tasclientstatus( TASClientstatus tas )
{
  Clientstatus stat;
  stat.in_game = tas.in_game;
  stat.away = tas.away;
  stat.rank = (tas.rank + 1) * 100;
  stat.moderator = tas.moderator;
  stat.bot = tas.bot;
  return stat;
}

bool TASServer::version_support_replyid( int version )
{
  if ( version == SER_VER_0_32 )
    return false;
  else
    return true;
}
