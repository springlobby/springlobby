//
// Class: NickListCtrl
//

#ifndef _NICKLISTCTRL_H_
#define _NICKLISTCTRL_H_

#include <wx/listctrl.h>
#include <wx/intl.h>
#include "userlist.h"


class NickListCtrl : public wxListCtrl
{
  public:
    NickListCtrl( wxWindow* parent );
    ~NickListCtrl();
  
    // NickListCtrl interface
  
    virtual wxString OnGetItemText(long item, long column) const;
    virtual int OnGetItemColumnImage(long item, long column) const;
    virtual wxListItemAttr* OnGetItemAttr(long item) const;
  
    void SetUserList( UserList* users ) { m_users = users; }
    UserList* GetUserList() { return m_users; }
  
  protected:
    // NickListCtrl variables
  
    UserList* m_users;
  
};


#endif  //_NICKLISTCTRL_H_

