#ifndef SPRINGLOBBY_HEADERGUARD_NICKLISTCTRL_H
#define SPRINGLOBBY_HEADERGUARD_NICKLISTCTRL_H

#include "customvirtlistctrl.h"
#include "usermenu.h"
#include "utils/mixins.hh"

class User;
class UserList;
class Ui;
class ChatPanelMenu;
class UserMenu;

class NickListCtrl : public CustomVirtListCtrl< const User* ,NickListCtrl >
{
  protected:
    typedef SL_GENERIC::UserMenu<ChatPanelMenu> UserMenu;

  public:
    NickListCtrl( wxWindow* parent, bool show_header = true, UserMenu* popup = 0,
        bool singleSelectList = true, const wxString& name = _T("NickListCtrl"), bool highlight = true  );
    virtual ~NickListCtrl();

    virtual void AddUser( const User& user );
    void RemoveUser( const User& user );

    void UserUpdated( const User& user );

    void ClearUsers();

    void OnActivateItem( wxListEvent& event );
    void OnShowMenu( wxContextMenuEvent& event );
    virtual void SetTipWindowText( const long item_hit, const wxPoint& position);

    void HighlightItem( long item );

    //these are overloaded to use list in virtual style
    wxString GetItemText(long item, long column) const;
    int GetItemColumnImage(long item, long column) const;
    wxListItemAttr * GetItemAttr(long item) const;

  protected:

    //! passed as callback to generic ItemComparator, returns -1,0,1 as per defined ordering
	int CompareOneCrit( DataType u1, DataType u2, int col, int dir ) const;
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

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2011

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

