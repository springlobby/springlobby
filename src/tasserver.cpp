/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: TASServer
//

#include <cassert>
#include <stdexcept>

#include "base64.h"
#include "boost/md5.hpp"
#include "tasserver.h"
#include "iunitsync.h"
#include "user.h"
#include "utils.h"
#include "battle.h"
#include "serverevents.h"
#include "socket.h"

#define SER_VER_BAD -1
#define SER_VER_UNKNOWN 0
#define SER_VER_0_32 1
#define SER_VER_0_33 -2
#define SER_VER_0_34 -3

//! @brief Struct used internally by the TASServer class to get client status information.
struct TASClientstatus {
  unsigned int in_game   : 1;
  unsigned int away      : 1;
  unsigned int rank      : 3;
  unsigned int moderator : 1;
  unsigned int bot       : 1;
};


//! @brief Union used internally by the TASServer class to get client status information.
union UTASClientStatus {
  unsigned char byte;
  TASClientstatus tasdata;
};


//! @brief Struct used internally by the TASServer class to get battle status information.
struct TASBattleStatus {
  unsigned int : 1;
  unsigned int ready : 1;
  unsigned int team : 4;
  unsigned int ally : 4;
  unsigned int player : 1;
  unsigned int handicap: 7;
  unsigned int : 4;
  unsigned int sync : 2;
  unsigned int side : 4;
  unsigned int : 4;
};


//! @brief Union used internally by the TASServer class to get battle status information.
union UTASBattleStatus {
  int data;
  TASBattleStatus tasdata;
};


struct TASColor {
  unsigned int red : 8;
  unsigned int green : 8;
  unsigned int blue : 8;
  unsigned int zero: 8;
};


union UTASColor {
  int data;
  TASColor color;
};


/*

myteamcolor:  Should be 32-bit signed integer in decimal form (e.g. 255 and not FF) where each color channel should occupy 1 byte (e.g. in hexdecimal: $00BBGGRR, B = blue, G = green, R = red). Example: 255 stands for $000000FF.

*/

UserStatus ConvTasclientstatus( TASClientstatus );
UserBattleStatus ConvTasbattlestatus( TASBattleStatus );
TASBattleStatus ConvTasbattlestatus( UserBattleStatus );
bool VersionSupportReplyid( int version );
StartType IntToStartType( int start );
NatType IntToNatType( int nat );
GameType IntToGameType( int gt );

TASServer::TASServer( Ui& ui ): Server(ui), m_ui(ui), m_ser_ver(SER_VER_UNKNOWN), m_connected(false), m_online(false), m_buffer(""), m_last_ping(0), m_ping_id(1000),m_battle_id(-1) { m_se = new ServerEvents( *this, ui); }

TASServer::~TASServer() { delete m_se; }

void TASServer::SetSocket( Socket* sock )
{
  Server::SetSocket( sock );
  if ( sock == 0 ) return;

  //sock->SetConnectedCallback( OnConnected );
  //sock->SetDisconnectedCallback( OnDisconnected );
  //sock->SetDataRecivedCallback( OnDataRecived );

  //sock->SetUserdata( (void*)this );

}

void TASServer::Connect( const std::string& addr, const int port )
{
  assert( m_sock != 0 );
  m_sock->Connect( addr, port );
  if ( IsConnected() ) {
    m_last_ping = time( 0 );
    m_connected = true;
  }
  m_online = false;
  m_agreement = "";
}

void TASServer::Disconnect()
{
  assert( m_sock != 0 );
  m_sock->Disconnect();
  m_connected = false;
}

bool TASServer::IsConnected()
{
  if ( m_sock == 0 ) return false;
  return (m_sock->State() == SS_OPEN);
}


bool TASServer::Register( const std::string& addr, const int port, const std::string& nick, const std::string& password )
{
  debug_func( "" );

  assert( m_sock != 0 );
  m_sock->Connect( addr, port );
  if ( !IsConnected() ) return false;

  std::string data;
  m_sock->Recive( data );
  if ( GetWordParam( data ) != "TASServer" ) return false;

  data = "REGISTER ";
  data += nick;
  data += " ";
  data += GetPasswordHash( password );
  data += "\n";
  m_sock->Send( data );
  data = "";

  m_sock->Recive( data );
  if ( data != "REGISTRATIONACCEPTED\n") return false;

  return true;
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
  debug_func( "" );

  std::string data = "LOGIN ";
  data += m_user;
  data += " ";
  data += GetPasswordHash( m_pass );
  data += " 2100 * SpringLobby 0.1\n";
  m_sock->Send( data );

}

