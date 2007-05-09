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
    virtual void connect( string addr, const int port );
    virtual void disconnect();
    virtual bool is_connected();
  
    virtual void login( string username, string password );
    virtual void logout();
    virtual bool is_online();
  
    virtual void update();
  
    virtual void ping();

    virtual void join_channel( string channel, string key );
    virtual void part_channel( string channel );
  
    virtual void say_channel( string channel, string msg );
    virtual void say_private( string nick, string msg );
  
    // TASServer specific functions
    void execute_command( string in );
    void execute_command( string cmd, string params, int replyid = -1 );
  
    string get_word_param( string& params );
    string get_sentence_param( string& params );
    int get_int_param( string& params );

    void handle_pong( int replyid );
    void handle_pinglist();
    
    // Static utility functions
    static Clientstatus conv_tasclientstatus( TASClientstatus );
    static bool version_support_replyid( int version );
  
  protected:
    // TASServer variables
  
    int m_ser_ver;
  
    bool m_connected;
    bool m_online;
    string m_buffer;
    time_t m_last_ping;
    int m_ping_id;
    list<TASPingListItem> m_pinglist; 
};


#endif  //_TASSERVER_H_

