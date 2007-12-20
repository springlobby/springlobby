/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: ServerEvents
//

#include <wx/intl.h>
#include <stdexcept>

#include "serverevents.h"
#include "mainwindow.h"
#include "ui.h"
#include "channel.h"
#include "user.h"
#include "utils.h"
#include "server.h"
#include "battle.h"
#include "settings.h"

void ServerEvents::OnConnected( const std::string& server_name, const std::string& server_ver, bool supported, const std::string server_spring_ver, const int udpport, bool lanmode )
{
  wxLogDebugFunc( WX_STRING(server_ver) + _T(" ") + WX_STRING(server_spring_ver) );
  m_serv.SetRequiredSpring( server_spring_ver );
  m_serv.SetUdpPort( udpport );
  m_ui.OnConnected( m_serv, server_name, server_ver, supported );
  m_serv.Login();
}


void ServerEvents::OnDisconnected()
{
  wxLogDebugFunc( _T("") );
  m_serv.SetRequiredSpring ("");
  m_serv.SetUdpPort( 0 );
  m_ui.OnDisconnected( m_serv );
}


void ServerEvents::OnLogin()
{

}


void ServerEvents::OnLoginInfoComplete()
{
  wxLogDebugFunc( _T("") );
  m_serv.JoinChannel( "springlobby", "" );
  //m_serv.RequestChannels();
  int num = sett().GetNumChannelsJoin();
  for ( int i= 0; i < num; i++ ) {
    wxString channel = WX_STRING(sett().GetChannelJoinName(i));
    wxString pass = channel.AfterFirst(' ');
    if ( !pass.IsEmpty() ) channel = channel.BeforeFirst(' ');
    m_serv.JoinChannel( STD_STRING(channel), STD_STRING(pass) );
  }
  m_ui.OnLoggedIn( );
}


void ServerEvents::OnLogout()
{
  //wxLogDebugFunc( _T("") );
}


void ServerEvents::OnUnknownCommand( const std::string& command, const std::string& params )
{
  wxLogDebugFunc( _T("") );
  m_ui.OnUnknownCommand( m_serv, command, params );
}


void ServerEvents::OnSocketError( const Sockerror& error )
{
  //wxLogDebugFunc( _T("") );
}


void ServerEvents::OnProtocolError( const Protocolerror error )
{
  //wxLogDebugFunc( _T("") );
}


void ServerEvents::OnMotd( const std::string& msg )
{
  wxLogDebugFunc( _T("") );
  m_ui.OnMotd( m_serv, msg );
}


void ServerEvents::OnPong( int ping_time )
{
  if ( ping_time == -1 ) {
    wxLogWarning( _("Ping Timeout!") );
    m_serv.Disconnect();
    OnDisconnected();
  }
}


void ServerEvents::OnNewUser( const std::string& nick, const std::string& country, int cpu )
{
  wxLogDebugFunc( _T("") );
  if ( m_serv.UserExists( nick ) ) throw std::runtime_error("New user from server, but already exists!");
  User& user = m_serv._AddUser( nick );
  user.SetCountry( country );
  user.SetCpu( cpu );
  m_ui.OnUserOnline( user );
}


void ServerEvents::OnUserStatus( const std::string& nick, UserStatus status )
{
  wxLogDebugFunc( _T("") );
  try{
    wxLogMessage( _T("calling m_serv.GetUser( nick ) ") );
    User& user = m_serv.GetUser( nick );
    wxLogMessage( _T("calling user.SetStatus( status ) ") );

    user.SetStatus( status );

    wxLogMessage( _T("calling m_ui.OnUserStatusChanged( user ) ") );
    m_ui.OnUserStatusChanged( user );
    wxLogMessage( _T("updating battles ") );

    if ( user.GetBattle() != 0 ) {
      Battle& battle = *user.GetBattle();
      if ( battle.GetFounder().GetNick() == user.GetNick() ) {
        if ( status.in_game != battle.GetInGame() ) {
          battle.SetInGame( status.in_game );
          if ( status.in_game ) m_ui.OnBattleStarted( battle );
          else m_ui.OnBattleInfoUpdated( battle );
        }
      }
    }
  }catch(...){
    wxLogWarning( _("OnUserStatus() failed ! (exception)") );
  }
}


void ServerEvents::OnUserQuit( const std::string& nick )
{
  wxLogDebugFunc( _T("") );
  m_ui.OnUserOffline( m_serv.GetUser( nick ) );
  m_serv._RemoveUser( nick );
}


