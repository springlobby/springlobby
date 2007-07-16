//
// Class: UserList
//

#ifndef _USERLIST_H_
#define _USERLIST_H_

#include <map>
#include <string>

class User;

//! @brief mapping from nick to user object
typedef std::map<std::string, User*> user_map_t;
//! @brief iterator for user map
typedef user_map_t::iterator user_iter_t;

class UserList
{
  public:
    UserList();
    void AddUser( User& user );
    void RemoveUser( std::string const& nick );
    User& GetUser( std::string const& nick );
    User& GetUser( user_map_t::size_type index );
    bool UserExists( std::string const& nick );
    user_map_t::size_type GetNumUsers();

  private:
    user_map_t m_users;
    // The following are used as internal cache to speed up random access:
    mutable user_iter_t m_seek;
    mutable user_map_t::size_type m_seekpos;
};

#endif  //_USERLIST_H_
