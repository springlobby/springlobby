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
// Class: Socket
// Created on: Fri Apr 27 16:26:26 2007
//

#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <string>
#include <wx/wx.h>
#include <wx/socket.h>
#include <wx/event.h>

using namespace std;

typedef int Sockstate;

#define SS_CLOSED 0
#define SS_CONNECTING 1
#define SS_OPEN 2

typedef int Sockerror;

#define SE_NO_ERROR 0
#define SE_NOT_CONNECTED 1
#define SE_RESOLVE_HOST_FAILED 2
#define SE_CONNECT_TO_HOST_FAILED 3

#define SOCKET_ID 100

#ifndef WX_STRING
#define WX_STRING(v) wxString(v.c_str(),wxConvUTF8)
#endif

class SocketEvents: public wxEvtHandler
{
  public:
    void OnSocketEvent(wxSocketEvent& event);
  protected:
  
  DECLARE_EVENT_TABLE()
};

class Socket;

typedef void (*socket_callback)(Socket*);

//! @brief Class that implements a TCP client socket.
class Socket
{
  public:
    
    Socket();
    virtual ~Socket();
  
    // Socket interface
  
    virtual void Connect( const string addr, const int port );
    virtual void Disconnect( );
  
    virtual bool Send( string data );
    virtual bool Recive( string& data );
  
    virtual Sockstate State( );
    virtual Sockerror Error( );
  
    virtual void set_userdata( void* data );
    virtual void* get_userdata();
  
    virtual void set_connected_callback( socket_callback callback );
    virtual socket_callback get_connected_callback();

    virtual void set_disconnected_callback( socket_callback callback );
    virtual socket_callback get_disconnected_callback();

    virtual void set_data_recived_callback( socket_callback callback );
    virtual socket_callback get_data_recived_callback();

  protected:
  // Socket variables
    wxSocketClient* m_sock;
    SocketEvents* m_events;
    socket_callback m_on_con;
    socket_callback m_on_discon;
    socket_callback m_on_data;
    bool m_connecting;
    void* m_udata;
};


#endif  //_SOCKET_H_

