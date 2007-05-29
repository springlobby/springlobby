//
// Class: NickListCtrl
//

#ifndef _NICKLISTCTRL_H_
#define _NICKLISTCTRL_H_

#include <wx/listctrl.h>
#include <wx/intl.h>
#include <iostream>
#include "userlist.h"


#define ICON_ADMIN 0
#define ICON_ADMIN_AWAY 1
#define ICON_ADMIN_INGAME 2
#define ICON_BOT 3
#define ICON_NOSTATE -1
#define ICON_AWAY 4
#define ICON_INGAME 5
#define ICON_UP 6
#define ICON_DOWN 7
#define ICON_RANK0 8
#define ICON_RANK1 9
#define ICON_RANK2 10
#define ICON_RANK3 11
#define ICON_RANK4 12
#define ICON_FLAGS_BASE 13

class User;
class UserList;
struct UserStatus;

class NickListCtrl : public wxListCtrl
{
  public:
    NickListCtrl( wxWindow* parent );
    ~NickListCtrl();
  
    // NickListCtrl interface
  
    virtual wxString OnGetItemText(long item, long column) const;
    virtual int OnGetItemColumnImage(long item, long column) const;
    virtual wxListItemAttr* OnGetItemAttr(long item) const;
    virtual int OnGetItemImage(long item) const;
    //void SetUserList( UserList* users ) { m_users = users; }
    //UserList* GetUserList() { return m_users; }
  
    void AddUser( User& user );
    void RemoveUser( const User& user );
  
    int GetStateIcon( const UserStatus& us ) const;
    int GetRankIcon( const UserStatus& us ) const;
    
    void UpdateSize();
    
  protected:
    // NickListCtrl variables
    
    UserList* m_users;
  
};

int wxCALLBACK NickListSortCallback(long item1, long item2, long sortData);

#endif  //_NICKLISTCTRL_H_

