/*
 * nickdataviewctrl.h
 *
 *  Created on: 27 июня 2015 г.
 *      Author: Руслан
 */

#ifndef SRC_GUI_NICKDATAVIEWCTRL_H_
#define SRC_GUI_NICKDATAVIEWCTRL_H_

#include "basedataviewctrl.h"

#include <map>

#include "userlist.h"

class wxWindow;
class ChatPanelMenu;
class wxString;
class User;
class wxPoint;

class NickDataViewCtrl: public BaseDataViewCtrl<User> {
public:
	NickDataViewCtrl(const wxString& dataViewName, wxWindow* parent, bool show_header = true, ChatPanelMenu* popup = 0,
    bool highlight = true);
	virtual ~NickDataViewCtrl();

	void UserFilterShowPlayersOnly(bool);
	void SetUsersFilterString(const wxString& fs);
	void AddUser(const User& user);
	void RemoveUser(const User& user);
	void UserUpdated(const User& user);
	void SetUsers(const UserList::user_map_t& userlist);
	void ClearUsers();
	int GetUsersCount() const;

private:
	bool AddRealUser(const User& user);
	bool RemoveRealUser(const User& user);
	void ClearRealUsers();
	bool IsContainsRealUser(const User& user) const;

	void OnItemActivatedEvent(wxDataViewEvent& event);
	void OnContextMenuEvent(wxDataViewEvent& event);

	void DoUsersFilter();
	void SetTipWindowText(const long item_hit, const wxPoint& position);

	void HighlightItem(long item);
//	void OnActivateItem(wxListEvent& event);
//	void OnShowMenu(wxContextMenuEvent& event);

	bool checkFilteringConditions(const User*) const;

private:
	bool m_userFilterShowPlayersOnly;
	ChatPanelMenu* m_menu;

	wxString m_UsersFilterString;		    //<- String with filter pattern for nicklist
	std::map<std::string, const User*> m_real_users_list; //<- actual list of users (not filtered)

private:
	enum {
		NICK_DATAVIEW_CTRL_ID = 31765 //wxID_HIGHEST
	};
	enum ColumnIndexes
	{
		STATUS = 0,
		COUNTRY,
		RANK,
		NICKNAME
	};

	DECLARE_EVENT_TABLE()
};

#endif /* SRC_GUI_NICKDATAVIEWCTRL_H_ */
