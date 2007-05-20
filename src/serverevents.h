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
  
    void OnConnected( const std::string& server_name, const std::string& server_ver, bool supported );
    void OnDisconnected();
      
    void OnLogin();
    void OnLoginInfoComplete();
    void OnLogout();
      
    void OnUnknownCommand( const std::string& command, const std::string& params );
    void OnSocketError( const Sockerror error );
    void OnProtocolError( const Protocolerror error );
    void OnMotd( const std::string& msg );
    void OnPong( int ping_time );
      
    void OnNewUser( const std::string& nick, const std::string& conutry, int cpu );
    void OnUserStatus( const std::string& nick, UserStatus status );
    void OnUserQuit( const std::string& nick );
    
    void OnBattleOpened( int id, bool replay, int nat, const std::string& nick, 
                         const std::string& host, int port, int maxplayers, 
                         bool haspass, int rank, int hash, const std::string& map, 
                         const std::string& title, const std::string& mod );

    void OnUserJoinedBattle( int battleid, const std::string& nick );
    void OnUserLeftBattle( int battleid, const std::string& nick );
    void OnBattleInfoUpdated( int battleid, int spectators, bool locked, int maphash, const std::string& map );
    void OnBattleClosed( int battleid );
    
    void OnJoinChannelResult( bool success, const std::string& channel, const std::string& reason );
    void OnChannelSaid( const std::string& channel, const std::string& who, const std::string& message );
    void OnChannelJoin( const std::string& channel, const std::string& who );
    void OnChannelPart( const std::string& channel, const std::string& who, const std::string& message );
    void OnChannelTopic( const std::string& channel, const std::string& who, const std::string& message, int when );
    void OnChannelAction( const std::string& channel, const std::string& who, const std::string& action );
    
    void OnPrivateMessage( const std::string& user, const std::string& message );
    
  protected:
    
};

ServerEvents& se();

#endif  //_UICONTROL_H_

