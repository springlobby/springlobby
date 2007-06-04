//
// Class: TASServer
//

#include <iostream>
#include <assert.h>
#include <stdio.h>
#include "tasserver.h"


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
  return pass.length() == 24 && pass[22] == '=' && pass[23] == '=';
}


std::string TASServer::GetPasswordHash( const std::string& pass )
{
  if ( IsPasswordHash(pass) ) return pass;
  return base64_encode(boost::md5(pass.c_str()).digest().value(), 16);
}


User& TASServer::GetMe()
{
  return GetUser( m_user );
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
  int pos, cpu, status, id, nat, port, maxplayers, rank, hash, specs, metal, energy, units, start;
  bool replay, haspass, dgun, ghost, dim, comm;
  std::string nick, contry, host, map, title, mod, channel, error, msg;
  NatType ntype;
  UserStatus cstatus;
  UTASClientstatus tasstatus;
  UTASBattlestatus tasbstatus;
  UserBattleStatus bstatus;
  UTASColor color;
  
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
  } else if ( cmd == "JOINBATTLE" ) {
    // JOINBATTLE battleid? startingmetal startingenergy maxunits startpos gameendcondition limitdgun diminishingMMs ghostedBuildings hashcode
    // JOINBATTLE 65096 1000 1000 500 2 0 0 0 1 -613726550
    id = GetIntParam( params );
    metal = GetIntParam( params );
    energy = GetIntParam( params );
    units = GetIntParam( params );
    start = GetIntParam( params );
    comm = (bool)GetIntParam( params );
    dgun = (bool)GetIntParam( params );
    dim = (bool)GetIntParam( params );
    ghost = (bool)GetIntParam( params );
    hash = GetIntParam( params );
    m_battle_id = id;
    m_se->OnJoinedBattle( id, metal, energy, units, IntToStartType(start), comm, dgun, dim, ghost, hash );
  } else if ( cmd == "CLIENTBATTLESTATUS" ) {
    nick = GetWordParam( params );
    tasbstatus.data = GetIntParam( params );
    bstatus = ConvTasbattlestatus( tasbstatus.tasdata );
    color.data = GetIntParam( params );
    bstatus.color_r = color.color.red;
    bstatus.color_g = color.color.green;
    bstatus.color_b = color.color.blue;
    m_se->OnClientBattleStatus( m_battle_id, nick, bstatus );
  } else {
    std::cout << "??? Cmd: " << cmd.c_str() << " params: " << params.c_str() << std::endl;
    m_se->OnUnknownCommand( cmd, params );
  }
  /*
[14:12] !! Command: "JOINBATTLEFAILED" params: "Password required".
[20:31] !! Command: "JOINBATTLE" params: "65096 1000 1000 500 2 0 0 0 1 -613726550".
[20:31] !! Command: "CLIENTBATTLESTATUS" params: "Lucypher 4195396 16734810".
[20:31] !! Command: "CLIENTBATTLESTATUS" params: "Drew11 20972544 200".
[20:31] !! Command: "REQUESTBATTLESTATUS" params: "".
[20:31] !! Command: "ADDSTARTRECT" params: "0 128 3 200 104".
[20:31] !! Command: "ADDSTARTRECT" params: "1 0 104 77 200".
[20:32] !! Command: "CLIENTBATTLESTATUS" params: "JabluchoPL 4195464 16734810".
[20:32] !! Command: "SAIDBATTLE" params: "JabluchoPL hi".
  */
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


void TASServer::JoinBattle( const int& battleid, const std::string& password )
{
  //JOINBATTLE BATTLE_ID [parameter]
  std::cout << "** TASServer::JoinBattle()" << std::endl;
  assert( IsOnline() );
  assert( m_sock != NULL );
  m_sock->Send( "JOINBATTLE " + i2s( battleid ) + " " + password + "\n" );
}


void TASServer::LeaveBattle( const int& battleid )
{
  //LEAVEBATTLE
  std::cout << "** TASServer::LeaveBattle()" << std::endl;
  assert( IsOnline() );
  assert( m_sock != NULL );
  m_sock->Send( "LEAVEBATTLE\n" );
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

UserBattleStatus TASServer::ConvTasbattlestatus( TASBattlestatus tas )
{
  UserBattleStatus stat;
  stat.ally = tas.ally;
  stat.handicap = tas.handicap;
  stat.ready = tas.ready;
  stat.side = tas.side;
  stat.spectator = tas.spectator;
  stat.sync = tas.sync;
  stat.team = tas.team;
  return stat;
}

bool TASServer::VersionSupportReplyid( int version )
{
  if ( version == SER_VER_0_32 )
    return false;
  else
    return true;
}
