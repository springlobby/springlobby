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
// Class: UserList
//
#include "userlist.h"
#include <iostream>

using namespace std;

UserList::UserList()
{
}


UserList::~UserList()
{
}


void UserList::AddUser( User* user )
{
  cout << "** UserList::AddUser()" << endl;
  assert( user != NULL );
  m_users[user->GetNick()] = user;
}


void UserList::RemoveUser( const string nick )
{
  m_users.erase( nick );
}

 
User* UserList::GetUser( const string nick )
{
  if ( m_users.count( nick ) > 0 ) {
    user_iter_t found = m_users.find( nick );
    return (*found).second;
  } else {
    return NULL;
  }
}


bool UserList::UserExists( const string nick )
{
  return (m_users.count( nick ) > 0);
}


int UserList::GetNumUsers()
{
  return m_users.size();
}