void TASServer::Logout()
{
  debug_func( "" );
  Disconnect();
}

bool TASServer::IsOnline() {
  if ( !m_connected ) return false;
  return m_online;
}


void TASServer::RequestChannels()
{
  m_sock->Send( "CHANNELS\n" );
}


void TASServer::AcceptAgreement()
{
  m_sock->Send( "CONFIRMAGREEMENT\n" );
}


void TASServer::Update()
{
  //debug_func( "" );
  assert( m_sock != 0 ); // TODO: This should be handled and generate an error in released code.

  if ( !m_connected ) { // We are not formally connected yet, but might be.
    if ( IsConnected() ) {
      m_last_ping = time( 0 );
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

    time_t now = time( 0 );
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
        std::string cmd = m_buffer.substr( 0, m_buffer.find( "\n", 0 ) );
        m_buffer = m_buffer.substr( m_buffer.find( "\n", 0 )+1, m_buffer.length() );
        ExecuteCommand( cmd );
      }
    }

  } while ( !data.empty() ); // Go on until recive stops providing data.
}


void TASServer::ExecuteCommand( const std::string& in )
{
  std::string cmd;
  std::string params = in;
  std::string::size_type pos = 0;
  int replyid = -1;

  if ( in.empty() ) return;

  if ( params[0] == '#' ) {
    cmd = params.substr( 1, params.find( " ", 0 ) );
    replyid = atoi( cmd.c_str() );
    params = params.substr( pos + 1 );
  }

  pos = params.find( "\n", 0 );
  if ( pos != std::string::npos ) {
    assert( pos < params.length() ); // We might be throwing away a second command following this one.
    params.replace( pos, params.length() - pos, "" );
  }

  pos = params.find( " ", 0 );
  if ( pos == std::string::npos ) {
    // Must be command without parameters.
    cmd = params;
    params = "";
  } else {
    cmd = params.substr( 0, pos );
    params = params.substr( pos + 1 );
  }

  ExecuteCommand( cmd, params );
}


