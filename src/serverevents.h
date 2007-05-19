//
// Class: ServerEvents
//

#ifndef _UICONTROL_H_
#define _UICONTROL_H_

#include <wx/msgdlg.h>
#include "socket.h"
#include "server.h"


typedef int Protocolerror;

//! @brief Class that implements server event behaviour.
class ServerEvents
{
  public:
    ServerEvents();
    ~ServerEvents();
  
  // Uicontrol interface
  
    void OnConnected( string server_name, string server_ver, bool supported );
    void OnDisconnected();
      
    void OnLogin();
    void OnLoginInfoComplete();
    void OnLogout();
      
    void OnUnknownCommand( string command, string params );
    void OnSocketError( const Sockerror error );
    void OnProtocolError( const Protocolerror error );
    void OnMotd( string msg );
    void OnPong( int ping_time );
      
    void OnNewUser( string nick, string conutry, int cpu );
    void OnUserStatus( string nick, UserStatus status );
    void OnUserQuit( string nick );
    
    void OnBattleOpened( int id, bool replay, int nat, string nick, 
                                   string host, int port, int maxplayers, 
                                   bool haspass, int rank, int hash, string map, 
                                   string title, string mod );

    void OnUserJoinedBattle( int battleid, string nick );
    void OnUserLeftBattle( int battleid, string nick );
    void OnBattleInfoUpdated( int battleid, int spectators, bool locked, int maphash, string map );
    void OnBattleClosed( int battleid );
    
    void OnJoinChannelResult( bool success, string channel, string reason );
    void OnChannelSaid( string channel, string who, string message );
    void OnChannelJoin( string channel, string who );
    void OnChannelPart( string channel, string who, string message );
    void OnChannelTopic( string channel, string who, string message, int when );
    void OnChannelAction( string channel, string who, string action );
  protected:
  // Uicontrol variables
    
};

ServerEvents& se();

#endif  //_UICONTROL_H_

