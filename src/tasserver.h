//
// Class: TASServer
//

#ifndef _TASSERVER_H_
#define _TASSERVER_H_

#include <string>
#include <time.h>
#include <list>
#include "server.h"
#include "ui.h"
#include "serverevents.h"
#include "base64.h"
#include "socket.h"
#include <boost/md5.hpp>

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

//! @brief Struct used internally by the TASServer class to calculate ping roundtimes.
struct TASPingListItem {
  int id;
  time_t t;
};

//! @brief Union used internally by the TASServer class to get client status information.
union UTASClientstatus {
  unsigned char byte;
  TASClientstatus tasdata;
};

//! @brief TASServer protocol implementation.
class TASServer : public Server
{
  public:
    TASServer( Ui& ui ): Server(ui), m_ui(ui), m_connected(false), m_online(false), m_buffer(""), m_last_ping(0), m_ping_id(1000), m_ser_ver(SER_VER_UNKNOWN) { m_se = new ServerEvents( *this, ui); }
      
    ~TASServer() { delete m_se; }
  
  // TASServer interface
  
    // Overloaded functions from Server
    void SetSocket( Socket* sock );
    void Connect( const std::string& addr, const int port );
    void Disconnect();
    bool IsConnected();
  
    void Login();
    void Logout();
    bool IsOnline() {
      if ( !m_connected ) return false;
      return m_online;
    }
  
    void Update();
  
    void Ping();

    void JoinChannel( const std::string& channel, const std::string& key );
    void PartChannel( const std::string& channel );
  
    void SayChannel( const std::string& channel, const std::string& msg );
    void SayPrivate( const std::string& nick, const std::string& msg );
   
    void JoinBattle( const int& battleid, const std::string& password = "" );
    void LeaveBattle( const int& battleid );
    
    // TASServer specific functions
    void ExecuteCommand( std::string in );
    void ExecuteCommand( std::string cmd, std::string params, int replyid = -1 );
  
    std::string GetWordParam( std::string& params );
    std::string GetSentenceParam( std::string& params );
    int GetIntParam( std::string& params );

    void HandlePong( int replyid );
    void HandlePinglist();
    
    // Static utility functions
    static UserStatus ConvTasclientstatus( TASClientstatus );
    static bool VersionSupportReplyid( int version );
    static StartType IntToStartType( int start ) {
      switch ( start ) {
        case 0: return ST_Fixed;
        case 1: return ST_Random;
        case 2: return ST_Choose;
        default: assert(false);
      };
    }
    void OnConnected( Socket* sock );
    void OnDisconnected( Socket* sock );
    void OnDataRecived( Socket* sock );
    
    bool IsPasswordHash( const std::string& pass );
    std::string GetPasswordHash( const std::string& pass );
    
  protected:
    // TASServer variables
  
    Ui& m_ui;
    ServerEvents* m_se;
    int m_ser_ver;
  
    bool m_connected;
    bool m_online;
    std::string m_buffer;
    time_t m_last_ping;
    int m_ping_id;
    std::list<TASPingListItem> m_pinglist;
  
    void _ReciveAndExecute();
};


#endif  //_TASSERVER_H_

