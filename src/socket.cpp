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
//
// Created on: Fri Apr 27 16:26:26 2007
//

#include "socket.h"


//! @brief Constructor
Socket::Socket()
{
  
}


//! @brief Destructor
Socket::~Socket()
{
  
}


//! @brief Connect to remote host
void Socket::Connect( const string addr, const int port )
{

}


//! @brief Disconnect from current connection
void Socket::Disconnect( )
{

}


//! @brief Send data over connection
bool Socket::Send( string data )
{
  return false;
}


//! @brief Recive data from connection
bool Socket::Recive( string& data )
{
  return false;
}


//! @brief Get curent socket state
Sockstate Socket::State( )
{
  
}


//! @brief Get socket error code
Sockerror Socket::Error( )
{
  
}

