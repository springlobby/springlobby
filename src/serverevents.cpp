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
// Class: ServerEvents
//
// Created on: Fri Apr 27 16:29:51 2007
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



void ServerEvents::OnConnected( string server_name, string server_ver, bool supported )
{
  cout << "** ServerEvents::OnConnected(): Server: " << server_ver.c_str() << endl;
  assert( sys().serv() != NULL );
  ui().OnConnected( server_name, server_ver, supported );
  sys().serv()->Login();
}

void ServerEvents::OnDisconnected()
{
  cout << "** ServerEvents::OnDisconnected()" << endl;
}

void ServerEvents::OnLogin()
{
  cout << "** ServerEvents::OnLogin()" << endl;
}

void ServerEvents::OnLoginInfoComplete()
{
  cout << "** ServerEvents::OnLoginInfoComplete()" << endl;
  assert( sys().serv() != NULL );
  sys().serv()->JoinChannel( "test", "" );
}

void ServerEvents::OnLogout()
{
  cout << "** ServerEvents::OnLogout()" << endl;
}
      
void ServerEvents::OnUnknownCommand( string command, string params )
{
  cout << "** ServerEvents::OnUnknownCommand()" << endl;
  servwin().UnknownCommand( WX_STRING(command), WX_STRING(params) );
}

void ServerEvents::OnSocketError( const Sockerror error )
{
  cout << "** ServerEvents::OnSocketError()" << endl;
}

void ServerEvents::OnProtocolError( const Protocolerror error )
{
  cout << "** ServerEvents::OnProtocolError()" << endl;
}

void ServerEvents::OnMotd( string msg )
{
  cout << "** ServerEvents::OnMotd()" << endl;
  servwin().Motd( WX_STRING(msg) );
}

void ServerEvents::OnPong( int ping_time )
{
  //cout << "** ServerEvents::OnPong()" << endl;
}
      
void ServerEvents::OnNewUser( string nick, string country, int cpu )
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

void ServerEvents::OnUserStatus( string nick, UserStatus status )
{
  //cout << "** ServerEvents::OnUserStatus()" << endl;
  User* user = sys().GetUser( nick );
  assert( user != NULL );
  
  user->SetStatus( status );
  
  ui().OnUserStatusChanged( *user );
}

void ServerEvents::OnUserQuit( string nick )
{
  cout << "** ServerEvents::OnUserQuit()" << endl;
  User* user = sys().GetUser( nick );
  assert( user != NULL );
  
  ui().OnUserOffline( *user );
  sys().RemoveUser( nick );
  
  delete user;
}
      
void ServerEvents::OnBattleOpened( int id, bool replay, int nat, string nick, 
                       string host, int port, int maxplayers, 
                       bool haspass, int rank, int hash, string map, 
                       string title, string mod )
{
  cout << "** ServerEvents::OnBattleOpened()" << endl;
}

void ServerEvents::OnUserJoinedBattle( int battleid, string nick )
{
  cout << "** ServerEvents::OnUserJoinedBattle()" << endl;
}

void ServerEvents::OnUserLeftBattle( int battleid, string nick )
{
  cout << "** ServerEvents::OnUserLeftBattle()" << endl;
}

void ServerEvents::OnBattleInfoUpdated( int battleid, int spectators, bool locked, int maphash, string map )
{
  //cout << "** ServerEvents::OnBattleInfoUpdated()" << endl;
}

void ServerEvents::OnBattleClosed( int battleid )
{
  cout << "** ServerEvents::OnBattleClosed()" << endl;
}

void ServerEvents::OnJoinChannelResult( bool success, string channel, string reason )
{
  cout << "** ServerEvents::OnJoinChannelResult()" << endl;
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

void ServerEvents::OnChannelSaid( string channel, string who, string message )
{
  Channel* chan = sys().GetChannel( channel );
  User* user = sys().GetUser( who );
  assert( chan != NULL );
  assert( user != NULL );
  
  chan->Said( *user, message );
}

void ServerEvents::OnChannelJoin( string channel, string who )
{
  Channel* chan = sys().GetChannel( channel );
  User* user = sys().GetUser( who );
  assert( chan != NULL );
  assert( user != NULL );
  
  chan->Joined( *user );
}


void ServerEvents::OnChannelPart( string channel, string who, string message )
{
  Channel* chan = sys().GetChannel( channel );
  User* user = sys().GetUser( who );
  assert( chan != NULL );
  assert( user != NULL );
  
  chan->Left( *user, message );
}


void ServerEvents::OnChannelTopic( string channel, string who, string message, int when )
{
  Channel* chan = sys().GetChannel( channel );
  User* user = sys().GetUser( who );
  assert( chan != NULL );
  assert( user != NULL );
  
  chan->SetTopic( message, *user );
}


void ServerEvents::OnChannelAction( string channel, string who, string action )
{
  Channel* chan = sys().GetChannel( channel );
  User* user = sys().GetUser( who );
  assert( chan != NULL );
  assert( user != NULL );
  
  chan->DidAction( *user, action );
}


