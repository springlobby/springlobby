//
// Class: NickListCtrl
//

#ifndef _NICKLISTCTRL_H_
#define _NICKLISTCTRL_H_

#include <wx/listctrl.h>
#include <wx/intl.h>
#include <iostream>
#include "userlist.h"


class User;
class UserList;
struct UserStatus;

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
    // NickListCtrl variables
    
    UserList* m_users;
  
};

int wxCALLBACK NickListSortCallback(long item1, long item2, long sortData);

#endif  //_NICKLISTCTRL_H_

