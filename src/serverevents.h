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
// Class: Uicontrol
// Created on: Fri Apr 27 16:29:51 2007
//

#ifndef _UICONTROL_H_
#define _UICONTROL_H_

#include <wx/msgdlg.h>
#include "socket.h"
#include "server.h"
#include "chatlist.h"

typedef int Protocolerror;

//! @brief Class that implements server event behaviour.
class ServerEvents
{
  public:
    ServerEvents();
    virtual ~ServerEvents();
  
  // Uicontrol interface
  
    virtual void OnConnected( string server_ver, bool supported );
    virtual void OnDisconnected();
      
    virtual void OnLogin();
    virtual void OnLoginInfoComplete();
    virtual void OnLogout();
      
    virtual void OnUnknownCommand( string command, string params );
    virtual void OnSocketError( const Sockerror error );
    virtual void OnProtocolError( const Protocolerror error );
    virtual void OnMotd( string msg );
    virtual void OnPong( int ping_time );
      
    virtual void OnNewUser( string nick, string contry, int cpu );
    virtual void OnUserStatus( string nick, UserStatus status );
    virtual void OnUserQuit( string nick );
    
    virtual void OnBattleOpened( int id, bool replay, int nat, string nick, 
                                   string host, int port, int maxplayers, 
                                   bool haspass, int rank, int hash, string map, 
                                   string title, string mod );

    virtual void OnUserJoinedBattle( int battleid, string nick );
    virtual void OnUserLeftBattle( int battleid, string nick );
    virtual void OnBattleInfoUpdated( int battleid, int spectators, bool locked, int maphash, string map );
    virtual void OnBattleClosed( int battleid );
    
    virtual void OnJoinChannelResult( bool success, string channel, string reason );
    virtual void OnChannelSaid( string channel, string who, string message );
    virtual void OnChannelJoin( string channel, string who );
    virtual void OnChannelPart( string channel, string who, string message );
    virtual void OnChannelTopic( string channel, string who, string message, int when );
    virtual void OnChannelAction( string channel, string who, string action );
  protected:
  // Uicontrol variables
    
};

ServerEvents& se();

#endif  //_UICONTROL_H_

