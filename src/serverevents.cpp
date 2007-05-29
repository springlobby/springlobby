//
// Class: ServerEvents
//

#include "serverevents.h"
#include "system.h"
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
  std::cout << "** ServerEvents::OnDisconnected()" << std::endl;
}

void ServerEvents::OnLogin()
{
  std::cout << "** ServerEvents::OnLogin()" << std::endl;
}

void ServerEvents::OnLoginInfoComplete()
{
  std::cout << "** ServerEvents::OnLoginInfoComplete()" << std::endl;
  m_serv.JoinChannel( "springlobby", "" );
}

void ServerEvents::OnLogout()
{
  std::cout << "** ServerEvents::OnLogout()" << std::endl;
}
      
void ServerEvents::OnUnknownCommand( const std::string& command, const std::string& params )
{
  std::cout << "** ServerEvents::OnUnknownCommand()" << std::endl;
  m_ui.OnUnknownCommand( m_serv, command, params );
}

void ServerEvents::OnSocketError( const Sockerror error )
{
  std::cout << "** ServerEvents::OnSocketError()" << std::endl;
}

void ServerEvents::OnProtocolError( const Protocolerror error )
{
  std::cout << "** ServerEvents::OnProtocolError()" << std::endl;
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
  std::cout << "** ServerEvents::OnUserStatus()" << std::endl;
  User& user = m_serv.GetUser( nick );
  user.SetStatus( status );
  m_ui.OnUserStatusChanged( user );
}

void ServerEvents::OnUserQuit( const std::string& nick )
{
  std::cout << "** ServerEvents::OnUserQuit()" << std::endl;
  m_ui.OnUserOffline( m_serv.GetUser( nick ) );
  std::cout << "** ServerEvents::OnUserQuit()" << std::endl;
  m_serv._RemoveUser( nick );
  std::cout << "** ServerEvents::OnUserQuit()" << std::endl;
}
      
void ServerEvents::OnBattleOpened( int id, bool replay, int nat, const std::string& nick, 
                       const std::string& host, int port, int maxplayers, 
                       bool haspass, int rank, int hash, const std::string& map, 
                       const std::string& title, const std::string& mod )
{
  std::cout << "** ServerEvents::OnBattleOpened()" << std::endl;
}

void ServerEvents::OnUserJoinedBattle( int battleid, const std::string& nick )
{
  std::cout << "** ServerEvents::OnUserJoinedBattle()" << std::endl;
}

void ServerEvents::OnUserLeftBattle( int battleid, const std::string& nick )
{
  std::cout << "** ServerEvents::OnUserLeftBattle()" << std::endl;
}
 
void ServerEvents::OnBattleInfoUpdated( int battleid, int spectators, bool locked, int maphash, const std::string& map )
{
  std::cout << "** ServerEvents::OnBattleInfoUpdated()" << std::endl;
}

void ServerEvents::OnBattleClosed( int battleid )
{
  std::cout << "** ServerEvents::OnBattleClosed()" << std::endl;
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
  m_serv.GetChannel( channel ).Left( m_serv.GetUser( who ), message );
}


void ServerEvents::OnChannelTopic( const std::string& channel, const std::string& who, const std::string& message, int when )
{
  std::cout << "** ServerEvents::OnChannelTopic()" << std::endl;
  m_serv.GetChannel( channel ).SetTopic( message, who );
}


void ServerEvents::OnChannelAction( const std::string& channel, const std::string& who, const std::string& action )
{
  m_serv.GetChannel( channel ).DidAction( m_serv.GetUser( who ), action );
}

/*
//! @todo fix
void ServerEvents::OnPrivateMessage( const std::string& user, const std::string& message )
{
  User& who = sys().GetUser( user );
  if ( who.GetUserData() != NULL ) {
  }
}
*/
