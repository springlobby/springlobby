//
// Class: ServerEvents
//

#include "serverevents.h"
#include "system.h"
#include "mainwindow.h"
#include "ui.h"

ServerEvents::ServerEvents()
{

}


ServerEvents::~ServerEvents()
{

}

ServerEvents& se()
{
  static ServerEvents m_se;
  return m_se;
}



void ServerEvents::OnConnected( const std::string& server_name, const std::string& server_ver, bool supported )
{
  std::cout << "** ServerEvents::OnConnected(): Server: " << server_ver.c_str() << std::endl;
  assert( sys().serv() != NULL );
  ui().OnConnected( server_name, server_ver, supported );
  sys().serv()->Login();
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
  assert( sys().serv() != NULL );
  sys().serv()->JoinChannel( "springlobby", "" );
}

void ServerEvents::OnLogout()
{
  std::cout << "** ServerEvents::OnLogout()" << std::endl;
}
      
void ServerEvents::OnUnknownCommand( const std::string& command, const std::string& params )
{
  std::cout << "** ServerEvents::OnUnknownCommand()" << std::endl;
  servwin().UnknownCommand( WX_STRING(command), WX_STRING(params) );
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
  servwin().Motd( WX_STRING(msg) );
}

void ServerEvents::OnPong( int ping_time )
{
  //cout << "** ServerEvents::OnPong()" << endl;
}
      
void ServerEvents::OnNewUser( const std::string& nick, const std::string& country, int cpu )
{
  //cout << "** ServerEvents::OnNewUser()" << endl;
  User* user = sys().GetUser( nick );
  if ( user == NULL ) {
    UserStatus stat;
    user = new User( nick, country, cpu, stat );
    sys().AddUser( user );
  } else {
    user->SetCountry( country );
    user->SetCpu( cpu );
  }
  ui().OnUserOnline( *user );
}

void ServerEvents::OnUserStatus( const std::string& nick, UserStatus status )
{
  //cout << "** ServerEvents::OnUserStatus()" << endl;
  User* user = sys().GetUser( nick );
  assert( user != NULL );
  
  user->SetStatus( status );
  
  ui().OnUserStatusChanged( *user );
}

void ServerEvents::OnUserQuit( const std::string& nick )
{
  std::cout << "** ServerEvents::OnUserQuit()" << std::endl;
  User* user = sys().GetUser( nick );
  assert( user != NULL );
  
  ui().OnUserOffline( *user );
  sys().RemoveUser( nick );
  
  delete user;
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
  //cout << "** ServerEvents::OnBattleInfoUpdated()" << endl;
}

void ServerEvents::OnBattleClosed( int battleid )
{
  std::cout << "** ServerEvents::OnBattleClosed()" << std::endl;
}

void ServerEvents::OnJoinChannelResult( bool success, const std::string& channel, const std::string& reason )
{
  std::cout << "** ServerEvents::OnJoinChannelResult()" << std::endl;
  if ( success ) {
    Channel* chan = new Channel();
    chan->SetName( channel );
    sys().AddChannel( chan );
    
    ui().OnJoinedChannelSuccessful( *chan );
    //mw().OpenChannelChat( WX_STRING(channel) );
  } else {
    wxString s;
    s << _T("Could not join channel ") << WX_STRING(channel) << _T(" because: ") << WX_STRING(reason);
    wxMessageDialog msg( NULL, s, _T("Join channel failed"), wxOK);
    msg.ShowModal();
  }
}

void ServerEvents::OnChannelSaid( const std::string& channel, const std::string& who, const std::string& message )
{
  Channel* chan = sys().GetChannel( channel );
  User* user = sys().GetUser( who );
  assert( chan != NULL );
  assert( user != NULL );
  
  chan->Said( *user, message );
}

void ServerEvents::OnChannelJoin( const std::string& channel, const std::string& who )
{
  Channel* chan = sys().GetChannel( channel );
  User* user = sys().GetUser( who );
  assert( chan != NULL );
  assert( user != NULL );
  
  chan->Joined( *user );
}


void ServerEvents::OnChannelPart( const std::string& channel, const std::string& who, const std::string& message )
{
  Channel* chan = sys().GetChannel( channel );
  User* user = sys().GetUser( who );
  assert( chan != NULL );
  assert( user != NULL );
  
  chan->Left( *user, message );
}


void ServerEvents::OnChannelTopic( const std::string& channel, const std::string& who, const std::string& message, int when )
{
  Channel* chan = sys().GetChannel( channel );
//  User* user = sys().GetUser( who );
  assert( chan != NULL );
//  assert( user != NULL );
  
  chan->SetTopic( message, who );
}


void ServerEvents::OnChannelAction( const std::string& channel, const std::string& who, const std::string& action )
{
  Channel* chan = sys().GetChannel( channel );
  User* user = sys().GetUser( who );
  assert( chan != NULL );
  assert( user != NULL );
  
  chan->DidAction( *user, action );
}


