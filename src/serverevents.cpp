//
// Class: ServerEvents
//

#include "serverevents.h"
#include "mainwindow.h"
#include "ui.h"


void ServerEvents::OnConnected( const std::string& server_name, const std::string& server_ver, bool supported )
{
  debug_func( server_ver );
  m_ui.OnConnected( m_serv, server_name, server_ver, supported );
  m_serv.Login();
}


void ServerEvents::OnDisconnected()
{
  debug_func( "" );
  m_ui.OnDisconnected( m_serv );
}


void ServerEvents::OnLogin()
{
  //debug_func( "" );
}


void ServerEvents::OnLoginInfoComplete()
{
  debug_func( "" );
  m_serv.JoinChannel( "springlobby", "" );
  m_serv.RequestChannels();
}


void ServerEvents::OnLogout()
{
  //debug_func( "" );
}


void ServerEvents::OnUnknownCommand( const std::string& command, const std::string& params )
{
  debug_func( "" );
  m_ui.OnUnknownCommand( m_serv, command, params );
}


void ServerEvents::OnSocketError( const Sockerror error )
{
  //debug_func( "" );
}


void ServerEvents::OnProtocolError( const Protocolerror error )
{
  //debug_func( "" );
}


void ServerEvents::OnMotd( const std::string& msg )
{
  debug_func( "" );
  m_ui.OnMotd( m_serv, msg );
}


void ServerEvents::OnPong( int ping_time )
{
  //debug_func( "" );
}


void ServerEvents::OnNewUser( const std::string& nick, const std::string& country, int cpu )
{
  debug_func( "" );
  if ( m_serv.UserExists( nick ) ) throw std::runtime_error("New user from server, but already exists!");
  User& user = m_serv._AddUser( nick );
  user.SetCountry( country );
  user.SetCpu( cpu );
  m_ui.OnUserOnline( user );
}


void ServerEvents::OnUserStatus( const std::string& nick, UserStatus status )
{
  //debug_func( "" );
  User& user = m_serv.GetUser( nick );
  UserStatus us = user.GetStatus();

  user.SetStatus( status );
  m_ui.OnUserStatusChanged( user );

  if ( !us.in_game && status.in_game && user.GetBattle() != NULL ) {
    Battle& battle = *user.GetBattle();
    if ( &battle.GetFounder() == &user ) 
      m_ui.OnBattleStarted( battle );
  }
}


void ServerEvents::OnUserQuit( const std::string& nick )
{
  debug_func( "" );
  m_ui.OnUserOffline( m_serv.GetUser( nick ) );
  m_serv._RemoveUser( nick );
}


void ServerEvents::OnBattleOpened( int id, bool replay, NatType nat, const std::string& nick, 
                       const std::string& host, int port, int maxplayers, 
                       bool haspass, int rank, std::string maphash, const std::string& map, 
                       const std::string& title, const std::string& mod )
{
  debug_func( "" );
  
  if ( m_serv.BattleExists( id ) ) throw std::runtime_error("New battle from server, but already exists!");
  Battle& battle = m_serv._AddBattle( id );
  
  User& user = m_serv.GetUser( nick );
  battle.AddUser( user );
  
  battle.SetIsReplay( replay );
  battle.SetNatType( nat );
  battle.SetFounder( nick );
  battle.SetIp( host );
  battle.SetPort( port );
  battle.SetMaxPlayers( maxplayers );
  battle.SetIsPassworded( haspass );
  battle.SetRankNeeded( rank );
  battle.SetMapHash( maphash );
  battle.SetMapname( map );
  battle.SetDescription( title );
  battle.SetModname( mod );
  
  m_ui.OnBattleOpened( battle ); 
}


void ServerEvents::OnJoinedBattle( int battleid )
{
  debug_func( "" );
  Battle& battle = m_serv.GetBattle( battleid );

  UserBattleStatus bs = m_serv.GetMe().GetBattleStatus();
  bs.spectator = false;
  m_serv.GetMe().SetBattleStatus( bs );

  m_ui.OnJoinedBattle( battle );
}


void ServerEvents::OnClientBattleStatus( int battleid, const std::string& nick, UserBattleStatus status )
{
  Battle& battle = m_serv.GetBattle( battleid );
  User& user = m_serv.GetUser( nick );
  status.color_index = user.GetBattleStatus().color_index;
  user.SetBattleStatus( status );
  m_ui.OnUserBattleStatus( battle, user );
}