void TASServer::ExecuteCommand( const std::string& cmd, const std::string& inparams, int replyid )
{
  std::string params = inparams;
  int pos, cpu, id, nat, port, maxplayers, rank, specs, metal, energy, units, start,
      top, left, right, bottom, ally;
  bool replay, haspass, dgun, ghost, dim;
  GameType gt;
  std::string hash;
  std::string nick, contry, host, map, title, mod, channel, error, msg, owner, ai, supported_spring_version;
  //NatType ntype;
  UserStatus cstatus;
  UTASClientStatus tasstatus;
  UTASBattleStatus tasbstatus;
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
    supported_spring_version = GetWordParam( params );
    m_se->OnConnected( "TAS Server", mod, (m_ser_ver > 0), supported_spring_version );

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
    replay = (bool)GetIntParam( params );
    nat = GetIntParam( params );
    nick = GetWordParam( params );
    host = GetWordParam( params );
    port = GetIntParam( params );
    maxplayers = GetIntParam( params );
    haspass = (bool)GetIntParam( params );
    rank = GetIntParam( params );
    hash = GetWordParam( params );
    map = GetSentenceParam( params );
    title = GetSentenceParam( params );
    mod = GetSentenceParam( params );
    m_se->OnBattleOpened( id, replay, IntToNatType( nat ), nick, host, port, maxplayers,
                            haspass, (rank + 1)*100, hash, map, title, mod );
  } else if ( cmd == "JOINEDBATTLE" ) {
    id = GetIntParam( params );
    nick = GetWordParam( params );
    m_se->OnUserJoinedBattle( id, nick );
  } else if ( cmd == "UPDATEBATTLEINFO" ) {
    id = GetIntParam( params );
    specs = GetIntParam( params );
    haspass = (bool)GetIntParam( params );
    hash = GetWordParam( params );
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
  } else if ( cmd == "SAYPRIVATE" ) {
    nick = GetWordParam( params );
    msg = GetSentenceParam( params );
    m_se->OnPrivateMessage( nick, msg, true );
  } else if ( cmd == "SAIDPRIVATE" ) {
    nick = GetWordParam( params );
    msg = GetSentenceParam( params );
    m_se->OnPrivateMessage( nick, msg, false );
  } else if ( cmd == "JOINBATTLE" ) {
    id = GetIntParam( params );
    metal = GetIntParam( params );
    energy = GetIntParam( params );
    units = GetIntParam( params );
    start = GetIntParam( params );
    gt = IntToGameType( GetIntParam( params ) );
    dgun = (bool)GetIntParam( params );
    dim = (bool)GetIntParam( params );
    ghost = (bool)GetIntParam( params );
    hash = GetWordParam( params );
    m_battle_id = id;
    m_se->OnJoinedBattle( id );
    m_se->OnBattleInfoUpdated( m_battle_id, metal, energy, units, IntToStartType(start), gt, dgun, dim, ghost, hash );
  } else if ( cmd == "UPDATEBATTLEDETAILS" ) {
    metal = GetIntParam( params );
    energy = GetIntParam( params );
    units = GetIntParam( params );
    start = GetIntParam( params );
    gt = IntToGameType( GetIntParam( params ) );
    dgun = (bool)GetIntParam( params );
    dim = (bool)GetIntParam( params );
    ghost = (bool)GetIntParam( params );
    hash = GetWordParam( params );
    m_se->OnBattleInfoUpdated( m_battle_id, metal, energy, units, IntToStartType(start), gt, dgun, dim, ghost, hash );
    //UPDATEBATTLEDETAILS startingmetal startingenergy maxunits startpos gameendcondition limitdgun diminishingMMs ghostedBuildings
  } else if ( cmd == "CLIENTBATTLESTATUS" ) {
    nick = GetWordParam( params );
    tasbstatus.data = GetIntParam( params );
    bstatus = ConvTasbattlestatus( tasbstatus.tasdata );
    color.data = GetIntParam( params );
    bstatus.color_r = color.color.red;
    bstatus.color_g = color.color.green;
    bstatus.color_b = color.color.blue;
    m_se->OnClientBattleStatus( m_battle_id, nick, bstatus );
  } else if ( cmd == "ADDSTARTRECT" ) {
    //ADDSTARTRECT allyno left top right bottom
    ally = GetIntParam( params );
    left = GetIntParam( params );
    top = GetIntParam( params );
    right = GetIntParam( params );
    bottom = GetIntParam( params );;
    m_se->OnBattleStartRectAdd( m_battle_id, ally, left, top, right, bottom );
  } else if ( cmd == "REMOVESTARTRECT" ) {
    //REMOVESTARTRECT allyno
    ally = GetIntParam( params );
    m_se->OnBattleStartRectRemove( m_battle_id, ally );
  } else if ( cmd == "ENABLEALLUNITS" ) {
    //"ENABLEALLUNITS" params: "".
    m_se->OnBattleEnableAllUnits( m_battle_id );
  } else if ( cmd == "ENABLEUNITS" ) {
    //ENABLEUNITS unitname1 unitname2
    while ( (nick = GetWordParam( params )) != "" ) {
      m_se->OnBattleEnableUnit( m_battle_id, nick );
    }
  } else if ( cmd == "DISABLEUNITS" ) {
    //"DISABLEUNITS" params: "arm_advanced_radar_tower arm_advanced_sonar_station arm_advanced_torpedo_launcher arm_dragons_teeth arm_energy_storage arm_eraser arm_fark arm_fart_mine arm_fibber arm_geothermal_powerplant arm_guardian"
    while ( (nick = GetWordParam( params )) != "" ) {
      m_se->OnBattleDisableUnit( m_battle_id, nick );
    }
  } else if ( cmd == "CHANNEL" ) {
    channel = GetWordParam( params );
    units = GetIntParam( params );
    m_se->OnChannelList( channel, units );
  } else if ( cmd == "ENDOFCHANNELS" ) {
    //Cmd: ENDOFCHANNELS params:
  } else if ( cmd == "REQUESTBATTLESTATUS" ) {
    m_se->OnRequestBattleStatus( m_battle_id );
  } else if ( cmd == "SAIDBATTLE" ) {
    nick = GetWordParam( params );
    msg = GetSentenceParam( params );
    m_se->OnSaidBattle( m_battle_id, nick, msg );
  } else if ( cmd == "SAIDBATTLEEX" ) {
    nick = GetWordParam( params );
    msg = GetSentenceParam( params );
    m_se->OnBattleAction( m_battle_id, nick, msg );
  } else if ( cmd == "AGREEMENT" ) {
    msg = GetSentenceParam( params );
    m_agreement += msg;
    m_agreement += "\n";
  } else if ( cmd == "AGREEMENTEND" ) {
    m_se->OnAcceptAgreement( m_agreement );
    m_agreement = "";
  } else if ( cmd == "OPENBATTLE" ) {
    m_battle_id = GetIntParam( params );
    m_se->OnHostedBattle( m_battle_id );
  } else if ( cmd == "ADDBOT" ) {
    // ADDBOT BATTLE_ID name owner battlestatus teamcolor {AIDLL}
    id = GetIntParam( params );
    nick = GetWordParam( params );
    owner = GetWordParam( params );
    tasbstatus.data = GetIntParam( params );
    bstatus = ConvTasbattlestatus( tasbstatus.tasdata );
    color.data = GetIntParam( params );
    bstatus.color_r = color.color.red;
    bstatus.color_g = color.color.green;
    bstatus.color_b = color.color.blue;
    ai = GetSentenceParam( params );
    ai = ai.substr( 0, ai.find_last_of( "." ) );
    m_se->OnBattleAddBot( id, nick, owner, bstatus, ai );
  } else if ( cmd == "UPDATEBOT" ) {
    id = GetIntParam( params );
    nick = GetWordParam( params );
    tasbstatus.data = GetIntParam( params );
    bstatus = ConvTasbattlestatus( tasbstatus.tasdata );
    color.data = GetIntParam( params );
    bstatus.color_r = color.color.red;
    bstatus.color_g = color.color.green;
    bstatus.color_b = color.color.blue;
    m_se->OnBattleUpdateBot( id, nick, bstatus );
    //UPDATEBOT BATTLE_ID name battlestatus teamcolor
  } else if ( cmd == "REMOVEBOT" ) {
    id = GetIntParam( params );
    nick = GetWordParam( params );
    m_se->OnBattleRemoveBot( id, nick );
    //REMOVEBOT BATTLE_ID name
  } else if ( cmd == "RING" ) {
    nick = GetWordParam( params );
    m_se->OnRing( nick );
    //RING username
  } else {
    debug( "??? Cmd: " + cmd + " params: " + params );
    m_se->OnUnknownCommand( cmd, params );
  }
}


