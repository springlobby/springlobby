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


void TASServer::SetSocket( Socket* sock )
{
  Server::SetSocket( sock );
  if ( sock == NULL ) return;
    
  sock->SetConnectedCallback( OnConnected );
  sock->SetDisconnectedCallback( OnDisconnected );
  sock->SetDataRecivedCallback( OnDataRecived );
  
  sock->SetUserdata( (void*)this );
  
}

void TASServer::Connect( string addr, const int port )
{
  assert( m_sock != NULL );
  m_sock->Connect( addr, port );
  if ( IsConnected() ) {
    m_last_ping = time( NULL );
    m_connected = true;
  }
  m_online = false;
  
}

void TASServer::Disconnect()
{
  assert( m_sock != NULL );
  m_sock->Disconnect();
  m_connected = false;
}

bool TASServer::IsConnected()
{
  if ( m_sock == NULL ) return false;
  return (m_sock->State() == SS_OPEN);
}
  
void TASServer::Login()
{
  cout << "** TASServer::login()" << endl;
  string password = m_pass;
  unsigned char output[16];
  unsigned char* input = new unsigned char[ password.length() ];
  for ( int i = 0; i < password.length(); i++ )
    input[i] = password[i];
  
  md5_csum( input, password.length(), &output[0] );
  password = base64_encode( &output[0] , 16 );
  
  string data = "LOGIN ";
  data += m_user;
  data += " ";
  data += password;
  data += " 2100 * SpringLobby 0.1\n";
  m_sock->Send( data );
  
  delete input;
}

void TASServer::Logout()
{
  cout << "** TASServer::logout()" << endl;
  Disconnect();
}

bool TASServer::IsOnline()
{
  if ( !m_connected ) return false;
  return m_online;
}

void TASServer::Update()
{
  //cout << "** TASServer::update()" << endl;
  assert( m_sock != NULL ); // TODO: This should be handled and generate an error in released code.
  
  if ( !m_connected ) { // We are not formally connected yet, but might be.
    if ( IsConnected() ) {
      m_last_ping = time( NULL );
      m_connected = true;
    }
    return;
  } else { // We are connected allready.
    if ( !IsConnected() ) {
      m_connected = false;
      m_online = false;
      m_ui->OnDisconnected();
      return;
    }

    time_t now = time( NULL );
    if ( m_last_ping + m_keepalive < now ) { // Is it time for a keepalive PING?
      Ping();
    }
    HandlePinglist();
  }
  
  _ReciveAndExecute();
  
}


void TASServer::_ReciveAndExecute()
{
  string data;
  
  do {
    
    data = "";
    if ( m_sock->Recive( data ) ) {
      m_buffer += data;
      if ( m_buffer.find( "\n", 0 ) != string::npos ) {
        ExecuteCommand( m_buffer );
        m_buffer = "";
      }
    }
    
  } while ( !data.empty() ); // Go on until recive stops providing data.
}


void TASServer::ExecuteCommand( string in )
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
  
  ExecuteCommand( cmd, in );
}

