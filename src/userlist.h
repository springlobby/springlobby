//
// Class: UserList
//

#ifndef _USERLIST_H_
#define _USERLIST_H_

#include "user.h"
#include <algorithm>
#include <iterator>
#include <map>
#include <stdexcept>

//! @brief std::map<> list that stores User pointers.
typedef std::map<std::string, User*> user_map_t;
//! @brief user_map_t iterator.
typedef user_map_t::iterator user_iter_t;


class UserList
{
  public:
    
    UserList(): m_seek(m_users.end()), m_seekpos(-1) {}
    
    void AddUser( User& user ) {
      m_users[user.GetNick()] = &user;
      m_seekpos = -1;
    }
    
    void RemoveUser( std::string const& nick ) {
      m_users.erase(nick);
      m_seekpos = -1;
    }
 
    User& GetUser( std::string const& nick ) {
      user_iter_t u = m_users.find(nick);
      if (u == m_users.end()) throw std::logic_error("UserList::GetUser(\"" + nick + "\"): no such user");
      return *u->second;
    }

    User const& GetUser( int index ) {
      if (m_seekpos < 0) {
        m_seek = m_users.begin();
        m_seekpos = 0;
      }
      std::advance( m_seek, index - m_seekpos );
      m_seekpos = index;
      return *m_seek->second;
    }
    
    bool UserExists( std::string const& nick ) {
      return m_users.find(nick) != m_users.end();
    }

    int GetNumUsers() { return m_users.size(); }
 
  private:
    user_map_t m_users;
    // The following are used as internal cache to speed up random access:
    mutable user_iter_t m_seek;
    mutable int m_seekpos;
};

#endif  //_USERLIST_H_
