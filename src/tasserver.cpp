//
// Class: TASServer
//

#include <iostream>
#include <assert.h>
#include <stdio.h>
#include "tasserver.h"
#include "md5.h"
#include "base64.h"
#include "socket.h"
#include "serverevents.h"


void TASServer::SetSocket( Socket* sock )
{
  Server::SetSocket( sock );
  if ( sock == NULL ) return;
    
  //sock->SetConnectedCallback( OnConnected );
  //sock->SetDisconnectedCallback( OnDisconnected );
  //sock->SetDataRecivedCallback( OnDataRecived );
  
  //sock->SetUserdata( (void*)this );
  
}

void TASServer::Connect( const std::string& addr, const int port )
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
  

bool TASServer::IsPasswordHash( const std::string& pass )
{
  std::cout << "** TASServer::IsPasswordHash(): " << pass.c_str() << "  " << pass.length() << std::endl;
  if ( pass.length() != 24 ) return false;
  std::cout << "** TASServer::IsPasswordHash(): " << pass.c_str() << "  " << pass.length() << std::endl;
  if ( pass[22] != '=' ) return false;
  std::cout << "** TASServer::IsPasswordHash(): " << pass.c_str() << "  " << pass.length() << std::endl;
  if ( pass[23] != '=' ) return false;
  std::cout << "** TASServer::IsPasswordHash(): " << pass.c_str() << "  " << pass.length() << std::endl;
  return true;
}


std::string TASServer::GetPasswordHash( const std::string& pass )
{
  if ( IsPasswordHash(pass) ) return pass;
    
  std::string password = pass;
  unsigned char output[16];
  unsigned char* input = new unsigned char[ password.length() ];
  for ( int i = 0; i < password.length(); i++ )
    input[i] = password[i];
  
  md5_csum( input, password.length(), &output[0] );
  password = base64_encode( &output[0] , 16 );
  
  delete[] input;
  
  return password;
}


void TASServer::Login()
{
  std::cout << "** TASServer::login()" << std::endl;

  std::string data = "LOGIN ";
  data += m_user;
  data += " ";
  data += GetPasswordHash( m_pass );
  data += " 2100 * SpringLobby 0.1\n";
  m_sock->Send( data );
  
}

void TASServer::Logout()
{
  std::cout << "** TASServer::logout()" << std::endl;
  Disconnect();
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
      m_se->OnDisconnected();
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
  std::string data;
  
  do {
    
    data = "";
    if ( m_sock->Recive( data ) ) {
      m_buffer += data;
      if ( m_buffer.find( "\n", 0 ) != std::string::npos ) {
        ExecuteCommand( m_buffer );
        m_buffer = "";
      }
    }
    
  } while ( !data.empty() ); // Go on until recive stops providing data.
}


void TASServer::ExecuteCommand( std::string in )
{
  std::string cmd;
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
  if ( pos != std::string::npos ) {
    assert( pos < in.length() ); // We might be throwing away a second command following this one.
    in.replace( pos, in.length() - pos, "" );
  }
    
  pos = in.find( " ", 0 );
  if ( pos == std::string::npos ) {
    // Must be command without parameters.
    cmd = in;
    in = "";
  } else {
    cmd = in.substr( 0, pos );
    in = in.substr( pos + 1 );
  }
  
  ExecuteCommand( cmd, in );
}

