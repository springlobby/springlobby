#ifndef SPRINGLOBBY_HEADERGUARD_USERLIST_H
#define SPRINGLOBBY_HEADERGUARD_USERLIST_H

#include <map>
#include <wx/string.h>

class User;

//! @brief mapping from nick to user object
typedef std::map<wxString, User*> user_map_t;
//! @brief iterator for user map
typedef user_map_t::iterator user_iter_t;
typedef user_map_t::const_iterator user_const_iter_t;

class UserList
{
  public:
    UserList();
    void AddUser( User& user );
    void RemoveUser( wxString const& nick );
    User& GetUser( wxString const& nick ) const;
    User& GetUser( user_map_t::size_type index ) const;
    bool UserExists( wxString const& nick ) const;
    user_map_t::size_type GetNumUsers() const;

  private:
    user_map_t m_users;
    // The following are used as internal cache to speed up random access:
    mutable user_const_iter_t m_seek;
    mutable user_map_t::size_type m_seekpos;
};

#endif // SPRINGLOBBY_HEADERGUARD_USERLIST_H
