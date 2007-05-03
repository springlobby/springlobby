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
// Class: SDLSocket
// Created on: Fri Apr 27 16:47:13 2007
//

#ifndef _SDLSOCKET_H_
#define _SDLSOCKET_H_

#include "socket.h"
#include "SDL/SDL.h"
#include "SDL/SDL_net.h"

class SDLSocket : public Socket
{
  public:
    SDLSocket();
    virtual ~SDLSocket();
  
    // SDLSocket interface
   
    virtual bool connect( string addr, int port );
    virtual bool disconnect( );
  
    virtual bool send( string data );
    virtual bool recive( string& data );
  
    virtual bool can_recive();
  
    virtual Sockstate state( );
    virtual Sockerror error( );
  
    static void initSDL( bool only_sdl_net = false );
    static void quitSDL( bool only_sdl_net = false );
  
  protected:
    // SDLSocket variables

    SDLNet_SocketSet set;
    IPaddress ip;
    TCPsocket sock;
    
    bool _connected;
    Sockerror _error;
  
};


#endif	//_SDLSOCKET_H_

