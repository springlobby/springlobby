//
// Class: BattleroomListCtrl
//

#ifndef _BATTLEROOMLISTCTRL_H_
#define _BATTLEROOMLISTCTRL_H_

#include <wx/listctrl.h>
#include <wx/intl.h>
#include "user.h"
#include "utils.h"

class BattleroomListCtrl : public wxListCtrl
{
  public:
    BattleroomListCtrl( wxWindow* parent );
     ~BattleroomListCtrl();
  
    // BattleroomListCtrl interface
  
    void AddUser( User& user );
    void RemoveUser( User& user );
    void UpdateUser( const int& index );
    void UpdateUser( User& user );
    
    void UpdateList();
    
    int GetUserIndex( User& user );
    
  protected:
    // BattleroomListCtrl variables
  
  
  
};


#endif  //_BATTLEROOMLISTCTRL_H_

