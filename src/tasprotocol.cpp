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
// Class: TASProtocol
//
// Created by:  <>
// Created on: Fri Apr 27 19:25:15 2007
//

#include <iostream>
#include <assert.h>
#include <stdio.h>
#include "tasprotocol.h"
#include "md5.h"
#include "base64.h"
#include "socket.h"
#include "uicontrol.h"

TASProtocol::TASProtocol() : Protocol()
{
  _connected = false;
  _online = false;
  _buffer = "";
  _last_ping = 0;
  _ping_id = 1000;
  _ser_ver = SER_VER_UNKNOWN;
}


TASProtocol::~TASProtocol()
{

}

void TASProtocol::connect( string addr, const int port )
{
  assert( _sock != NULL );
  if ( !_sock->connect( addr, port ) ) {
    // TODO: Handle errors
  }
  if ( is_connected() ) {
    _last_ping = time( NULL );
    _connected = true;
  }
  _online = false;
  
}

void TASProtocol::disconnect()
{
  assert( _sock != NULL );
  if ( !_sock->disconnect() ) {
    // TODO: Handle errors
  }
  _connected = false;
}

bool TASProtocol::is_connected()
{
  if ( _sock == NULL ) return false;
  return (_sock->state() == SS_OPEN);
}
  
void TASProtocol::login( string username, string password )
{
  unsigned char output[16];
  unsigned char* input = new unsigned char[ password.length() ];
  for ( int i = 0; i < password.length(); i++ )
    input[i] = password[i];
  
  md5_csum( input, password.length(), &output[0] );
  password = base64_encode( &output[0] , 16 );
  //cout << password.c_str() << endl;
  
  string data = "LOGIN ";
  data += username;
  data += " ";
  data += password;
  data += " 2100 * TKALCL 0.001\n";
  _sock->send( data );
  
  delete input;
}

void TASProtocol::logout()
{
  disconnect();
}

bool TASProtocol::is_online()
{
  if ( !_connected ) return false;
  return _online;
}

void TASProtocol::update()
{
  assert( _sock != NULL ); // TODO: This should be handled and generate an error in released code.
  string data = ".";
  
  if ( !_connected ) { // We are not formally connected yet, but might be.
    if ( is_connected() ) {
      _last_ping = time( NULL );
      _connected = true;
    }
    return;
  } else { // We are connected allready.
    if ( !is_connected() ) {
      _connected = false;
      _online = false;
      _ui->on_disconnected();
      return;
    }

    time_t now = time( NULL );
    if ( _last_ping + _keepalive < now ) { // Is it time for a keepalive PING?
      ping();
    }
    handle_pinglist();
  }
  
  while ( !data.empty() ) { // Go on until recive stops providing data.
    
    data = "";
    if ( _sock->recive( data ) ) {
      _buffer += data;
      if ( _buffer.find( "\n", 0 ) != string::npos ) {
        execute_command( _buffer );
        _buffer = "";
      }
    }
    
  }
}

