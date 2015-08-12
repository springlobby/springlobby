/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_USERLIST_H
#define SPRINGLOBBY_HEADERGUARD_USERLIST_H

/**
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

TODO: remove all wx stuff & merge / move to

lsl/container/userlist.h

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
**/

#include <map>
#include <string>

class User;

class UserList
{
public:
	//! @brief mapping from nick to user object
	typedef std::map<std::string, User*> user_map_t;
	//! @brief iterator for user map
	typedef user_map_t::iterator user_iter_t;
	typedef user_map_t::const_iterator user_const_iter_t;

	UserList();
	~UserList();

	void AddUser(User& user);
	void RemoveUser(std::string const& nick);
	User& GetUser(std::string const& nick) const;
	User& GetUser(user_map_t::size_type index) const;
	bool UserExists(std::string const& nick) const;
	user_map_t::size_type GetNumUsers() const;

	void Nullify();
	const user_map_t& GetUsers() const
	{
		return m_users;
	}
	/*
	UserList& operator= (const UserList& other) = delete;
	UserList& operator= (const UserList&& other);
*/
protected:
	user_map_t m_users;

private:
	// The following are used as internal cache to speed up random access:
	mutable user_const_iter_t m_seek;
	mutable user_map_t::size_type m_seekpos;
};

#endif // SPRINGLOBBY_HEADERGUARD_USERLIST_H
