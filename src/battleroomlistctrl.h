#ifndef SPRINGLOBBY_HEADERGUARD_BATTLEROOMLISTCTRL_H
#define SPRINGLOBBY_HEADERGUARD_BATTLEROOMLISTCTRL_H

#include "customlistctrl.h"
#include "usermenu.h"

class User;
class IBattle;
class Ui;
struct BattleBot;
class wxIcon;


/** \brief display participants of battle and their info (ally,team,color,cpu...)
 * \todo DOCMEMORE */
class BattleroomListCtrl : public CustomListCtrl
{
  public:
    BattleroomListCtrl( wxWindow* parent, IBattle* battle, Ui& ui, bool readonly );
     ~BattleroomListCtrl();

	  void SetBattle( IBattle* battle );
	  IBattle& GetBattle();

    void Sort();

    void AddUser( User& user );
    void RemoveUser( User& user );
    void UpdateUser( const int& index );
    void UpdateUser( User& user );
    void UpdateList();

    int GetUserIndex( User& user );

    void SortList();

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
    virtual void SetTipWindowText( const long item_hit, const wxPoint position);
    virtual void HighlightItem( long item );

  protected:
    static int wxCALLBACK CompareStatusUP(long item1, long item2, long sortData);
    static int wxCALLBACK CompareStatusDOWN(long item1, long item2, long sortData);
    static int wxCALLBACK CompareSideUP(long item1, long item2, long sortData);
    static int wxCALLBACK CompareSideDOWN(long item1, long item2, long sortData);
    static int wxCALLBACK CompareColorUP(long item1, long item2, long sortData);
    static int wxCALLBACK CompareColorDOWN(long item1, long item2, long sortData);
    static int wxCALLBACK CompareCountryUP(long item1, long item2, long sortData);
    static int wxCALLBACK CompareCountryDOWN(long item1, long item2, long sortData);
    static int wxCALLBACK CompareRankUP(long item1, long item2, long sortData);
    static int wxCALLBACK CompareRankDOWN(long item1, long item2, long sortData);
    static int wxCALLBACK CompareNicknameUP(long item1, long item2, long sortData);
    static int wxCALLBACK CompareNicknameDOWN(long item1, long item2, long sortData);
    static int wxCALLBACK CompareTeamUP(long item1, long item2, long sortData);
    static int wxCALLBACK CompareTeamDOWN(long item1, long item2, long sortData);
    static int wxCALLBACK CompareAllyUP(long item1, long item2, long sortData);
    static int wxCALLBACK CompareAllyDOWN(long item1, long item2, long sortData);
    static int wxCALLBACK CompareCpuUP(long item1, long item2, long sortData);
    static int wxCALLBACK CompareCpuDOWN(long item1, long item2, long sortData);
    static int wxCALLBACK CompareHandicapUP(long item1, long item2, long sortData);
    static int wxCALLBACK CompareHandicapDOWN(long item1, long item2, long sortData);
    wxString GetCellContentsString( long row_number, int column );

    wxString GetSelectedUserNick();

    IBattle* m_battle;

    typedef SL_GENERIC::UserMenu<BattleroomListCtrl> UserMenu;
    UserMenu* m_popup;

    User* m_sel_user;
    std::vector<User*> items;

    wxMenu* m_sides;
    wxMenuItem* m_spec_item;

    wxMenuItem* m_handicap_item;

    Ui& m_ui;
    static Ui* m_ui_for_sort;

		bool m_ro;

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
