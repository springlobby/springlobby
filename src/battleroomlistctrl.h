//
// Class: BattleroomListCtrl
//

#ifndef _BATTLEROOMLISTCTRL_H_
#define _BATTLEROOMLISTCTRL_H_

#include <wx/listctrl.h>

class User;

class BattleroomListCtrl : public wxListCtrl
{
  public:
    BattleroomListCtrl( wxWindow* parent );
     ~BattleroomListCtrl();

    void AddUser( User& user );
    void RemoveUser( User& user );
    void UpdateUser( const int& index );
    void UpdateUser( User& user );
    void UpdateList();
    int GetUserIndex( User& user );
  protected:

};


#endif  //_BATTLEROOMLISTCTRL_H_

