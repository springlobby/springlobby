//
// Class: NickListCtrl
//

#ifndef _NICKLISTCTRL_H_
#define _NICKLISTCTRL_H_

#include <wx/listctrl.h>
#include <wx/intl.h>
#include <iostream>
#include "userlist.h"


#define ICON_ADMIN 0;
#define ICON_ADMIN_AWAY 1;
#define ICON_ADMIN_INGAME 2;
#define ICON_BOT 3;
#define ICON_NOSTATE -1;
#define ICON_AWAY 4;
#define ICON_INGAME 5;

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
    void SetUserList( UserList* users ) { m_users = users; }
    UserList* GetUserList() { return m_users; }
  
    int GetStateIcon( const UserStatus& us ) const;
    
    void UpdateSize();
    
  protected:
    // NickListCtrl variables
  
    UserList* m_users;
    wxImageList* m_imagelist;
  
};


#endif  //_NICKLISTCTRL_H_