void ServerEvents::OnBattleOpened( int id, bool replay, NatType nat, const std::string& nick,
                       const std::string& host, int port, int maxplayers,
                       bool haspass, int rank, std::string maphash, const std::string& map,
                       const std::string& title, const std::string& mod )
{
  wxLogDebugFunc( _T("") );

  if ( m_serv.BattleExists( id ) ) throw std::runtime_error("New battle from server, but already exists!");
  Battle& battle = m_serv._AddBattle( id );

  User& user = m_serv.GetUser( nick );
  battle.OnUserAdded( user );

  battle.SetIsReplay( replay );
  battle.SetNatType( nat );
  battle.SetFounder( nick );
  battle.SetHostIp( host );
  battle.SetHostPort( port );
  battle.SetMaxPlayers( maxplayers );
  battle.SetIsPassworded( haspass );
  battle.SetRankNeeded( rank );
  battle.SetMap( WX_STRING(map), WX_STRING(maphash) );
  battle.SetDescription( title );
  battle.SetMod( WX_STRING(mod), wxEmptyString );

  m_ui.OnBattleOpened( battle );
  if ( user.Status().in_game ) {
    battle.SetInGame( true );
    m_ui.OnBattleStarted( battle );
  }
}


void ServerEvents::OnJoinedBattle( int battleid )
{
  wxLogDebugFunc( _T("") );
  Battle& battle = m_serv.GetBattle( battleid );

  UserBattleStatus& bs = m_serv.GetMe().BattleStatus();
  bs.spectator = false;

  m_ui.OnJoinedBattle( battle );
}


void ServerEvents::OnHostedBattle( int battleid )
{
  wxLogDebugFunc( _T("") );
  Battle& battle = m_serv.GetBattle( battleid );

  UserBattleStatus& bs = m_serv.GetMe().BattleStatus();
  bs.spectator = false;

  m_ui.OnHostedBattle( battle );
}


void ServerEvents::OnStartHostedBattle( int battleid )
{
  wxLogDebugFunc( _T("") );
  Battle& battle = m_serv.GetBattle( battleid );
  battle.SetInGame( true );
  m_ui.OnBattleStarted( battle );
}


void ServerEvents::OnClientBattleStatus( int battleid, const std::string& nick, UserBattleStatus status )
{
  Battle& battle = m_serv.GetBattle( battleid );
  User& user = m_serv.GetUser( nick );
  status.color_index = user.BattleStatus().color_index;
  user.SetBattleStatus( status );
  m_ui.OnUserBattleStatus( battle, user );
}


void ServerEvents::OnUserJoinedBattle( int battleid, const std::string& nick )
{
  wxLogDebugFunc( _T("") );
  Battle& battle = m_serv.GetBattle( battleid );
  User& user = m_serv.GetUser( nick );

  battle.OnUserAdded( user );
  m_ui.OnUserJoinedBattle( battle, user );

 if ( &user == &battle.GetFounder() ) {
    if ( user.Status().in_game ) {
      battle.SetInGame( true );
      m_ui.OnBattleStarted( battle );
    }
  }
}


void ServerEvents::OnUserLeftBattle( int battleid, const std::string& nick )
{
  wxLogDebugFunc( _T("") );
  Battle& battle = m_serv.GetBattle( battleid );
  User& user = m_serv.GetUser( nick );

  battle.OnUserRemoved( user );

  m_ui.OnUserLeftBattle( battle, user );

}


void ServerEvents::OnBattleInfoUpdated( int battleid, int metal, int energy, int units, StartType
                    start, GameType gt, bool dgun, bool dim, bool ghost, std::string hash )
{
  wxLogDebugFunc( _T("") );
  Battle& battle = m_serv.GetBattle( battleid );

  battle.SetStartMetal( metal );
  battle.SetStartEnergy( energy );
  battle.SetMaxUnits( units );
  battle.SetStartType( start );
  battle.SetGameType( gt );
  battle.SetLimitDGun( dgun );
  battle.SetDimMMs( dim );
  battle.SetGhostedBuildings( ghost );
//  battle.SetH( hash );
  m_ui.OnBattleInfoUpdated( battle );
}