void TASProtocol::execute_command( string in )
{
  string cmd;
  int pos;
  int replyid = -1;
  
  if ( in.empty() ) return;
  
  //cout << "Debug: " << in << endl;
  if ( in[0] == '#' ) {
    cmd = in.substr( 1, in.find( " ", 0 ) );
    replyid = str2int( cmd );
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

void TASProtocol::execute_command( string cmd, string params, int replyid )
{
  int pos, cpu, status, id, nat, port, maxplayers, rank, hash, specs;
  bool replay, haspass;
  string nick, contry, host, map, title, mod;
  Clientstatus cstatus;
  UTASClientstatus tasstatus;
  
  if ( cmd == "TASServer") {
    mod = get_word_param( params );
    if ( mod == "0.32" )
      _ser_ver = SER_VER_0_32;
    else if ( mod == "0.33" )
      _ser_ver = SER_VER_0_32;
    else if ( mod == "0.34" )
      _ser_ver = SER_VER_0_34;
    else
      _ser_ver = SER_VER_BAD;
    _ui->on_connected( mod, (_ser_ver > 0) );
    
  } else if ( cmd == "ACCEPTED" ) {
    _online = true;
    _ui->on_login( );
  } else if ( cmd == "MOTD" ) {
    _ui->on_motd( params );
  } else if ( cmd == "ADDUSER" ) {
    nick = get_word_param( params );
    contry = get_word_param( params );
    cpu = get_int_param( params );
    _ui->on_new_user( nick, contry, cpu );
  } else if ( cmd == "CLIENTSTATUS" ) {
    nick = get_word_param( params );
    tasstatus.byte = get_int_param( params );
    cstatus = conv_tasclientstatus( tasstatus.tasdata );
    _ui->on_user_status( nick, cstatus );
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
    
    _ui->on_battle_opened( id, replay, nat, nick, host, port, maxplayers, haspass, (rank + 1)*100, hash, map, title, mod );
    
  } else if ( cmd == "JOINEDBATTLE" ) {
    id = get_int_param( params );
    nick = get_word_param( params );
    _ui->on_user_joined_battle( id, nick );
  } else if ( cmd == "UPDATEBATTLEINFO" ) {
    id = get_int_param( params );
    specs = get_int_param( params );
    haspass = get_int_param( params );
    hash = get_int_param( params );
    map = get_sentence_param( params );
    _ui->on_battleinfo_updated( id, specs, haspass, hash, map );
  } else if ( cmd == "LOGININFOEND" ) {
    _ui->on_login_info_complete();
  } else if ( cmd == "REMOVEUSER" ) {
    nick = get_word_param( params );
    _ui->on_user_quit( nick );
  } else if ( cmd == "BATTLECLOSED" ) {
    id = get_int_param( params );
    _ui->on_battle_closed( id );
  } else if ( cmd == "LEFTBATTLE" ) {
    id = get_int_param( params );
    nick = get_word_param( params );
    _ui->on_user_left_battle( id, nick );
  } else if ( cmd == "PONG" ) {
    handle_pong( replyid );
  } else {
    cout << "??? Cmd: " << cmd.c_str() << " params: " << params.c_str() << endl;
  }
  
}

string TASProtocol::get_word_param( string& params )
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

string TASProtocol::get_sentence_param( string& params )
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

int TASProtocol::get_int_param( string& params )
{
  int pos;
  string param;
  
  pos = params.find( " ", 0 );
  if ( pos == string::npos ) {
    param = params;
    params = "";
    return str2int( param );
  } else {
    param = params.substr( 0, pos );
    params = params.substr( pos + 1 );
    return str2int( param );
  }
}

void TASProtocol::ping()
{
  string cmd = "";

  _ping_id++;
//  cout << ">>> PING #" << _ping_id << endl;
  if ( version_support_replyid( _ser_ver ) ) {
    cmd += "#";
    cmd += _ping_id;
    cmd += " PING\n";
  } else {
    cmd += "PING ";
    cmd += _ping_id;
    cmd += "\n";
  }
  
  _sock->send( cmd );
  
  TASPingListItem pli;
  pli.id = _ping_id;
  pli.t = time( NULL );
  _pinglist.push_back ( pli );
  _last_ping = time( NULL );
}

void TASProtocol::handle_pong( int replyid )
{
  list<TASPingListItem>::iterator it;
  
//  cout << ">>> PONG #" << replyid << endl;
  bool found = false;
  for ( it = _pinglist.begin(); it != _pinglist.end(); it++ ) {
    if (it->id == replyid ) {
      found = true;
      break;
    }
  }
  
  if ( found ) {
    _ui->on_pong( (time( NULL ) - it->t) );
    _pinglist.erase( it );
  } else {
    if ( !_pinglist.empty() ) {
      _ui->on_pong( (time( NULL ) - _pinglist.begin()->t) );
      _pinglist.pop_front();
    } else {
      _ui->on_pong( -1 );
    }
  }
}

void TASProtocol::handle_pinglist()
{
  list<TASPingListItem>::iterator it;
  int now = time( NULL );
  while ( !_pinglist.empty() ) {
    if ( _pinglist.begin()->t + PING_TIMEOUT < now ) {
      // TODO: Fix Ping time!
      //cout << "!!! Ping Timeout, took more than " << PING_TIMEOUT << "s" << endl;
      //_ui->on_pong( PING_TIMEOUT );
      _pinglist.pop_front();
    } else {
      break;
    }
  }
}


void TASProtocol::join_channel( string channel, string key )
{
  //JOIN channame [key]
  assert( is_online() );
  assert( _sock != NULL );
  
  string cmd = "JOIN " + channel;
  if ( key != "" )
    cmd += " " + key;
  cmd += "\n";
  
  _sock->send( cmd );
}


void TASProtocol::part_channel( string channel )
{
  //LEAVE channame
  assert( is_online() );
  assert( _sock != NULL );
  
  _sock->send( "LEAVE " + channel + "\n" );
  
}
 

void TASProtocol::say_channel( string channel, string msg )
{
  //SAY channame {message}
  assert( is_online() );
  assert( _sock != NULL );
  
  _sock->send( "SAY " + channel + " " + msg + "\n" );
}


void TASProtocol::say_private( string nick, string msg )
{
  //SAYPRIVATE username {message}
  assert( is_online() );
  assert( _sock != NULL );
  
  _sock->send( "SAYPRIVARE " + nick + " " + msg + "\n" );
}



////////////////////////
// Utility functions
//////////////////////

int TASProtocol::str2int( string s )
{
  assert( !s.empty() );
  int i;
  sscanf ( s.c_str(),"%i", &i );
  return i;
}

Clientstatus TASProtocol::conv_tasclientstatus( TASClientstatus tas )
{
  Clientstatus stat;
  stat.in_game = tas.in_game;
  stat.away = tas.away;
  stat.rank = (tas.rank + 1) * 100;
  stat.moderator = tas.moderator;
  stat.bot = tas.bot;
  return stat;
}

bool TASProtocol::version_support_replyid( int version )
{
  if ( version == SER_VER_0_32 )
    return false;
  else
    return true;
}
