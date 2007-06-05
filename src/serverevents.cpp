//
// Class: ServerEvents
//

#include "serverevents.h"
#include "mainwindow.h"
#include "ui.h"


void ServerEvents::OnConnected( const std::string& server_name, const std::string& server_ver, bool supported )
{
  std::cout << "** ServerEvents::OnConnected(): Server: " << server_ver.c_str() << std::endl;
  m_ui.OnConnected( m_serv, server_name, server_ver, supported );
  m_serv.Login();
}


void ServerEvents::OnDisconnected()
{
  //std::cout << "** ServerEvents::OnDisconnected()" << std::endl;
}


void ServerEvents::OnLogin()
{
  //std::cout << "** ServerEvents::OnLogin()" << std::endl;
}


void ServerEvents::OnLoginInfoComplete()
{
  std::cout << "** ServerEvents::OnLoginInfoComplete()" << std::endl;
  m_serv.JoinChannel( "springlobby", "" );
  m_serv.RequestChannels();
}


void ServerEvents::OnLogout()
{
  //std::cout << "** ServerEvents::OnLogout()" << std::endl;
}


void ServerEvents::OnUnknownCommand( const std::string& command, const std::string& params )
{
  std::cout << "** ServerEvents::OnUnknownCommand()" << std::endl;
  m_ui.OnUnknownCommand( m_serv, command, params );
}


void ServerEvents::OnSocketError( const Sockerror error )
{
  //std::cout << "** ServerEvents::OnSocketError()" << std::endl;
}


void ServerEvents::OnProtocolError( const Protocolerror error )
{
  //std::cout << "** ServerEvents::OnProtocolError()" << std::endl;
}


void ServerEvents::OnMotd( const std::string& msg )
{
  std::cout << "** ServerEvents::OnMotd()" << std::endl;
  m_ui.OnMotd( m_serv, msg );
}


void ServerEvents::OnPong( int ping_time )
{
  //std::cout << "** ServerEvents::OnPong()" << std::endl;
}


void ServerEvents::OnNewUser( const std::string& nick, const std::string& country, int cpu )
{
  std::cout << "** OnNewUser::OnUserStatus()" << std::endl;
  if ( m_serv.UserExists( nick ) ) throw std::runtime_error("New user from server, but already exists!");
  User& user = m_serv._AddUser( nick );
  user.SetCountry( country );
  user.SetCpu( cpu );
  m_ui.OnUserOnline( user );
}


void ServerEvents::OnUserStatus( const std::string& nick, UserStatus status )
{
  //std::cout << "** ServerEvents::OnUserStatus()" << std::endl;
  User& user = m_serv.GetUser( nick );
  user.SetStatus( status );
  m_ui.OnUserStatusChanged( user );
}


void ServerEvents::OnUserQuit( const std::string& nick )
{
  std::cout << "** ServerEvents::OnUserQuit()" << std::endl;
  m_ui.OnUserOffline( m_serv.GetUser( nick ) );
  m_serv._RemoveUser( nick );
}


void ServerEvents::OnBattleOpened( int id, bool replay, NatType nat, const std::string& nick, 
                       const std::string& host, int port, int maxplayers, 
                       bool haspass, int rank, int maphash, const std::string& map, 
                       const std::string& title, const std::string& mod )
{
  std::cout << "** ServerEvents::OnBattleOpened()" << std::endl;
  
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


void ServerEvents::OnJoinedBattle( int battleid, int metal, int energy, int units, StartType 
                    start, bool comm, bool dgun, bool dim, bool ghost, int hash )
{
  std::cout << "** ServerEvents::OnJoinedBattle()" << std::endl;
  Battle& battle = m_serv.GetBattle( battleid );
  //battle.AddUser( m_serv.GetMe() );
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
  std::cout << "** ServerEvents::OnUserJoinedBattle()" << std::endl;
  Battle& battle = m_serv.GetBattle( battleid );
  User& user = m_serv.GetUser( nick );
  
  battle.AddUser( user );
  m_ui.OnUserJoinedBattle( battle, user );
}


void ServerEvents::OnUserLeftBattle( int battleid, const std::string& nick )
{
  std::cout << "** ServerEvents::OnUserLeftBattle()" << std::endl;
  Battle& battle = m_serv.GetBattle( battleid );
  User& user = m_serv.GetUser( nick );
  
  m_ui.OnUserLeftBattle( battle, user );

  battle.RemoveUser( user );
}


void ServerEvents::OnBattleInfoUpdated( int battleid, int spectators, bool locked, int maphash, const std::string& map )
{
  std::cout << "** ServerEvents::OnBattleInfoUpdated( )" << std::endl;
  Battle& battle = m_serv.GetBattle( battleid );
  
  battle.SetSpectators( spectators );
  battle.SetIsLocked( locked );
    
  battle.SetMapHash( maphash );
  battle.SetMapname( map );
  
  m_ui.OnBattleInfoUpdated( battle );
}


void ServerEvents::OnBattleClosed( int battleid )
{
  std::cout << "** ServerEvents::OnBattleClosed()" << std::endl;
  Battle& battle = m_serv.GetBattle( battleid );
  
  m_ui.OnBattleClosed( battle );
  
  m_serv._RemoveBattle( battleid );
}


void ServerEvents::OnJoinChannelResult( bool success, const std::string& channel, const std::string& reason )
{
  std::cout << "** ServerEvents::OnJoinChannelResult()" << std::endl;
  if ( success ) {

    Channel& chan = m_serv._AddChannel( channel );
    m_ui.OnJoinedChannelSuccessful( chan );
  
  } else {
    m_ui.ShowMessage( _T("Join channel failed"), _T("Could not join channel ") + WX_STRING(channel) + _T(" because: ") + WX_STRING(reason) );
  }
}


void ServerEvents::OnChannelSaid( const std::string& channel, const std::string& who, const std::string& message )
{
  std::cout << "** ServerEvents::OnChannelSaid()" << std::endl;
  m_serv.GetChannel( channel ).Said( m_serv.GetUser( who ), message );
}


void ServerEvents::OnChannelJoin( const std::string& channel, const std::string& who )
{
  std::cout << "** ServerEvents::OnChannelJoin()" << std::endl;
  m_serv.GetChannel( channel ).Joined( m_serv.GetUser( who ) );
}


void ServerEvents::OnChannelPart( const std::string& channel, const std::string& who, const std::string& message )
{
  std::cout << "** ServerEvents::OnChannelPart()" << std::endl;
  m_serv.GetChannel( channel ).Left( m_serv.GetUser( who ), message );
}


void ServerEvents::OnChannelTopic( const std::string& channel, const std::string& who, const std::string& message, int when )
{
  std::cout << "** ServerEvents::OnChannelTopic()" << std::endl;
  m_serv.GetChannel( channel ).SetTopic( message, who );
}


void ServerEvents::OnChannelAction( const std::string& channel, const std::string& who, const std::string& action )
{
  std::cout << "** ServerEvents::OnChannelAction()" << std::endl;
  m_serv.GetChannel( channel ).DidAction( m_serv.GetUser( who ), action );
}


void ServerEvents::OnPrivateMessage( const std::string& user, const std::string& message )
{
  std::cout << "** ServerEvents::OnPrivateMessage()" << std::endl;  
  User& who = m_serv.GetUser( user );
  m_ui.OnUserSaid( who, message );
}

void ServerEvents::OnChannelList( const std::string& channel, const int& numusers )
{
  m_ui.OnChannelList( channel, numusers );
}
