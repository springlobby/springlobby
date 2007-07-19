#ifndef SPRINGLOBBY_HEADERGUARD_NICKLISTCTRL_H
#define SPRINGLOBBY_HEADERGUARD_NICKLISTCTRL_H

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

#endif // SPRINGLOBBY_HEADERGUARD_NICKLISTCTRL_H