void TASServer::Ping()
{
  //debug_func( "" );
  std::string cmd = "";

  m_ping_id++;
  if ( VersionSupportReplyid( m_ser_ver ) ) {
    //cmd += "#";
    //cmd += m_ping_id;
    cmd += " PING\n";
  } else {
    cmd += "PING ";
    cmd += m_ping_id;
    cmd += "\n";
  }

  m_sock->Send( cmd );

  TASPingListItem pli;
  pli.id = m_ping_id;
  pli.t = time( 0 );
  m_pinglist.push_back ( pli );
  m_last_ping = time( 0 );
}


void TASServer::HandlePong( int replyid )
{
  std::list<TASPingListItem>::iterator it;

  bool found = false;
  for ( it = m_pinglist.begin(); it != m_pinglist.end(); it++ ) {
    if (it->id == replyid ) {
      found = true;
      break;
    }
  }

  if ( found ) {
    m_se->OnPong( (time( 0 ) - it->t) );
    m_pinglist.erase( it );
  } else {
    if ( !m_pinglist.empty() ) {
      m_se->OnPong( (time( 0 ) - m_pinglist.begin()->t) );
      m_pinglist.pop_front();
    } else {
      m_se->OnPong( -1 );
    }
  }
}


void TASServer::HandlePinglist()
{
  std::list<TASPingListItem>::iterator it;
  int now = time( 0 );
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
  debug_func( channel );
  assert( IsOnline() );
  assert( m_sock != 0 );

  std::string cmd = "JOIN " + channel;
  if ( key != "" )
    cmd += " " + key;
  cmd += "\n";

  m_sock->Send( cmd );
}


void TASServer::PartChannel( const std::string& channel )
{
  //LEAVE channame
  debug_func( "channel" );
  assert( IsOnline() );
  assert( m_sock != 0 );

  m_sock->Send( "LEAVE " + channel + "\n" );

}


