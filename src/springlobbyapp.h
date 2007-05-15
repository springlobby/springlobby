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
// Class: SpringLobbyApp
// Created on: Fri May 11 19:19:23 2007
//

#ifndef _SPRINGLOBBYAPP_H_
#define _SPRINGLOBBYAPP_H_

//#include <wx/wx.h>
#include <wx/app.h>
#include <wx/menu.h>

#include "server.h"
#include "tasserver.h"
#include "serverevents.h"
#include "chatlist.h"
#include "settings.h"

#define TIMER_ID 101
#define TIMER_INTERVAL 100

class MainWindow;
class ConnectWindow;

 
//! @brief Spring lobby wxApp, this class connects all the other classes
class SpringLobbyApp : public wxApp, public Settings, public ServerEvents, public ChatList
{
  public:
    // SpringLobbyApp interface
    SpringLobbyApp();
    virtual ~SpringLobbyApp();
    
    virtual bool OnInit();
    virtual int OnExit();
  
    Server* Serv();
    void SetServ( Server* serv );
  
    MainWindow& MainWin();
  
    void OpenMainWindow();
    void OpenConnectWindow();
  
    void Quit();
    void DefaultConnect();
    void Connect( const string servername, const string username, const string password );
    void Disconnect();
  
    void OnTimer( wxTimerEvent& event );
  
    virtual void on_connected( string server_ver, bool supported );
    virtual void on_disconnected();
      
    virtual void on_login();
    virtual void on_login_info_complete();
    virtual void on_logout();
      
    virtual void on_unknown_command( string command, string params );
    virtual void on_socket_error( const Sockerror error );
    virtual void on_protocol_error( const Protocolerror error );
    virtual void on_motd( string msg );
    virtual void on_pong( int ping_time );
      
    virtual void on_new_user( string nick, string contry, int cpu );
    virtual void on_user_status( string nick, UserStatus status );
    virtual void on_user_quit( string nick );
      
    virtual void on_battle_opened( int id, bool replay, int nat, string nick, 
                                   string host, int port, int maxplayers, 
                                   bool haspass, int rank, int hash, string map, 
                                   string title, string mod );

    virtual void on_user_joined_battle( int battleid, string nick );
    virtual void on_user_left_battle( int battleid, string nick );
    virtual void on_battleinfo_updated( int battleid, int spectators, bool locked, int maphash, string map );
    virtual void on_battle_closed( int battleid );
    virtual void on_join_channel_result( bool success, string channel, string reason );
    virtual void on_channel_said( string channel, string who, string message );
    virtual void on_channel_join( string channel, string who );
    virtual void on_channel_part( string channel, string who, string message );
    virtual void on_channel_topic( string channel, string who, string message, int when );
    virtual void on_channel_action( string channel, string who, string action );
  protected:
    // SpringLobbyApp variables
    
    MainWindow* m_main_win;
    ConnectWindow* m_con_win;
  
    Server* m_serv;
  
    wxTimer* m_timer;
  
    ChatList m_cl;
  
    DECLARE_EVENT_TABLE()
};


SpringLobbyApp& app();


DECLARE_APP(SpringLobbyApp)

#endif  //_SPRINGLOBBYAPP_H_

