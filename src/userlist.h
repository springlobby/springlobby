//
// Class: UserList
//

#ifndef _USERLIST_H_
#define _USERLIST_H_

#include "user.h"
#include <map>

//! @brief std::map<> list that stores User pointers.
typedef std::map<std::string, User*> user_map_t;
//! @brief user_map_t iterator.
typedef user_map_t::iterator user_iter_t;


class UserList
{
  public:
    void AddUser( User* user ) { m_users[user->GetNick()] = user; }
    void RemoveUser( std::string const& nick ) { m_users.erase(nick); }
 
    User* GetUser( std::string const& nick ) {
        user_iter_t u = m_users.find(nick);
        return u == m_users.end() ? NULL : u->second;
    }

    bool UserExists( std::string const& nick ) {
        return m_users.find(nick) != m_users.end();
    }

    int GetNumUsers() { return m_users.size(); }
 
  protected:
    user_map_t m_users;
 
};


#endif  //_USERLIST_H_