void TASServer::DoActionChannel( const std::string& channel, const std::string& msg )
{
  //SAYEX channame {message}
  debug_func( "" );
  assert( IsOnline() );
  assert( m_sock != 0 );

  m_sock->Send( "SAYEX " + channel + " " + msg + "\n" );
}


void TASServer::SayChannel( const std::string& channel, const std::string& msg )
{
  //SAY channame {message}
  debug_func( "" );
  assert( IsOnline() );
  assert( m_sock != 0 );

  m_sock->Send( "SAY " + channel + " " + msg + "\n" );
}


void TASServer::SayPrivate( const std::string& nick, const std::string& msg )
{
  //SAYPRIVATE username {message}
  debug_func( "" );
  assert( IsOnline() );
  assert( m_sock != 0 );

  m_sock->Send( "SAYPRIVATE " + nick + " " + msg + "\n" );
}


void TASServer::SayBattle( int battleid, const std::string& msg )
{
  debug_func( "" );
  assert( IsOnline() );
  assert( m_sock != 0 );
  m_sock->Send( "SAYBATTLE " + msg + "\n" );
}


void TASServer::DoActionBattle( int battleid, const std::string& msg )
{
  debug_func( "" );
  assert( IsOnline() );
  assert( m_sock != 0 );
  m_sock->Send( "SAYBATTLEEX " + msg + "\n" );
}


void TASServer::Ring( const std::string& nick )
{
  debug_func( "" );
  assert( IsOnline() );
  assert( m_sock != 0 );
  m_sock->Send( "RING " + nick + "\n" );
}


void TASServer::HostBattle( BattleOptions bo, const std::string& password )
{
  debug_func( "" );
  assert( IsOnline() );
  assert( m_sock != 0 );

  wxString cmd = wxString::Format( _T("OPENBATTLE 0 %d "), bo.nattype );
  cmd += (password=="")?_T("*"):WX_STRING(password);
  cmd += wxString::Format( _T(" %d %d %d %d %d %d %d %d %d %d "),
    bo.port,
    bo.maxplayers,
    bo.startmetal,
    bo.startenergy,
    bo.maxunits,
    bo.starttype,
    bo.gametype,
    bo.limitdgun,
    bo.dimmms,
    bo.ghostedbuildings
  );
  cmd += WX_STRING(bo.modhash);
  cmd += wxString::Format( _T(" %d "), bo.rankneeded );
  cmd += WX_STRING( bo.maphash ) + _T(" ");
  cmd += WX_STRING( bo.mapname ) + _T("\t");
  cmd += WX_STRING( bo.description ) + _T("\t");
  cmd += WX_STRING( bo.modname ) + _T("\n");

  m_sock->Send( STD_STRING(cmd) );
  // OPENBATTLE type natType password port maxplayers startingmetal startingenergy maxunits startpos
  // gameendcondition limitdgun diminishingMMs ghostedBuildings hashcode rank maphash {map} {title} {modname}
}


void TASServer::JoinBattle( const int& battleid, const std::string& password )
{
  //JOINBATTLE BATTLE_ID [parameter]
  debug_func( "" );
  assert( IsOnline() );
  assert( m_sock != 0 );
  m_sock->Send( "JOINBATTLE " + i2s( battleid ) + " " + password + "\n" );
}


void TASServer::LeaveBattle( const int& battleid )
{
  //LEAVEBATTLE
  debug_func( "" );
  assert( IsOnline() );
  assert( m_sock != 0 );
  m_sock->Send( "LEAVEBATTLE\n" );
}


