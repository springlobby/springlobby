/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_NICKLISTCTRL_H
#define SPRINGLOBBY_HEADERGUARD_NICKLISTCTRL_H

#include "customvirtlistctrl.h"
#include "usermenu.h"

class User;
class UserList;
class Ui;
class ChatPanelMenu;
class UserMenu;

class NickListCtrl : public CustomVirtListCtrl<const User*, NickListCtrl>
{
private:
	typedef SL_GENERIC::UserMenu<ChatPanelMenu> UserMenu;

public:
	NickListCtrl(wxWindow* parent, bool show_header = true, ChatPanelMenu* popup = 0,
		     bool singleSelectList = true, const wxString& name = _T("NickListCtrl"), bool highlight = true);
	virtual ~NickListCtrl();

	virtual void AddUser(const User& user);
	void RemoveUser(const User& user);

	void UserUpdated(const User& user);

	void ClearUsers();
	void SetUsersFilterString(wxString fs);
	void UserFilterShowPlayersOnly(bool);
	void DoUsersFilter();
	int GetUsersCount();

	void OnActivateItem(wxListEvent& event);
	void OnShowMenu(wxContextMenuEvent& event);
	virtual void SetTipWindowText(const long item_hit, const wxPoint& position);

	void HighlightItem(long item);

	//these are overloaded to use list in virtual style
	wxString GetItemText(long item, long column) const;
	int GetItemColumnImage(long item, long column) const;
	wxListItemAttr* GetItemAttr(long item) const;

private:
	bool checkFilteringConditions(const User*);
	//! passed as callback to generic ItemComparator, returns -1,0,1 as per defined ordering
	int CompareOneCrit(DataType u1, DataType u2, int col, int dir) const;
	//! utils func for comparing user status, so the CompareOneCrit doesn't get too crowded
	static int CompareUserStatus(DataType u1, DataType u2);
	//! required per base clase
	virtual void Sort();

	int GetIndexFromRealData(const User& user);
	int GetIndexFromData(const DataType& data) const;

private:
	bool m_userFilterShowPlayersOnly;
	ChatPanelMenu* m_menu;

	wxString m_UsersFilterString;		    //<- String with filter pattern for nicklist
	std::vector<const User*> m_real_users_list; //<- actual list of users (not filtered)

	enum {
		NICK_LIST = 31765 //wxID_HIGHEST
				  //wxID_HIGHEST is used by BattleListCTRL. The cant be in the same Tab like BattleTab
	};


	DECLARE_EVENT_TABLE()
};


#endif // SPRINGLOBBY_HEADERGUARD_NICKLISTCTRL_H
