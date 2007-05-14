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
// Class: TASServer
// Created on: Fri Apr 27 19:25:15 2007
//

#ifndef _TASSERVER_H_
#define _TASSERVER_H_

#include <string>
#include <time.h>
#include <list>
#include "server.h"

using namespace std;

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
    TASServer();
    virtual ~TASServer();
  
  // TASServer interface
  
    // Overloaded functions from Server
    virtual void SetSocket( Socket* sock );
    virtual void Connect( string addr, const int port );
    virtual void Disconnect();
    virtual bool IsConnected();
  
    virtual void Login();
    virtual void Logout();
    virtual bool IsOnline();
  
    virtual void Update();
  
    virtual void Ping();

    virtual void JoinChannel( string channel, string key );
    virtual void PartChannel( string channel );
  
    virtual void SayChannel( string channel, string msg );
    virtual void SayPrivate( string nick, string msg );
   
    // TASServer specific functions
    void ExecuteCommand( string in );
    void ExecuteCommand( string cmd, string params, int replyid = -1 );
  
    string GetWordParam( string& params );
    string GetSentenceParam( string& params );
    int GetIntParam( string& params );

    void HandlePong( int replyid );
    void HandlePinglist();
    
    // Static utility functions
    static Clientstatus ConvTasclientstatus( TASClientstatus );
    static bool VersionSupportReplyid( int version );
    
    static void OnConnected( Socket* sock );
    static void OnDisconnected( Socket* sock );
    static void OnDataRecived( Socket* sock );
    
  protected:
    // TASServer variables
  
    int m_ser_ver;
  
    bool m_connected;
    bool m_online;
    string m_buffer;
    time_t m_last_ping;
    int m_ping_id;
    list<TASPingListItem> m_pinglist;
  
    void _ReciveAndExecute();
};


#endif  //_TASSERVER_H_