void TASServer::ExecuteCommand( std::string cmd, std::string params, int replyid )
{
  int pos, cpu, status, id, nat, port, maxplayers, rank, hash, specs;
  bool replay, haspass;
  std::string nick, contry, host, map, title, mod, channel, error, msg;
  NatType ntype;
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
    m_se->OnConnected( "TASServer", mod, (m_ser_ver > 0) );
    
  } else if ( cmd == "ACCEPTED" ) {
    m_online = true;
    m_se->OnLogin( );
  } else if ( cmd == "MOTD" ) {
    m_se->OnMotd( params );
  } else if ( cmd == "ADDUSER" ) {
    nick = GetWordParam( params );
    contry = GetWordParam( params );
    cpu = GetIntParam( params );
    m_se->OnNewUser( nick, contry, cpu );
  } else if ( cmd == "CLIENTSTATUS" ) {
    nick = GetWordParam( params );
    tasstatus.byte = GetIntParam( params );
    cstatus = ConvTasclientstatus( tasstatus.tasdata );
    m_se->OnUserStatus( nick, cstatus );
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
    
    //! @todo Fix nat settings
    
    m_se->OnBattleOpened( id, replay, NAT_None, nick, host, port, maxplayers, 
                            haspass, (rank + 1)*100, hash, map, title, mod );
    
  } else if ( cmd == "JOINEDBATTLE" ) {
    id = GetIntParam( params );
    nick = GetWordParam( params );
    m_se->OnUserJoinedBattle( id, nick );
  } else if ( cmd == "UPDATEBATTLEINFO" ) {
    id = GetIntParam( params );
    specs = GetIntParam( params );
    haspass = GetIntParam( params );
    hash = GetIntParam( params );
    map = GetSentenceParam( params );
    m_se->OnBattleInfoUpdated( id, specs, haspass, hash, map );
  } else if ( cmd == "LOGININFOEND" ) {
    m_se->OnLoginInfoComplete();
  } else if ( cmd == "REMOVEUSER" ) {
    nick = GetWordParam( params );
    m_se->OnUserQuit( nick );
  } else if ( cmd == "BATTLECLOSED" ) {
    id = GetIntParam( params );
    m_se->OnBattleClosed( id );
  } else if ( cmd == "LEFTBATTLE" ) {
    id = GetIntParam( params );
    nick = GetWordParam( params );
    m_se->OnUserLeftBattle( id, nick );
  } else if ( cmd == "PONG" ) {
    HandlePong( replyid );
  } else if ( cmd == "JOIN" ) {
    channel = GetWordParam( params );
    m_se->OnJoinChannelResult( true, channel, "" );
  } else if ( cmd == "JOIN" ) {
    channel = GetWordParam( params );
    error = GetSentenceParam( params );
    m_se->OnJoinChannelResult( false, channel, error );
    
  } else if ( cmd == "SAID" ) {
    channel = GetWordParam( params );
    nick = GetWordParam( params );
    msg = GetSentenceParam( params );
    m_se->OnChannelSaid( channel, nick, msg );
  } else if ( cmd == "JOINED" ) {
    channel = GetWordParam( params );
    nick = GetWordParam( params );
    m_se->OnChannelJoin( channel, nick );
  } else if ( cmd == "LEFT" ) {
    channel = GetWordParam( params );
    nick = GetWordParam( params );
    msg = GetSentenceParam( params );
    m_se->OnChannelPart( channel, nick, msg );
  } else if ( cmd == "CHANNELTOPIC" ) {
    std::cout << "** TOPIC " << params << std::endl;
    channel = GetWordParam( params );
    nick = GetWordParam( params );
    pos = GetIntParam( params );
    msg = GetSentenceParam( params );
    m_se->OnChannelTopic( channel, nick, msg, pos/1000 );
  } else if ( cmd == "SAIDEX" ) {
    channel = GetWordParam( params );
    nick = GetWordParam( params );
    msg = GetSentenceParam( params );
    m_se->OnChannelAction( channel, nick, msg );
  } else if ( cmd == "CLIENTS" ) {
    channel = GetWordParam( params );
    while ( (nick = GetWordParam( params )) != "" ) {
      m_se->OnChannelJoin( channel, nick );
    }
  } else if ( cmd == "SAIDPRIVATE" ) {
    nick = GetWordParam( params );
    msg = GetSentenceParam( params );
    m_se->OnPrivateMessage( nick, msg );
    //"SAIDPRIVATE" params: "CAutohost TEST2W ---> http://spring.unknown-files.net/file/2273" 
    // !! Command: "CHANNELMESSAGE" params: "main <ChanServ> has muted <smoth>".
  } else {
    std::cout << "??? Cmd: " << cmd.c_str() << " params: " << params.c_str() << std::endl;
    m_se->OnUnknownCommand( cmd, params );
  }
  
}

