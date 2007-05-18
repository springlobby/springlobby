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
// Class: Server
//
// Created on: Fri Apr 27 16:23:28 2007
//

#include <assert.h>
#include "server.h"
#include "socket.h"

Server::Server()
{
  m_sock = NULL;
  m_ui = NULL;
  m_keepalive = 15;
}


Server::~Server()
{
}

void Server::SetSocket( Socket* sock )
{
  assert( (!IsConnected()) || (sock == NULL) ); // Should disconnect first.
  m_sock = sock;
}

Socket* Server::GetSocket( )
{
  return m_sock;
}


void Server::SetServerEvents( ServerEvents* ui )
{
  m_ui = ui;
}


ServerEvents* Server::GetServerEvents( )
{
  return m_ui;
}

void Server::SetUsername( const string username )
{
  m_user = username;
}


void Server::SetPassword( const string password )
{
  m_pass = password;
}