void TASServer::SendHostInfo( HostInfo update )
{
  debug_func( "" );
  assert( IsOnline() );
  assert( m_sock != 0 );

  Battle& battle = GetBattle( m_battle_id );
  assert( battle.IsFounderMe() );

  //BattleOptions bo = battle.opts();

  if ( ( update & ( HI_Map | HI_Locked | HI_Spectators ) ) > 0 ) {
    // UPDATEBATTLEINFO SpectatorCount locked maphash {mapname}
    wxString cmd = _T("UPDATEBATTLEINFO");
    cmd += wxString::Format( _T(" %d %d "), battle.GetSpectators(), battle.IsLocked() );
    cmd += battle.GetMapHash() + _T(" ");
    cmd += battle.GetMapName() + _T("\n");

    m_sock->Send( STD_STRING(cmd) );
  }
  if ( ( update & (HI_StartResources|HI_MaxUnits|HI_StartType|HI_GameType|HI_Options) ) > 0 ) {
    // UPDATEBATTLEDETAILS startingmetal startingenergy maxunits startpos gameendcondition limitdgun diminishingMMs ghostedBuildings
    wxString cmd = _T("UPDATEBATTLEDETAILS");
    cmd += wxString::Format( _T(" %d %d %d %d %d %d %d %d\n"),
      battle.GetStartMetal(), battle.GetStartEnergy(), battle.GetMaxUnits(), battle.GetStartType(), battle.GetGameType(),
      battle.LimitDGun(), battle.DimMMs(), battle.GhostedBuildings()
    );

    m_sock->Send( STD_STRING(cmd) );
  }

  if ( update & HI_StartRects ) { // Startrects should be updated.

    for ( int i = 0; i < 16; i++ ) { // Loop through all, and remove updated or deleted.

      wxString cmd;
      BattleStartRect* sr = battle.GetStartRect( i );
      if ( sr == 0 ) continue;
      if ( sr->deleted ) {
        cmd = wxString::Format( _T("REMOVESTARTRECT %d\n"), i );
        m_sock->Send( STD_STRING(cmd) );
        battle.StartRectRemoved( i );
      } else if ( !sr->local && sr->updated ) {
        cmd = wxString::Format( _T("REMOVESTARTRECT %d\n"), i );
        m_sock->Send( STD_STRING(cmd) );
      }

    }

    for ( int i = 0; i < 16; i++ ) { // Loop through all, and update.
      wxString cmd;
      BattleStartRect* sr = battle.GetStartRect( i );
      if ( sr == 0 ) continue;
      if ( sr->updated ) {
        cmd = wxString::Format( _T("ADDSTARTRECT %d %d %d %d %d\n"), sr->ally, sr->left, sr->top, sr->right, sr->bottom );
        m_sock->Send( STD_STRING(cmd) );
        battle.StartRectUpdated( i );
      }
    }

  }
}


Battle* TASServer::GetCurrentBattle()
{
  assert( m_battle_id != -1 );
  return &GetBattle( m_battle_id );
}


void TASServer::SendMyBattleStatus( UserBattleStatus& bs )
{
  debug_func( "" );
  assert( IsOnline() );
  assert( m_sock != 0 );
  GetMe().SetBattleStatus( bs );

  UTASBattleStatus tasbs;
  tasbs.tasdata = ConvTasbattlestatus( bs );
  UTASColor tascl;
  tascl.color.red = bs.color_r;
  tascl.color.green = bs.color_g;
  tascl.color.blue = bs.color_b;
  tascl.color.zero = 0;
  //MYBATTLESTATUS battlestatus myteamcolor
  m_sock->Send( "MYBATTLESTATUS " + i2s( tasbs.data ) + " " + i2s( tascl.data ) + "\n" );
}


void TASServer::SendMyUserStatus()
{
  debug_func( "" );
  assert( IsOnline() );
  assert( m_sock != 0 );
  UserStatus& us = GetMe().Status();

  UTASClientStatus taus;
  taus.tasdata.in_game = us.in_game;
  taus.tasdata.away = us.away;
  taus.tasdata.rank = us.rank;
  taus.tasdata.moderator = us.moderator;
  taus.tasdata.bot = us.bot;

  m_sock->Send( "MYSTATUS " + i2s( taus.byte ) + "\n" );
}


void TASServer::StartHostedBattle()
{
  assert( m_battle_id != -1 );
  m_se->OnStartHostedBattle( m_battle_id );
}


void TASServer::ForceSide( int battleid, const std::string& nick, int side )
{
  debug_func( "" );
  assert( battleid == m_battle_id );
  assert( IsOnline() );
  assert( m_sock != 0 );

  if ( nick == GetMe().GetNick() ) {
    GetMe().BattleStatus().side = side;
    SendMyBattleStatus( GetMe().BattleStatus() );
  } else {
    DoActionBattle( battleid, "sugests that " + nick + " changes to " + usync()->GetSideName( STD_STRING(GetBattle(battleid).GetModName()), side ) + " side." );
  }
}


