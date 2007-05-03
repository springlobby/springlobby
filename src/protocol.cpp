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
// Class: Protocol
//
// Created on: Fri Apr 27 16:23:28 2007
//

#include <assert.h>
#include "protocol.h"
#include "socket.h"

Protocol::Protocol()
{
  _sock = NULL;
  _ui = NULL;
  _keepalive = 15;
}


Protocol::~Protocol()
{
}

void Protocol::set_socket( Socket* sock )
{
  assert( !is_connected() ); // Should disconnect first.
  _sock = sock;
}

Socket* Protocol::get_socket( )
{
  return _sock;
}


void Protocol::set_uicontrol( Uicontrol* ui )
{
  _ui = ui;
}


Uicontrol* Protocol::get_uicontrol( )
{
  return _ui;
}


