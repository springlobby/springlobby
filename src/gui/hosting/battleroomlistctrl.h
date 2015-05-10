/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_BATTLEROOMLISTCTRL_H
#define SPRINGLOBBY_HEADERGUARD_BATTLEROOMLISTCTRL_H

#include "gui/customvirtlistctrl.h"
#include "gui/usermenu.h"

class User;
class IBattle;
class Ui;
struct BattleBot;
class wxIcon;

/** \brief display participants of battle and their info (ally,team,color,...)
 * \todo DOCMEMORE */
class BattleroomListCtrl : public CustomVirtListCtrl<User*, BattleroomListCtrl>
{
public:
	BattleroomListCtrl(wxWindow* parent, IBattle* battle, bool readonly, bool showingame);
	~BattleroomListCtrl();

	void SetBattle(IBattle* battle);
	IBattle& GetBattle();

	void AddUser(User& user);
	void RemoveUser(User& user);
	void UpdateUser(User& user);

	int GetIndexFromData(const DataType& data) const;

	wxString GetItemText(long item, long column) const;
	int GetItemColumnImage(long item, long column) const;
	wxListItemAttr* GetItemAttr(long item) const;

	void OnUserMenuDeleteFromGroup(wxCommandEvent& event);
	void OnUserMenuCreateGroup(wxCommandEvent& event);

private:
	void UpdateUser(const int& index);
	void UpdateList();

	void OnListRightClick(wxListEvent& event);
	void OnColClick(wxListEvent& event);
	void OnTeamSelect(wxCommandEvent& event);
	void OnAllySelect(wxCommandEvent& event);
	void OnColourSelect(wxCommandEvent& event);
	void OnSideSelect(wxCommandEvent& event);
	void OnHandicapSelect(wxCommandEvent& event);
	void OnSpecSelect(wxCommandEvent& event);
	void OnActivateItem(wxListEvent& event);

	void OnKickPlayer(wxCommandEvent& event);
	void OnRingPlayer(wxCommandEvent& event);
	void OnUserMenuAddToGroup(wxCommandEvent& event);

	virtual void SetTipWindowText(const long item_hit, const wxPoint& position);
	static int CompareTrueSkill(const DataType user1, const DataType user2);
	static int CompareLobbyStatus(const DataType user1, const DataType user2);
	static int CompareStatus(const DataType user1, const DataType user2, const IBattle* m_battle);
	static int CompareSide(const DataType user1, const DataType user2);
	static int CompareColor(const DataType user1, const DataType user2);
	static int CompareRank(const DataType user1, const DataType user2);
	static int CompareTeam(const DataType user1, const DataType user2);
	static int CompareAlly(const DataType user1, const DataType user2);
	static int CompareHandicap(const DataType user1, const DataType user2);

	//    wxString GetCellContentsString( long row_number, int column );

	wxString GetSelectedUserNick();

	IBattle* m_battle;

	typedef SL_GENERIC::UserMenu<BattleroomListCtrl> UserMenu;
	UserMenu* m_popup;

	User* m_sel_user;

	wxMenu* m_sides;
	std::vector<wxMenuItem*> side_vector;
	wxMenuItem* m_spec_item;

	wxMenuItem* m_handicap_item;

	int CompareOneCrit(DataType u1, DataType u2, int col, int dir) const;

	virtual void Sort();

	bool m_ro;
	bool m_showingame;

	// if -1 then it's not set
	int m_status_column_index;
	int m_ingame_column_index;
	int m_faction_column_index;
	int m_colour_column_index;
	int m_country_column_index;
	int m_rank_column_index;
	int m_trueskill_column_index;
	int m_nick_column_index;
	int m_team_column_index;
	int m_ally_column_index;
	int m_resourcebonus_column_index;

	enum {
		BRLIST_LIST = wxID_HIGHEST,
		BRLIST_TEAM,
		BRLIST_ALLY = BRLIST_TEAM + 1000,
		BRLIST_COLOUR = BRLIST_ALLY + 1000,
		BRLIST_SIDE = BRLIST_COLOUR + 1000,
		BRLIST_HANDICAP = BRLIST_SIDE + 1000,
		BRLIST_SPEC,
		BRLIST_KICK,
		BRLIST_RING,
		BRLIST_ADDTOGROUP
	};

	DECLARE_EVENT_TABLE()
};


#endif // SPRINGLOBBY_HEADERGUARD_BATTLEROOMLISTCTRL_H
