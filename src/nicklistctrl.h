#ifndef SPRINGLOBBY_HEADERGUARD_NICKLISTCTRL_H
#define SPRINGLOBBY_HEADERGUARD_NICKLISTCTRL_H

#include "customvirtlistctrl.h"
#include "usermenu.h"

class User;
class UserList;
class Ui;
class ChatPanel;
class UserMenu;

class NickListCtrl : public CustomVirtListCtrl<const User*>
{
  protected:
    typedef SL_GENERIC::UserMenu<ChatPanel> UserMenu;

  public:
    NickListCtrl( wxWindow* parent, bool show_header = true, UserMenu* popup = 0,
        bool singleSelectList = true, const wxString& name = _T("NickListCtrl"), bool highlight = true  );
    virtual ~NickListCtrl();

    virtual void AddUser( const User& user );
    void RemoveUser( const User& user );

    void UserUpdated( const User& user );

    void GetSelectedUsers( UserList& users ) ;
    wxArrayString GetSelectedUserNicks( ) ;

    void ClearUsers();

    void OnActivateItem( wxListEvent& event );
    void OnColClick( wxListEvent& event );
    void OnShowMenu( wxContextMenuEvent& event );
    virtual void SetTipWindowText( const long item_hit, const wxPoint position);

    void HighlightItem( long item );

    //these are overloaded to use list in virtual style
    virtual wxString OnGetItemText(long item, long column) const;
    virtual int OnGetItemImage(long item) const;
    virtual int OnGetItemColumnImage(long item, long column) const;
    wxListItemAttr * OnGetItemAttr(long item) const;

  protected:

    //! passed as callback to generic ItemComparator, returns -1,0,1 as per defined ordering
    static int CompareOneCrit( DataType u1, DataType u2, int col, int dir );
    //! utils func for comparing user status, so the CompareOneCrit doesn't get too crowded
    static int CompareUserStatus( DataType u1, DataType u2 );
    //! required per base clase
    virtual void Sort( );

    int GetIndexFromData( const DataType& data ) const;

    UserMenu* m_menu;

    enum {
      NICK_LIST = 31765 //wxID_HIGHEST
      //wxID_HIGHEST is used by BattleListCTRL. The cant be in the same Tab like BattleTab
    };


    DECLARE_EVENT_TABLE()
};


#endif // SPRINGLOBBY_HEADERGUARD_NICKLISTCTRL_H
