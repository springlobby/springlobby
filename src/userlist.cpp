/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
#include <iterator>
#include <stdexcept>
#include <wx/log.h>

#include "userlist.h"
#include "user.h"
#include "utils/debug.h"
#include "utils/conversion.h"

const UserList::user_map_t::size_type SEEKPOS_INVALID = UserList::user_map_t::size_type(-1);

UserList::UserList(): m_seek(m_users.end()), m_seekpos(SEEKPOS_INVALID)
{ }

void UserList::AddUser( User& user )
{
  m_users[user.GetNick()] = &user;
  m_seekpos = SEEKPOS_INVALID;
}

void UserList::RemoveUser( const wxString& nick )
{
  m_users.erase(nick);
  m_seekpos = SEEKPOS_INVALID;
}

User& UserList::GetUser( const wxString& nick ) const
{
  user_const_iter_t u = m_users.find(nick);
  ASSERT_EXCEPTION( u != m_users.end(), _T("UserList::GetUser(\"") + nick + _T("\"): no such user") );
  //ASSERT_LOGIC( u != m_users.end(), _T("UserList::GetUser(\"") + nick + _T("\"): no such user") );
  return *u->second;
}

User& UserList::GetUser( user_map_t::size_type index ) const
{
  if ((m_seekpos == SEEKPOS_INVALID) || (m_seekpos > index)) {
    m_seek = m_users.begin();
    m_seekpos = 0;
  }
  std::advance( m_seek, index - m_seekpos );
  m_seekpos = index;
  return *m_seek->second;
}

bool UserList::UserExists( wxString const& nick ) const
{
  return m_users.find(nick) != m_users.end();
}

UserList::user_map_t::size_type UserList::GetNumUsers() const
{
  return m_users.size();
}

void UserList::Nullify()
{
    for( user_map_t::iterator it = m_users.begin(); it != m_users.begin(); ++it ) {
        delete it->second;
        it->second = NULL;
    }
}
