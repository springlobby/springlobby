//
// Class: NickListCtrl
//

#ifndef _NICKLISTCTRL_H_
#define _NICKLISTCTRL_H_

#include <wx/listctrl.h>

class User;
class UserList;

class NickListCtrl : public wxListCtrl
{
  public:
    NickListCtrl( wxWindow* parent );
    ~NickListCtrl();

    void AddUser( User& user );
    void RemoveUser( const User& user );

    void UserUpdated( User& user );
    void UserUpdated( const int& index );

  protected:
    UserList* m_users;
};

#endif  //_NICKLISTCTRL_H_