std::string TASServer::GetWordParam( std::string& params )
{
  int pos;
  std::string param;
  
  pos = params.find( " ", 0 );
  if ( pos == std::string::npos ) {
    param = params;
    params = "";
    return param;
  } else {
    param = params.substr( 0, pos );
    params = params.substr( pos + 1 );
    return param;
  }
}

std::string TASServer::GetSentenceParam( std::string& params )
{
  int pos;
  std::string param;
  
  pos = params.find( "\t", 0 );
  if ( pos == std::string::npos ) {
    param = params;
    params = "";
    return param;
  } else {
    param = params.substr( 0, pos );
    params = params.substr( pos + 1 );
    return param;
  }
}

int TASServer::GetIntParam( std::string& params )
{
  int pos;
  std::string param;
  
  pos = params.find( " ", 0 );
  if ( pos == std::string::npos ) {
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
  std::string cmd = "";

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
  std::list<TASPingListItem>::iterator it;
  
//  cout << ">>> PONG #" << replyid << endl;
  bool found = false;
  for ( it = m_pinglist.begin(); it != m_pinglist.end(); it++ ) {
    if (it->id == replyid ) {
      found = true;
      break;
    }
  }
  
  if ( found ) {
    m_se->OnPong( (time( NULL ) - it->t) );
    m_pinglist.erase( it );
  } else {
    if ( !m_pinglist.empty() ) {
      m_se->OnPong( (time( NULL ) - m_pinglist.begin()->t) );
      m_pinglist.pop_front();
    } else {
      m_se->OnPong( -1 );
    }
  }
}

void TASServer::HandlePinglist()
{
  std::list<TASPingListItem>::iterator it;
  int now = time( NULL );
  while ( !m_pinglist.empty() ) {
    if ( m_pinglist.begin()->t + PING_TIMEOUT < now ) {
      m_pinglist.pop_front();
    } else {
      break;
    }
  }
}


void TASServer::JoinChannel( const std::string& channel, const std::string& key )
{
  //JOIN channame [key]
  std::cout << "** TASServer::JoinChannel(" << channel.c_str() << ")" << std::endl;
  assert( IsOnline() );
  assert( m_sock != NULL );
  
  std::string cmd = "JOIN " + channel;
  if ( key != "" )
    cmd += " " + key;
  cmd += "\n";
  
  m_sock->Send( cmd );
}


void TASServer::PartChannel( const std::string& channel )
{
  //LEAVE channame
  std::cout << "** TASServer::PartChannel()" << std::endl;
  assert( IsOnline() );
  assert( m_sock != NULL );
  
  m_sock->Send( "LEAVE " + channel + "\n" );
  
}
 

void TASServer::SayChannel( const std::string& channel, const std::string& msg )
{
  //SAY channame {message}
  std::cout << "** TASServer::SayChannel()" << std::endl;
  assert( IsOnline() );
  assert( m_sock != NULL );
  
  m_sock->Send( "SAY " + channel + " " + msg + "\n" );
}


void TASServer::SayPrivate( const std::string& nick, const std::string& msg )
{
  //SAYPRIVATE username {message}
  std::cout << "** TASServer::SayPrivate()" << std::endl;
  assert( IsOnline() );
  assert( m_sock != NULL );
  
  m_sock->Send( "SAYPRIVARE " + nick + " " + msg + "\n" );
}



void TASServer::OnConnected( Socket* sock )
{
  std::cout << "** TASServer::OnConnected()" << std::endl;
  //TASServer* serv = (TASServer*)sock->GetUserdata();
  m_last_ping = time( NULL );
  m_connected = true;
  m_online = false;
}


void TASServer::OnDisconnected( Socket* sock )
{
  std::cout << "** TASServer::OnDisconnected()" << std::endl;
  //TASServer* serv = (TASServer*)sock->GetUserdata();
  m_connected = false;
  m_online = false;
}


void TASServer::OnDataRecived( Socket* sock )
{
  //TASServer* serv = (TASServer*)sock->GetUserdata();
  _ReciveAndExecute();
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
  if ( stat.rank > RANK_4 ) stat.rank = RANK_4;
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
