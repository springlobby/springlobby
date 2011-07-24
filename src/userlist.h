#ifndef SPRINGLOBBY_HEADERGUARD_USERLIST_H
#define SPRINGLOBBY_HEADERGUARD_USERLIST_H

#include <map>
#include <wx/string.h>

class User;

class UserList
{
  public:
      //! @brief mapping from nick to user object
    typedef std::map<wxString, User*> user_map_t;
    //! @brief iterator for user map
    typedef user_map_t::iterator user_iter_t;
    typedef user_map_t::const_iterator user_const_iter_t;

    UserList();
    virtual ~UserList() {}
    void AddUser( User& user );
    void RemoveUser( wxString const& nick );
    User& GetUser( wxString const& nick ) const;
    User& GetUser( user_map_t::size_type index ) const;
    bool UserExists( wxString const& nick ) const;
    user_map_t::size_type GetNumUsers() const;

    void Nullify();

  private:
    user_map_t m_users;
    // The following are used as internal cache to speed up random access:
    mutable user_const_iter_t m_seek;
    mutable user_map_t::size_type m_seekpos;

};

#endif // SPRINGLOBBY_HEADERGUARD_USERLIST_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2011

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    SpringLobby is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

