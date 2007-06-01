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
  
    // NickListCtrl interface
  
    //virtual wxString OnGetItemText(long item, long column) const;
    //virtual int OnGetItemColumnImage(long item, long column) const;
    //virtual wxListItemAttr* OnGetItemAttr(long item) const;
    //virtual int OnGetItemImage(long item) const;
    //void SetUserList( UserList* users ) { m_users = users; }
    //UserList* GetUserList() { return m_users; }
  
    void AddUser( User& user );
    void RemoveUser( const User& user );
      
    //void UpdateSize();
    
  protected:
    // NickListCtrl variables
    
    UserList* m_users;
  
};

int wxCALLBACK NickListSortCallback(long item1, long item2, long sortData);

#endif  //_NICKLISTCTRL_H_