void TASServer::ExecuteCommand( string cmd, string params, int replyid )
{
  int pos, cpu, status, id, nat, port, maxplayers, rank, hash, specs;
  bool replay, haspass;
  string nick, contry, host, map, title, mod, channel, error, msg;
  UserStatus cstatus;
  UTASClientstatus tasstatus;
  
  if ( cmd == "TASServer") {
    mod = GetWordParam( params );
    if ( mod == "0.32" )
      m_ser_ver = SER_VER_0_32;
    else if ( mod == "0.33" )
      m_ser_ver = SER_VER_0_32;
    else if ( mod == "0.34" )
      m_ser_ver = SER_VER_0_34;
    else
      m_ser_ver = SER_VER_BAD;
    m_ui->OnConnected( "TASServer", mod, (m_ser_ver > 0) );
    
  } else if ( cmd == "ACCEPTED" ) {
    m_online = true;
    m_ui->OnLogin( );
  } else if ( cmd == "MOTD" ) {
    m_ui->OnMotd( params );
  } else if ( cmd == "ADDUSER" ) {
    nick = GetWordParam( params );
    contry = GetWordParam( params );
    cpu = GetIntParam( params );
    m_ui->OnNewUser( nick, contry, cpu );
  } else if ( cmd == "CLIENTSTATUS" ) {
    nick = GetWordParam( params );
    tasstatus.byte = GetIntParam( params );
    cstatus = ConvTasclientstatus( tasstatus.tasdata );
    m_ui->OnUserStatus( nick, cstatus );
  } else if ( cmd == "BATTLEOPENED" ) {
    id = GetIntParam( params );
    replay = GetIntParam( params );
    nat = GetIntParam( params );
    nick = GetWordParam( params );
    host = GetWordParam( params );
    port = GetIntParam( params );
    maxplayers = GetIntParam( params );
    haspass = GetIntParam( params );
    rank = GetIntParam( params );
    hash = GetIntParam( params );
    map = GetSentenceParam( params );
    title = GetSentenceParam( params );
    mod = GetSentenceParam( params );
    
    m_ui->OnBattleOpened( id, replay, nat, nick, host, port, maxplayers, 
                            haspass, (rank + 1)*100, hash, map, title, mod );
    
  } else if ( cmd == "JOINEDBATTLE" ) {
    id = GetIntParam( params );
    nick = GetWordParam( params );
    m_ui->OnUserJoinedBattle( id, nick );
  } else if ( cmd == "UPDATEBATTLEINFO" ) {
    id = GetIntParam( params );
    specs = GetIntParam( params );
    haspass = GetIntParam( params );
    hash = GetIntParam( params );
    map = GetSentenceParam( params );
    m_ui->OnBattleInfoUpdated( id, specs, haspass, hash, map );
  } else if ( cmd == "LOGININFOEND" ) {
    m_ui->OnLoginInfoComplete();
  } else if ( cmd == "REMOVEUSER" ) {
    nick = GetWordParam( params );
    m_ui->OnUserQuit( nick );
  } else if ( cmd == "BATTLECLOSED" ) {
    id = GetIntParam( params );
    m_ui->OnBattleClosed( id );
  } else if ( cmd == "LEFTBATTLE" ) {
    id = GetIntParam( params );
    nick = GetWordParam( params );
    m_ui->OnUserLeftBattle( id, nick );
  } else if ( cmd == "PONG" ) {
    HandlePong( replyid );
  } else if ( cmd == "JOIN" ) {
    channel = GetWordParam( params );
    m_ui->OnJoinChannelResult( true, channel, "" );
  } else if ( cmd == "JOIN" ) {
    channel = GetWordParam( params );
    error = GetSentenceParam( params );
    m_ui->OnJoinChannelResult( false, channel, error );
    
  } else if ( cmd == "SAID" ) {
    channel = GetWordParam( params );
    nick = GetWordParam( params );
    msg = GetSentenceParam( params );
    m_ui->OnChannelSaid( channel, nick, msg );
  } else if ( cmd == "JOINED" ) {
    channel = GetWordParam( params );
    nick = GetWordParam( params );
    m_ui->OnChannelJoin( channel, nick );
  } else if ( cmd == "LEFT" ) {
    channel = GetWordParam( params );
    nick = GetWordParam( params );
    msg = GetSentenceParam( params );
    m_ui->OnChannelPart( channel, nick, msg );
  } else if ( cmd == "CHANNELTOPIC" ) {
    channel = GetWordParam( params );
    nick = GetWordParam( params );
    pos = GetIntParam( params );
    msg = GetSentenceParam( params );
    m_ui->OnChannelTopic( channel, nick, msg, pos/1000 );
  } else if ( cmd == "SAIDEX" ) {
    channel = GetWordParam( params );
    nick = GetWordParam( params );
    msg = GetSentenceParam( params );
    m_ui->OnChannelAction( channel, nick, msg );
  } else if ( cmd == "CLIENTS" ) {
    channel = GetWordParam( params );
    while ( (nick = GetWordParam( params )) != "" ) {
      m_ui->OnChannelJoin( channel, nick );
    }
    
    //CLIENTS params: main ChanServ []Cookiebot hawkki Pullapitko scf84 replay trepan 10gb_bot_24h Contex[1944] NowakPL Springie
    // !! Command: "CHANNELMESSAGE" params: "main <ChanServ> has muted <smoth>".
  } else {
    cout << "??? Cmd: " << cmd.c_str() << " params: " << params.c_str() << endl;
    m_ui->OnUnknownCommand( cmd, params );
  }
  
}

