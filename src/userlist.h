//
// Class: UserList
//

#ifndef _USERLIST_H_
#define _USERLIST_H_

#include "user.h"
#include <map>
#include <vector>
#include <assert.h>

//! @brief std::map<> list that stores User pointers.
typedef std::map<std::string, User*> user_map_t;
//! @brief std::vector<> list that stores User pointers.
typedef std::vector<User*> user_vector_t;
//! @brief user_map_t iterator.
typedef user_map_t::iterator user_iter_t;


class UserList
{
  public:
    
    void AddUser( User* user ) {
      assert( user != NULL );
      int i = FindIndex( user->GetNick() );
      if ( i < 0 ) m_iusers.push_back(user); else m_iusers[i] = user;
      m_users[user->GetNick()] = user;
    }
    
    void RemoveUser( std::string const& nick ) {
      int i = FindIndex( nick );
      if ( i >= 0 ) m_iusers.erase(m_iusers.begin()+i);
      m_users.erase(nick);
    }
 
    User* GetUser( std::string const& nick ) {
      user_iter_t u = m_users.find(nick);
      return u == m_users.end() ? NULL : u->second;
    }

    User* GetUser( const int index ) { return m_iusers[index]; }
    
    bool UserExists( std::string const& nick ) {
      return m_users.find(nick) != m_users.end();
    }

    int FindIndex( std::string const& nick ) {
      for (int i = 0; i < m_iusers.size(); i++) {
        if ( m_iusers[i]->GetNick() == nick )
          return i;
      }
      return -1;
    }
    
    int GetNumUsers() { return m_iusers.size(); }
 
  protected:
    user_map_t m_users;
    user_vector_t m_iusers;
 
};


UserList& users();

#endif  //_USERLIST_H_
