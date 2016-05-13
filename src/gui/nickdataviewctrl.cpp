/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "nickdataviewctrl.h"

#include <wx/dataview.h>
#include <wx/defs.h>
#include <wx/dvrenderers.h>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/string.h>
#include <wx/translation.h>
#include <utility>

#include "user.h"
#include "utils/conversion.h"
#include "nickdataviewmodel.h"
#include "gui/ui.h"
#include "gui/mainwindow.h"
#include "gui/usermenu.h"
#include "gui/chatpanelmenu.h"

BEGIN_EVENT_TABLE(NickDataViewCtrl, BaseDataViewCtrl)
EVT_DATAVIEW_ITEM_ACTIVATED(NICK_DATAVIEW_CTRL_ID, NickDataViewCtrl::OnItemActivatedEvent)
EVT_DATAVIEW_ITEM_CONTEXT_MENU(NICK_DATAVIEW_CTRL_ID, NickDataViewCtrl::OnContextMenuEvent)
END_EVENT_TABLE()

NickDataViewCtrl::NickDataViewCtrl(const wxString& dataViewName, wxWindow* parent, bool show_header, ChatPanelMenu* popup, bool /*highlight*/)
    : BaseDataViewCtrl(dataViewName, parent, NICK_DATAVIEW_CTRL_ID)
{
	m_menu = popup;

	AppendBitmapColumn(_("s"), STATUS, wxDATAVIEW_CELL_INERT, wxDVC_DEFAULT_MINWIDTH, wxALIGN_CENTER, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendBitmapColumn(_("c"), COUNTRY, wxDATAVIEW_CELL_INERT, wxDVC_DEFAULT_MINWIDTH, wxALIGN_CENTER, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendBitmapColumn(_("r"), RANK, wxDATAVIEW_CELL_INERT, wxDVC_DEFAULT_MINWIDTH, wxALIGN_CENTER, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn(_("Nickname"), NICKNAME, wxDATAVIEW_CELL_INERT, wxDVC_DEFAULT_WIDTH + 10 /*add 10 px to let some nicks to fit better*/, wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);

	if (!show_header) {
		//TODO: implement "no header" style somehow
	}

	NickDataViewModel* model = new NickDataViewModel();
	AssociateModel(model);

	//Hide all bots in users list by default.
	m_userFilterShowPlayersOnly = true;

	LoadColumnProperties();
}

NickDataViewCtrl::~NickDataViewCtrl()
{
}

void NickDataViewCtrl::UserFilterShowPlayersOnly(bool showOnlyPlayers)
{
	m_userFilterShowPlayersOnly = showOnlyPlayers;

	DoUsersFilter();
}

void NickDataViewCtrl::SetUsersFilterString(const wxString& fs)
{
	m_UsersFilterString = fs.Lower();

	DoUsersFilter();
}

void NickDataViewCtrl::AddUser(const User& user)
{
	if (!AddRealUser(user)) {
		//User already added to widget
		return;
	}

	DoUsersFilter();
}

void NickDataViewCtrl::RemoveUser(const User& user)
{
	if (!RemoveRealUser(user)) {
		return;
	}

	//Only remove added users
	if (ContainsItem(user)) {
		RemoveItem(user);
	}
}

void NickDataViewCtrl::UserUpdated(const User& user)
{

	if (ContainsItem(user)) {
		RefreshItem(user);
	}
	DoUsersFilter();
}

void NickDataViewCtrl::SetUsers(const UserList::user_map_t& userlist)
{
	ClearUsers();

	for (const auto item : userlist) {
		AddRealUser(*item.second);
	}

	DoUsersFilter();
}

void NickDataViewCtrl::ClearUsers()
{
	Clear();
	ClearRealUsers();
}

int NickDataViewCtrl::GetUsersCount() const
{
	return GetItemsCount();
}

void NickDataViewCtrl::DoUsersFilter()
{

	for (auto const item : m_real_users_list) {
		if (checkFilteringConditions(item.second)) {
			//User passed filter. Add him/her to the list.
			if (!ContainsItem(*item.second)) {
				AddItem(*item.second);
			}
		} else {
			//Remove user from the list.
			if (ContainsItem(*item.second)) {
				RemoveItem(*item.second);
			}
		}
	}

	Resort();
	Refresh();
}

void NickDataViewCtrl::SetTipWindowText(const long /*item_hit*/,
					const wxPoint& /*position*/)
{
	//TODO: implement!
}

void NickDataViewCtrl::HighlightItem(long /*item*/)
{
	//TODO: implement!
}

bool NickDataViewCtrl::checkFilteringConditions(const User* user) const
{
	//Filter out bots
	if ((m_userFilterShowPlayersOnly) && (user->GetStatus().bot)) {
		return false;
	}
	//Check users nicks
	if (!TowxString(user->GetNick()).Lower().Contains(m_UsersFilterString)) {
		return false;
	}
	//All is good, user passed
	return true;
}

bool NickDataViewCtrl::AddRealUser(const User& user)
{
	if (IsContainsRealUser(user)) {
		return false;
	}

	m_real_users_list[user.GetNick()] = &user;
	return true;
}

bool NickDataViewCtrl::RemoveRealUser(const User& user)
{
	const auto it = m_real_users_list.find(user.GetNick());
	if (it == m_real_users_list.end()) {
		return false;
	}

	m_real_users_list.erase(it);
	return true;
}

void NickDataViewCtrl::ClearRealUsers()
{
	m_real_users_list.clear();
}

bool NickDataViewCtrl::IsContainsRealUser(const User& user) const
{
	const auto it = m_real_users_list.find(user.GetNick());
	if (it == m_real_users_list.end()) {
		return false;
	}
	return true;
}

void NickDataViewCtrl::OnItemActivatedEvent(wxDataViewEvent& /*event*/)
{

	User* user = GetSelectedItem();

	if (user == nullptr) {
		return;
	}

	//Open private chat with user
	ui().mw().OpenPrivateChat(*user, true); //true --> setfoucs
}

void NickDataViewCtrl::OnContextMenuEvent(wxDataViewEvent& /*event*/)
{
	if (m_menu != nullptr) {
		//no need to popup the menu when there's no user selected
		const User* user = GetSelectedItem();
		if (user == nullptr) {
			return;
		}

		wxString nick = wxString(user->GetNick());
		SL_GENERIC::UserMenu<ChatPanelMenu>* popup = m_menu->GetUserMenu();
		popup->EnableItems(true, nick);
		PopupMenu(popup);
	}
}