string TASServer::GetWordParam( string& params )
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

string TASServer::GetSentenceParam( string& params )
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

int TASServer::GetIntParam( string& params )
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

void TASServer::Ping()
{
  //cout << "** TASServer::ping()" << endl;
  string cmd = "";

  m_ping_id++;
//  cout << ">>> PING #" << _ping_id << endl;
  if ( VersionSupportReplyid( m_ser_ver ) ) {
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

void TASServer::HandlePong( int replyid )
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
    m_ui->OnPong( (time( NULL ) - it->t) );
    m_pinglist.erase( it );
  } else {
    if ( !m_pinglist.empty() ) {
      m_ui->OnPong( (time( NULL ) - m_pinglist.begin()->t) );
      m_pinglist.pop_front();
    } else {
      m_ui->OnPong( -1 );
    }
  }
}

void TASServer::HandlePinglist()
{
  list<TASPingListItem>::iterator it;
  int now = time( NULL );
  while ( !m_pinglist.empty() ) {
    if ( m_pinglist.begin()->t + PING_TIMEOUT < now ) {
      m_pinglist.pop_front();
    } else {
      break;
    }
  }
}


void TASServer::JoinChannel( string channel, string key )
{
  //JOIN channame [key]
  cout << "** TASServer::JoinChannel()" << endl;
  assert( IsOnline() );
  assert( m_sock != NULL );
  
  string cmd = "JOIN " + channel;
  if ( key != "" )
    cmd += " " + key;
  cmd += "\n";
  
  m_sock->Send( cmd );
}


void TASServer::PartChannel( string channel )
{
  //LEAVE channame
  cout << "** TASServer::PartChannel()" << endl;
  assert( IsOnline() );
  assert( m_sock != NULL );
  
  m_sock->Send( "LEAVE " + channel + "\n" );
  
}
 

void TASServer::SayChannel( string channel, string msg )
{
  //SAY channame {message}
  cout << "** TASServer::SayChannel()" << endl;
  assert( IsOnline() );
  assert( m_sock != NULL );
  
  m_sock->Send( "SAY " + channel + " " + msg + "\n" );
}


void TASServer::SayPrivate( string nick, string msg )
{
  //SAYPRIVATE username {message}
  cout << "** TASServer::SayPrivate()" << endl;
  assert( IsOnline() );
  assert( m_sock != NULL );
  
  m_sock->Send( "SAYPRIVARE " + nick + " " + msg + "\n" );
}



void TASServer::OnConnected( Socket* sock )
{
  TASServer* serv = (TASServer*)sock->GetUserdata();
  serv->m_last_ping = time( NULL );
  serv->m_connected = true;
  serv->m_online = false;
  cout << "** TASServer::OnConnected()" << endl;
}


void TASServer::OnDisconnected( Socket* sock )
{
  TASServer* serv = (TASServer*)sock->GetUserdata();
  serv->m_connected = false;
  serv->m_online = false;
  cout << "** TASServer::OnDisconnected()" << endl;
}


void TASServer::OnDataRecived( Socket* sock )
{
  TASServer* serv = (TASServer*)sock->GetUserdata();
  serv->_ReciveAndExecute();
}


////////////////////////
// Utility functions
//////////////////////

UserStatus TASServer::ConvTasclientstatus( TASClientstatus tas )
{
  UserStatus stat;
  stat.in_game = tas.in_game;
  stat.away = tas.away;
  stat.rank = (tas.rank + 1) * 100;
  stat.moderator = tas.moderator;
  stat.bot = tas.bot;
  return stat;
}

bool TASServer::VersionSupportReplyid( int version )
{
  if ( version == SER_VER_0_32 )
    return false;
  else
    return true;
}