void TASServer::ForceTeam( int battleid, const std::string& nick, int team )
{
  debug_func( "" );
  assert( battleid == m_battle_id );
  assert( IsOnline() );
  assert( m_sock != 0 );

  if ( !GetBattle(battleid).IsFounderMe() ) {
    if ( nick == GetMe().GetNick() ) {
      GetMe().BattleStatus().team = team;
      SendMyBattleStatus( GetMe().BattleStatus() );
    } else {
      DoActionBattle( battleid, "sugests that " + nick + " changes to team #" + i2s( team + 1 ) + "." );
    }
    return;
  }

  //FORCETEAMNO username teamno
  m_sock->Send( "FORCETEAMNO " + nick + " " + i2s( team ) + "\n" );
}


void TASServer::ForceAlly( int battleid, const std::string& nick, int ally )
{
  debug_func( "" );
  assert( battleid == m_battle_id );
  assert( IsOnline() );
  assert( m_sock != 0 );

  if ( !GetBattle(battleid).IsFounderMe() ) {
    if ( nick == GetMe().GetNick() ) {
      GetMe().BattleStatus().ally = ally;
      SendMyBattleStatus( GetMe().BattleStatus() );
    } else {
      DoActionBattle( battleid, "sugests that " + nick + " changes to ally #" + i2s( ally + 1 ) + "." );
    }
    return;
  }

  //FORCEALLYNO username teamno
  m_sock->Send( "FORCEALLYNO " + nick + " " + i2s( ally ) + "\n" );
}


void TASServer::ForceColour( int battleid, const std::string& nick, int r, int g, int b )
{
  debug_func( "" );
  assert( battleid == m_battle_id );
  assert( IsOnline() );
  assert( m_sock != 0 );

  if ( !GetBattle(battleid).IsFounderMe() ) {
    if ( nick == GetMe().GetNick() ) {
      GetMe().BattleStatus().color_r = r;
      GetMe().BattleStatus().color_g = g;
      GetMe().BattleStatus().color_b = b;
      SendMyBattleStatus( GetMe().BattleStatus() );
    } else {
      DoActionBattle( battleid, "sugests that " + nick + " changes colour." );
    }
    return;
  }

  UTASColor tascl;
  tascl.color.red = r;
  tascl.color.green = g;
  tascl.color.blue = b;
  tascl.color.zero = 0;
  //FORCETEAMCOLOR username color
  m_sock->Send( "FORCETEAMCOLOR " + nick + " " + i2s( tascl.data ) + "\n" );
}


void TASServer::ForceSpectator( int battleid, const std::string& nick, bool spectator )
{
  debug_func( "" );

  if ( !GetBattle(battleid).IsFounderMe()) {
    if ( nick == GetMe().GetNick() ) {
      GetMe().BattleStatus().spectator = spectator;
      SendMyBattleStatus( GetMe().BattleStatus() );
    } else {
      if ( spectator ) DoActionBattle( battleid, "sugests that " + nick + " becomes a spectator." );
      else DoActionBattle( battleid, "sugests that " + nick + " plays." );
    }
    return;
  }

  if ( !spectator ) {
    if ( nick == GetMe().GetNick() ) {
      GetMe().BattleStatus().spectator = spectator;
      SendMyBattleStatus( GetMe().BattleStatus() );
    } else {
      DoActionBattle( battleid, "sugests that " + nick + " plays." );
    }
    return;
  }

  assert( battleid == m_battle_id );
  assert( IsOnline() );
  assert( m_sock != 0 );

  //FORCESPECTATORMODE username
  m_sock->Send( "FORCESPECTATORMODE " + nick + "\n" );
}


void TASServer::BattleKickPlayer( int battleid, const std::string& nick )
{
  debug_func( "" );
  assert( battleid == m_battle_id );
  assert( IsOnline() );
  assert( m_sock != 0 );

  if ( !GetBattle(battleid).IsFounderMe() ) {
    if ( nick == GetMe().GetNick() ) {
      LeaveBattle( battleid );
    } else {
      DoActionBattle( battleid, "thinks " + nick + " should leave." );
    }
    return;
  }

  //KICKFROMBATTLE username
  m_sock->Send( "KICKFROMBATTLE " + nick + "\n" );
}


