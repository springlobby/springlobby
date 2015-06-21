/*
 * battleroomdataviewctrl.h
 *
 *  Created on: 20 июня 2015 г.
 *      Author: Руслан
 */

#ifndef SRC_GUI_HOSTING_BATTLEROOMDATAVIEWCTRL_H_
#define SRC_GUI_HOSTING_BATTLEROOMDATAVIEWCTRL_H_

#include "gui/basedataviewctrl.h"

#include <vector>

class User;
class IBattle;
class wxCommandEvent;
class wxListEvent;
class wxPoint;
class wxString;
class wxMenu;
class wxMenuItem;

class BattleroomDataViewCtrl: public BaseDataViewCtrl<User> {
public:
	BattleroomDataViewCtrl(wxString& dataViewName, wxWindow* parent, IBattle* battle, bool readOnly, bool showInGame);
	virtual ~BattleroomDataViewCtrl();

	void SetBattle(IBattle* battle);
	IBattle& GetBattle();

	void AddUser(User& user);
	void RemoveUser(User& user);
	void UpdateUser(User& user);

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

private:
	wxString GetSelectedUserNick();

	IBattle* m_Battle;

	User* m_sel_user;

	wxMenu* m_sides;
	std::vector<wxMenuItem*> side_vector;
	wxMenuItem* m_spec_item;

	wxMenuItem* m_handicap_item;

	bool m_ViewIsReadOnly;
	bool m_ShowInGame;

private:
	enum {
		BATTLEROOM_VIEW_ID = wxID_HIGHEST,
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
	enum ColumnIndexes
	{
		STATUS = 0,
		INGAME,
		FACTION,
		COLOUR,
		COUNTRY,
		RANK,
		NICKNAME,
		TRUESKILL,
		TEAM,
		ALLY,
		BONUS
	};
	DECLARE_EVENT_TABLE()
};

#endif /* SRC_GUI_HOSTING_BATTLEROOMDATAVIEWCTRL_H_ */
