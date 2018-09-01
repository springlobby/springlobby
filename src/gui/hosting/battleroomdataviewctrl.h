/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SRC_GUI_HOSTING_BATTLEROOMDATAVIEWCTRL_H_
#define SRC_GUI_HOSTING_BATTLEROOMDATAVIEWCTRL_H_

#include <vector>
#include "gui/basedataviewctrl.h"
#include "gui/usermenu.h"
class User;
class IBattle;
class wxCommandEvent;
class wxListEvent;
class wxPoint;
class wxString;
class wxMenu;
class wxMenuItem;


class BattleroomDataViewCtrl : public BaseDataViewCtrl<User>
{
public:
	BattleroomDataViewCtrl(const wxString& dataViewName, wxWindow* parent, IBattle* battle, bool readOnly, bool showInGame);
	virtual ~BattleroomDataViewCtrl();

	void SetBattle(IBattle* battle);

	void AddUser(User& user);
	void RemoveUser(User& user);
	void UpdateUser(User& user);

	void OnUserMenuCreateGroup(wxCommandEvent& event);
	void OnUserMenuAddToGroup(wxCommandEvent& event);
	void OnUserMenuDeleteFromGroup(wxCommandEvent& event);

private:
	void OnContextMenuEvent(wxDataViewEvent& event);
	void OnItemActivatedEvent(wxDataViewEvent& event);
	void OnAllySelectEvent(wxCommandEvent& event);
	void OnTeamSelectEvent(wxCommandEvent& event);

	void OnColourSelect(wxCommandEvent& event);
	void OnSideSelect(wxCommandEvent& event);
	void OnHandicapSelect(wxCommandEvent& event);
	void OnSpecSelect(wxCommandEvent& event);

	void OnKickPlayer(wxCommandEvent& event);
	void OnRingPlayer(wxCommandEvent& event);

	virtual void SetTipWindowText(const long item_hit, const wxPoint& position);

private:
	void CreateContextMenu();
	void UpdateContextMenuSides();

private:
	IBattle* m_Battle;

	bool m_ViewIsReadOnly;
	bool m_ShowInGame;

	SL_GENERIC::UserMenu<BattleroomDataViewCtrl>* m_contextMenu;
	wxMenu* m_sides;
	std::vector<wxMenuItem*> side_vector;
	wxMenuItem* m_spec_item;
	wxMenuItem* m_handicap_item;

private:
	enum {
		BATTLEROOM_VIEW_ID = wxID_HIGHEST,
		BATTLEROOM_VIEW_TEAM,
		BATTLEROOM_VIEW_ALLY = BATTLEROOM_VIEW_TEAM + 1000,
		BATTLEROOM_VIEW_COLOUR = BATTLEROOM_VIEW_ALLY + 1000,
		BATTLEROOM_VIEW_SIDE = BATTLEROOM_VIEW_COLOUR + 1000,
		BATTLEROOM_VIEW_HANDICAP = BATTLEROOM_VIEW_SIDE + 1000,
		BATTLEROOM_VIEW_SPEC,
		BATTLEROOM_VIEW_KICK,
		BATTLEROOM_VIEW_RING,
		BATTLEROOM_VIEW_ADDTOGROUP
	};
	enum ColumnIndexes {
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