void ServerEvents::OnBattleInfoUpdated( int battleid, int spectators, bool locked, std::string maphash, const std::string& map )
{
  wxLogDebugFunc( _T("") );
  Battle& battle = m_serv.GetBattle( battleid );

  battle.SetSpectators( spectators );
  battle.SetIsLocked( locked );

  std::string oldmap = STD_STRING(battle.GetMapName());

  battle.SetMap( WX_STRING(map), WX_STRING(maphash) );

  if ( (oldmap != map) && (battle.UserExists( m_serv.GetMe().GetNick())) ) battle.SendMyBattleStatus();

  m_ui.OnBattleInfoUpdated( battle );
}


void ServerEvents::OnSetBattleInfo( int battleid, const std::string& param, const std::string& value )
{
  wxLogDebugFunc( _T("") );
  Battle& battle = m_serv.GetBattle( battleid );

  std::string val = value;

  // TODO: This is a temporary solution until we can dump tasserver < 0.35 support
  if      ( param == "game/startpostype"     ) battle.SetStartType( GetIntParam(val) );
  else if ( param == "game/maxunits"         ) battle.SetMaxUnits( GetIntParam(val) );
  else if ( param == "game/limitdgun"        ) battle.SetLimitDGun( GetIntParam(val) );
  else if ( param == "game/startmetal"       ) battle.SetStartMetal( GetIntParam(val) );
  else if ( param == "game/gamemode"         ) battle.SetGameType( GetIntParam(val) );
  else if ( param == "game/ghostedbuildings" ) battle.SetGhostedBuildings( GetIntParam(val) );
  else if ( param == "game/startenergy"      ) battle.SetStartEnergy( GetIntParam(val) );
  else if ( param == "game/diminishingmms"   ) battle.SetDimMMs( GetIntParam(val) );

}


void ServerEvents::OnBattleInfoUpdated( int battleid )
{
  wxLogDebugFunc( _T("") );
  Battle& battle = m_serv.GetBattle( battleid );
  m_ui.OnBattleInfoUpdated( battle );
}


void ServerEvents::OnBattleClosed( int battleid )
{
  wxLogDebugFunc( _T("") );
  Battle& battle = m_serv.GetBattle( battleid );

  m_ui.OnBattleClosed( battle );

  m_serv._RemoveBattle( battleid );
}


void ServerEvents::OnBattleDisableUnit( int battleid, const std::string& unitname )
{
  wxLogDebugFunc( _T("") );
  Battle& battle = m_serv.GetBattle( battleid );
  battle.DisableUnit( unitname );
  m_ui.OnBattleDisableUnit( battle, unitname );
}


void ServerEvents::OnBattleEnableUnit( int battleid, const std::string& unitname )
{
  wxLogDebugFunc( _T("") );
  Battle& battle = m_serv.GetBattle( battleid );
  battle.EnableUnit( unitname );
  m_ui.OnBattleEnableUnit( battle, unitname );
}


void ServerEvents::OnBattleEnableAllUnits( int battleid )
{
  wxLogDebugFunc( _T("") );
  Battle& battle = m_serv.GetBattle( battleid );
  battle.EnableAllUnits();
  m_ui.OnBattleEnableAllUnits( battle );
}


void ServerEvents::OnJoinChannelResult( bool success, const std::string& channel, const std::string& reason )
{
  wxLogDebugFunc( _T("") );
  if ( success ) {

    Channel& chan = m_serv._AddChannel( channel );
    m_ui.OnJoinedChannelSuccessful( chan );
    if ( channel == "springlobby") {
      m_serv.DoActionChannel( "springlobby", "is using SpringLobby v" + GetSpringLobbyVersion() );
    }

  } else {
    m_ui.ShowMessage( _("Join channel failed"), _("Could not join channel ") + WX_STRING(channel) + _(" because: ") + WX_STRING(reason) );
  }
}


void ServerEvents::OnChannelSaid( const std::string& channel, const std::string& who, const std::string& message )
{
  wxLogDebugFunc( _T("") );
  m_serv.GetChannel( channel ).Said( m_serv.GetUser( who ), message );
}


void ServerEvents::OnChannelJoin( const std::string& channel, const std::string& who )
{
  wxLogDebugFunc( _T("") );
  m_serv.GetChannel( channel ).Joined( m_serv.GetUser( who ) );
}


void ServerEvents::OnChannelPart( const std::string& channel, const std::string& who, const std::string& message )
{
  wxLogDebugFunc( _T("") );
  m_serv.GetChannel( channel ).Left( m_serv.GetUser( who ), message );
}


void ServerEvents::OnChannelTopic( const std::string& channel, const std::string& who, const std::string& message, int when )
{
  wxLogDebugFunc( _T("") );
  m_serv.GetChannel( channel ).SetTopic( message, who );
}


