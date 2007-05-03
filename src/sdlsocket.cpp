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
//
// Created on: Fri Apr 27 16:47:13 2007
//

#include "sdlsocket.h"
#include <assert.h>

SDLSocket::SDLSocket() : Socket()
{
  _connected = false;
  _error = SE_NO_ERROR;
  
  set = SDLNet_AllocSocketSet(1);
  if ( !set ) assert(false);
  
}


SDLSocket::~SDLSocket()
{
  if ( _connected ) {
    disconnect();
  }
	// TODO
}

void SDLSocket::initSDL( bool only_sdl_net )
{
  if ( !only_sdl_net ) {
    if ( SDL_Init(0) == -1 ) {
      printf("SDL_Init: %s\n", SDL_GetError());
      exit(1);
    }
  }
  if ( SDLNet_Init() == -1 ) {
    printf("SDLNet_Init: %s\n", SDLNet_GetError());
    exit(1);
  }
}

void SDLSocket::quitSDL( bool only_sdl_net )
{
  SDLNet_Quit();
  if ( !only_sdl_net )
    SDL_Quit();
}


bool SDLSocket::connect( string addr, int port )
{
  _error = SE_NO_ERROR;

  if ( _connected ) {
    assert(false);
    disconnect();
  }
  if( SDLNet_ResolveHost( &ip, addr.c_str(), port ) == -1 ) {
    printf( "SDLNet_ResolveHost: %s\n", SDLNet_GetError() );
    return false;
  }
  sock = SDLNet_TCP_Open( &ip );
  if( !sock ) {
    printf( "SDLNet_TCP_Open: %s\n", SDLNet_GetError() );
    return false;
  }
  SDLNet_TCP_AddSocket( set, sock );
  
  _connected = true;
  
  return true;
}

bool SDLSocket::disconnect( )
{
  _error = SE_NO_ERROR;
  
  if ( !_connected ) return false;
    
  SDLNet_TCP_DelSocket( set, sock );
  SDLNet_TCP_Close( sock );
  
  _connected = false;
  
  return true;
}

bool SDLSocket::send( string data )
{
  int len = data.length();
  
  _error = SE_NO_ERROR;

  if( SDLNet_TCP_Send( sock, data.c_str(), len ) < len ) {
    printf("SDLNet_TCP_Send: \"%s\" disconnecting \n", SDLNet_GetError());
    disconnect();
    return false;
  }
  return true;
}

bool SDLSocket::recive( string& data )
{
  char c;
  int bytes = 0;
  
  _error = SE_NO_ERROR;

  while ( (can_recive()) && (bytes < 255) ) {
  
    if ( SDLNet_TCP_Recv( sock, &c, 1 ) == 1 ) {
      data += c;
      if ( c == '\n' )
        break;
    }
    bytes++;
  }
  return ( bytes > 0 );
}

bool SDLSocket::can_recive()
{
  int ready;
  _error = SE_NO_ERROR;

  if ( !_connected ) {
    assert(false);
    return false;
  }
  ready = SDLNet_CheckSockets( set, 0 );
  if ( ready == -1 ) {
    perror("SDLNet_CheckSockets");
    disconnect();
    return false;
  }
  return ready;
}

Sockstate SDLSocket::state( )
{
  if ( _connected )
    return SS_OPEN;
  else
    return SS_CLOSED;
}

Sockerror SDLSocket::error( )
{
  return _error;
}


