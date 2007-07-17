#include <iterator>
#include <stdexcept>

#include "userlist.h"
#include "user.h"

// FIXME make this the max value for the type it is used for
#define SEEKPOS_INVALID (-1)

UserList::UserList(): m_seek(m_users.end()), m_seekpos(SEEKPOS_INVALID)
{ }

void UserList::AddUser( User& user )
{
  m_users[user.GetNick()] = &user;
  m_seekpos = SEEKPOS_INVALID;
}

void UserList::RemoveUser( std::string const& nick )
{
  m_users.erase(nick);
  m_seekpos = SEEKPOS_INVALID;
}

User& UserList::GetUser( std::string const& nick )
{
  user_iter_t u = m_users.find(nick);
  if (u == m_users.end()) throw std::logic_error("UserList::GetUser(\"" + nick + "\"): no such user");
  return *u->second;
}

User& UserList::GetUser( user_map_t::size_type index )
{
  if ((m_seekpos == SEEKPOS_INVALID) || (m_seekpos > index)) {
    m_seek = m_users.begin();
    m_seekpos = 0;
  }
  std::advance( m_seek, index - m_seekpos );
  m_seekpos = index;
  return *m_seek->second;
}

bool UserList::UserExists( std::string const& nick )
{
  return m_users.find(nick) != m_users.end();
}

user_map_t::size_type UserList::GetNumUsers()
{
  return m_users.size();
}