void ServerEvents::OnChannelAction( const std::string& channel, const std::string& who, const std::string& action )
{
  wxLogDebugFunc( _T("") );
  m_serv.GetChannel( channel ).DidAction( m_serv.GetUser( who ), action );
}


void ServerEvents::OnPrivateMessage( const std::string& user, const std::string& message, bool fromme )
{
  wxLogDebugFunc( _T("") );
  User& who = m_serv.GetUser( user );
  m_ui.OnUserSaid( who, message, fromme );

}

void ServerEvents::OnChannelList( const std::string& channel, const int& numusers )
{
  m_ui.OnChannelList( channel, numusers );
}


void ServerEvents::OnRequestBattleStatus( int battleid )
{
  Battle& battle = m_serv.GetBattle( battleid );
  m_ui.OnRequestBattleStatus( battle );
}


void ServerEvents::OnSaidBattle( int battleid, const std::string& nick, const std::string& msg )
{
  Battle& battle = m_serv.GetBattle( battleid );
  m_ui.OnSaidBattle( battle, nick, msg );
}

void ServerEvents::OnBattleAction( int battleid, const std::string& nick, const std::string& msg )
{
  Battle& battle = m_serv.GetBattle( battleid );
  m_ui.OnBattleAction( battle, nick, msg );
}


void ServerEvents::OnBattleStartRectAdd( int battleid, int allyno, int left, int top, int right, int bottom )
{
  Battle& battle = m_serv.GetBattle( battleid );
  battle.AddStartRect( allyno, left, top, right, bottom );
  m_ui.OnBattleStartRectsUpdated( battle );
}


void ServerEvents::OnBattleStartRectRemove( int battleid, int allyno )
{
  Battle& battle = m_serv.GetBattle( battleid );
  battle.RemoveStartRect( allyno );
  m_ui.OnBattleStartRectsUpdated( battle );
}


void ServerEvents::OnBattleAddBot( int battleid, const std::string& nick, const std::string& owner, UserBattleStatus status, const std::string& aidll )
{
  wxLogDebugFunc( _T("") );
  Battle& battle = m_serv.GetBattle( battleid );
  battle.OnBotAdded( nick, owner, status, aidll );
  BattleBot* bot = battle.GetBot( nick );
  ASSERT_LOGIC( bot != 0, _T("Bot null after add.") );
  m_ui.OnBattleBotAdded( battle, *bot );
}


void ServerEvents::OnBattleUpdateBot( int battleid, const std::string& nick, UserBattleStatus status )
{
  wxLogDebugFunc( _T("") );
  Battle& battle = m_serv.GetBattle( battleid );
  battle.OnBotUpdated( nick, status );
  BattleBot* bot = battle.GetBot( nick );
  ASSERT_LOGIC( bot != 0, _T("Bot null after add.") );
  m_ui.OnBattleBotUpdated( battle, *bot );
}


void ServerEvents::OnBattleRemoveBot( int battleid, const std::string& nick )
{
  wxLogDebugFunc( _T("") );
  Battle& battle = m_serv.GetBattle( battleid );
  BattleBot* bot = battle.GetBot( nick );
  ASSERT_LOGIC( bot != 0, _T("Bot null after add.") );
  m_ui.OnBattleBotRemoved( battle, *bot );
  battle.OnBotRemoved( nick );
}


void ServerEvents::OnAcceptAgreement( const std::string& agreement )
{
  m_ui.OnAcceptAgreement( agreement );
}


void ServerEvents::OnRing( const std::string& from )
{
  m_ui.OnRing( from );
}


void ServerEvents::OnServerMessage( const std::string& message )
{
  m_ui.OnServerMessage( m_serv, message );
}


void ServerEvents::OnChannelMessage( const std::string& channel, const std::string& msg )
{
  m_ui.OnChannelMessage( channel, msg );
}


void ServerEvents::OnHostUdpPortChange( const int& udpport )
{
  if ( !m_serv.GetCurrentBattle() ) return;
  if ( m_serv.GetCurrentBattle()->GetNatType() == NAT_Hole_punching || m_serv.GetCurrentBattle()->GetNatType() == NAT_Fixed_source_ports ) m_serv.GetCurrentBattle()->SetHostPort( udpport );
}

void ServerEvents::OnUdpSourcePort(int udpport){
  if ( !m_serv.GetCurrentBattle() ) return;
  m_serv.GetCurrentBattle()->SetExternalUdpSourcePort(udpport);
}
