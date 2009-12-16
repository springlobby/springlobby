#ifndef SPRINGLOBBY_HEADERGUARD_BATTLEROOMLISTCTRL_H
#define SPRINGLOBBY_HEADERGUARD_BATTLEROOMLISTCTRL_H

#include "customvirtlistctrl.h"
#include "usermenu.h"

class User;
class IBattle;
class Ui;
struct BattleBot;
class wxIcon;

/** \brief display participants of battle and their info (ally,team,color,cpu...)
 * \todo DOCMEMORE */
class BattleroomListCtrl : public CustomVirtListCtrl< User *, BattleroomListCtrl >
{
  public:
    BattleroomListCtrl( wxWindow* parent, IBattle* battle, bool readonly, bool showingame );
     ~BattleroomListCtrl();

    void SetBattle( IBattle* battle );
    IBattle& GetBattle();

    void AddUser( User& user );
    void RemoveUser( User& user );
    void UpdateUser( const int& index );
    void UpdateUser( User& user );
    void UpdateList();

    int GetIndexFromData( const DataType& data ) const;

    void OnListRightClick( wxListEvent& event );
    void OnColClick( wxListEvent& event );
    void OnTeamSelect( wxCommandEvent& event );
    void OnAllySelect( wxCommandEvent& event );
    void OnColourSelect( wxCommandEvent& event );
    void OnSideSelect( wxCommandEvent& event );
    void OnHandicapSelect( wxCommandEvent& event );
    void OnSpecSelect( wxCommandEvent& event );
    void OnActivateItem( wxListEvent& event );

    void OnKickPlayer( wxCommandEvent& event );
    void OnRingPlayer( wxCommandEvent& event );
    void OnUserMenuCreateGroup( wxCommandEvent& event );
    void OnUserMenuDeleteFromGroup( wxCommandEvent& event );
    void OnUserMenuAddToGroup( wxCommandEvent& event );
    virtual void SetTipWindowText( const long item_hit, const wxPoint& position);

    wxString GetItemText(long item, long column) const;
    int GetItemColumnImage(long item, long column) const;
    wxListItemAttr * GetItemAttr(long item) const;

  protected:
		static int CompareLobbyStatus( const DataType user1, const DataType user2 );
    static int CompareStatus(const DataType user1, const DataType user2, const IBattle* m_battle );
    static int CompareSide(const DataType user1, const DataType user2);
    static int CompareColor(const DataType user1, const DataType user2);
    static int CompareRank(const DataType user1, const DataType user2);
    static int CompareTeam(const DataType user1, const DataType user2);
    static int CompareAlly(const DataType user1, const DataType user2);
    static int CompareCpu(const DataType user1, const DataType user2);
    static int CompareHandicap(const DataType user1, const DataType user2);

//    wxString GetCellContentsString( long row_number, int column );

    wxString GetSelectedUserNick();

    IBattle* m_battle;
    static IBattle* s_battle;

    typedef SL_GENERIC::UserMenu<BattleroomListCtrl> UserMenu;
    UserMenu* m_popup;

    User* m_sel_user;

    wxMenu* m_sides;
    std::vector<wxMenuItem*> side_vector;
    wxMenuItem* m_spec_item;

    wxMenuItem* m_handicap_item;

    static int CompareOneCrit( DataType u1, DataType u2, int col, int dir ) ;

    virtual void Sort();

    bool m_ro;
    bool m_showingame;
    static bool s_showingame;

    enum {
      BRLIST_LIST = wxID_HIGHEST,
      BRLIST_TEAM,
      BRLIST_ALLY = BRLIST_TEAM + 1000,
      BRLIST_COLOUR = BRLIST_ALLY + 1000,
      BRLIST_SIDE = BRLIST_COLOUR +1000,
      BRLIST_HANDICAP = BRLIST_SIDE +1000,
      BRLIST_SPEC,
      BRLIST_KICK,
      BRLIST_RING,
      BRLIST_ADDTOGROUP
    };

    DECLARE_EVENT_TABLE();

};



#endif // SPRINGLOBBY_HEADERGUARD_BATTLEROOMLISTCTRL_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-09

    springsettings is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

