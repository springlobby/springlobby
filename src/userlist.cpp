//
// Class: UserList
//

#include "userlist.h"

UserList& users()
{
  static UserList m_users;
  return m_users;
}
