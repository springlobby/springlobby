#ifndef SPRINGLOBBY_HEADERGUARD_NICKLISTCTRL_H
#define SPRINGLOBBY_HEADERGUARD_NICKLISTCTRL_H

//#include <wx/listctrl.h>
#include <wx/event.h>
#include "customlistctrl.h"

class User;
class UserList;
class Ui;
class wxMenu;




class NickListCtrl : public customListCtrl
{
  public:
    NickListCtrl( wxWindow* parent, Ui& ui, bool show_header = true, wxMenu* popup = 0  );
    ~NickListCtrl();

    void AddUser( User& user );
    void RemoveUser( const User& user );

    void UserUpdated( User& user );
    void UserUpdated( const int& index );

    int GetUserIndex( User& user );

    void ClearUsers();

    void OnActivateItem( wxListEvent& event );
    void OnColClick( wxListEvent& event );
    void OnShowMenu( wxContextMenuEvent& event );
    void SetTipWindowText( const long item_hit, const wxPoint position);

  protected:
    static int wxCALLBACK ComparePlayernameUP(long item1, long item2, long sortData);
    static int wxCALLBACK ComparePlayernameDOWN(long item1, long item2, long sortData);
    static int wxCALLBACK ComparePlayerstatusUP(long item1, long item2, long sortData);
    static int wxCALLBACK ComparePlayerstatusDOWN(long item1, long item2, long sortData);
    static int wxCALLBACK ComparePlayerrankUP(long item1, long item2, long sortData);
    static int wxCALLBACK ComparePlayerrankDOWN(long item1, long item2, long sortData);
    static int wxCALLBACK ComparePlayercountryUP(long item1, long item2, long sortData);
    static int wxCALLBACK ComparePlayercountryDOWN(long item1, long item2, long sortData);
    void Sort();

    UserList* m_users;
    Ui& m_ui;

    wxMenu* m_menu;

    struct {
      int col;
      bool direction;
    } m_sortorder[4];

    DECLARE_EVENT_TABLE()
};

enum
{
  NICK_LIST = 31765 //wxID_HIGHEST
  //wxID_HIGHEST is used by BattleListCTRL. The cant be in the same Tab like BattleTab
};

#endif // SPRINGLOBBY_HEADERGUARD_NICKLISTCTRL_H