void ServerEvents::OnUserJoinedBattle( int battleid, const std::string& nick )
{
  debug_func( "" );
  Battle& battle = m_serv.GetBattle( battleid );
  User& user = m_serv.GetUser( nick );
  
  battle.AddUser( user );
  m_ui.OnUserJoinedBattle( battle, user );
}


void ServerEvents::OnUserLeftBattle( int battleid, const std::string& nick )
{
  debug_func( "" );
  Battle& battle = m_serv.GetBattle( battleid );
  User& user = m_serv.GetUser( nick );
  
  m_ui.OnUserLeftBattle( battle, user );

  battle.RemoveUser( user );
}


void ServerEvents::OnBattleInfoUpdated( int battleid, int metal, int energy, int units, StartType 
                    start, bool comm, bool dgun, bool dim, bool ghost, std::string hash )
{
  debug_func( "" );
  Battle& battle = m_serv.GetBattle( battleid );

  battle.SetStartMetal( metal );
  battle.SetStartEnergy( energy );
  battle.SetMaxUnits( units );
  battle.SetStartType( start );
  battle.SetComEndsGame( comm );
  battle.SetLimitDGun( dgun );
  battle.SetDimMMs( dim );
  battle.SetGhostedBuildings( ghost );
//  battle.SetH( hash );
  m_ui.OnBattleInfoUpdated( battle );
}


void ServerEvents::OnBattleInfoUpdated( int battleid, int spectators, bool locked, std::string maphash, const std::string& map )
{
  debug_func( "" );
  Battle& battle = m_serv.GetBattle( battleid );
  
  battle.SetSpectators( spectators );
  battle.SetIsLocked( locked );
    
  battle.SetMapHash( maphash );
  battle.SetMapname( map );
  
  m_ui.OnBattleInfoUpdated( battle );
}


void ServerEvents::OnBattleClosed( int battleid )
{
  debug_func( "" );
  Battle& battle = m_serv.GetBattle( battleid );
  
  m_ui.OnBattleClosed( battle );
  
  m_serv._RemoveBattle( battleid );
}


void ServerEvents::OnBattleDisableUnit( int battleid, const std::string& unitname )
{
  Battle& battle = m_serv.GetBattle( battleid );
  battle.DisableUnit( unitname );
  m_ui.OnBattleDisableUnit( battle, unitname );
}


void ServerEvents::OnBattleEnableUnit( int battleid, const std::string& unitname )
{
  Battle& battle = m_serv.GetBattle( battleid );
  battle.EnableUnit( unitname );
  m_ui.OnBattleEnableUnit( battle, unitname );
}


void ServerEvents::OnBattleEnableAllUnits( int battleid )
{
  Battle& battle = m_serv.GetBattle( battleid );
  battle.EnableAllUnits();
  m_ui.OnBattleEnableAllUnits( battle );
}


void ServerEvents::OnJoinChannelResult( bool success, const std::string& channel, const std::string& reason )
{
  debug_func( "" );
  if ( success ) {

    Channel& chan = m_serv._AddChannel( channel );
    m_ui.OnJoinedChannelSuccessful( chan );
  
  } else {
    m_ui.ShowMessage( _T("Join channel failed"), _T("Could not join channel ") + WX_STRING(channel) + _T(" because: ") + WX_STRING(reason) );
  }
}


void ServerEvents::OnChannelSaid( const std::string& channel, const std::string& who, const std::string& message )
{
  debug_func( "" );
  m_serv.GetChannel( channel ).Said( m_serv.GetUser( who ), message );
}


void ServerEvents::OnChannelJoin( const std::string& channel, const std::string& who )
{
  debug_func( "" );
  m_serv.GetChannel( channel ).Joined( m_serv.GetUser( who ) );
}


void ServerEvents::OnChannelPart( const std::string& channel, const std::string& who, const std::string& message )
{
  debug_func( "" );
  m_serv.GetChannel( channel ).Left( m_serv.GetUser( who ), message );
}


void ServerEvents::OnChannelTopic( const std::string& channel, const std::string& who, const std::string& message, int when )
{
  debug_func( "" );
  m_serv.GetChannel( channel ).SetTopic( message, who );
}


void ServerEvents::OnChannelAction( const std::string& channel, const std::string& who, const std::string& action )
{
  debug_func( "" );
  m_serv.GetChannel( channel ).DidAction( m_serv.GetUser( who ), action );
}


void ServerEvents::OnPrivateMessage( const std::string& user, const std::string& message, bool fromme )
{
  debug_func( "" ); 
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