void TASServer::AddBot( int battleid, const std::string& nick, const std::string& owner, UserBattleStatus status, const std::string& aidll )
{
  debug_func( "" );
  assert( IsOnline() );
  assert( m_sock != 0 );

  UTASBattleStatus tasbs;
  tasbs.tasdata = ConvTasbattlestatus( status );
  UTASColor tascl;
  tascl.color.red = status.color_r;
  tascl.color.green = status.color_g;
  tascl.color.blue = status.color_b;
  tascl.color.zero = 0;
  //ADDBOT name battlestatus teamcolor {AIDLL}
  std::string cmd = "ADDBOT " + nick + " " + i2s( tasbs.data ) + " " + i2s( tascl.data ) + " " + aidll + ".dll\n";
  debug( cmd );
  m_sock->Send( cmd );
}


void TASServer::RemoveBot( int battleid, const std::string& nick )
{
  debug_func( "" );
  assert( IsOnline() );
  assert( m_sock != 0 );

  Battle& battle = GetBattle( battleid );
  BattleBot* bot = battle.GetBot( nick );
  ASSERT_LOGIC( bot != 0, "Bot does not exist." );

/*  if (!( battle.IsFounderMe() || ( bot->owner == GetMe().GetNick() ) )) {
    DoActionBattle( battleid, "thinks the bot " + nick + " should be removed." );
    return;
  }*/

  //REMOVEBOT name
  m_sock->Send( "REMOVEBOT " + nick + "\n" );
}


void TASServer::UpdateBot( int battleid, const std::string& nick, UserBattleStatus status )
{
  debug_func( "" );
  assert( IsOnline() );
  assert( m_sock != 0 );

  UTASBattleStatus tasbs;
  tasbs.tasdata = ConvTasbattlestatus( status );
  UTASColor tascl;
  tascl.color.red = status.color_r;
  tascl.color.green = status.color_g;
  tascl.color.blue = status.color_b;
  tascl.color.zero = 0;
  //UPDATEBOT name battlestatus teamcolor
  std::string cmd = "UPDATEBOT " + nick + " " + i2s( tasbs.data ) + " " + i2s( tascl.data ) + "\n";
  debug( cmd );
  m_sock->Send( cmd );
}


void TASServer::OnConnected( Socket* sock )
{
  debug_func( "" );
  //TASServer* serv = (TASServer*)sock->GetUserdata();
  m_last_ping = time( 0 );
  m_connected = true;
  m_online = false;
}


void TASServer::OnDisconnected( Socket* sock )
{
  debug_func( "" );
  m_connected = false;
  m_online = false;
  m_se->OnDisconnected();
}


void TASServer::OnDataRecived( Socket* sock )
{
  //TASServer* serv = (TASServer*)sock->GetUserdata();
  _ReciveAndExecute();
}


////////////////////////
// Utility functions
//////////////////////

UserStatus ConvTasclientstatus( TASClientstatus tas )
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

UserBattleStatus ConvTasbattlestatus( TASBattleStatus tas )
{
  UserBattleStatus stat;
  stat.ally = tas.ally;
  stat.handicap = tas.handicap;
  stat.ready = (tas.ready==1)?true:false;
  stat.side = tas.side;
  stat.spectator = (tas.player == 0)?true:false;
  stat.sync = tas.sync;
  stat.team = tas.team;
  return stat;
}


TASBattleStatus ConvTasbattlestatus( UserBattleStatus bs)
{
  TASBattleStatus stat;
  stat.ally = bs.ally;
  stat.handicap = bs.handicap;
  stat.ready = bs.ready?1:0;
  stat.side = bs.side;
  stat.player = bs.spectator?0:1;
  stat.sync = bs.sync;
  stat.team = bs.team;
  return stat;
}


bool VersionSupportReplyid( int version )
{
  if ( version == SER_VER_0_32 )
    return false;
  else
    return true;
}

StartType IntToStartType( int start )
{
  switch ( start ) {
    case 0: return ST_Fixed;
    case 1: return ST_Random;
    case 2: return ST_Choose;
    default: assert(false);
  };
  return ST_Fixed;
}


NatType IntToNatType( int nat )
{
  switch ( nat ) {
    case 0: return NAT_None;
    case 1: return NAT_Hole_punching;
    case 2: return NAT_Fixed_source_ports;
    default: assert(false);
  };
  return NAT_None;
}


GameType IntToGameType( int gt )
{
  switch ( gt ) {
    case 0: return GT_ComContinue;
    case 1: return GT_ComEnds;
    case 2: return GT_Lineage;
    default: assert(false);
  };
  return GT_ComContinue;
}
