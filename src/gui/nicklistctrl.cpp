/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

//
// Class: NickListCtrl
//

#include <wx/imaglist.h>
#include <wx/menu.h>
#include <wx/string.h>
#include <wx/intl.h>
#include <stdexcept>
#include <wx/log.h>
#include <algorithm>

#include "nicklistctrl.h"
#include <lslutils/misc.h>
#include "utils/conversion.h"
#include "iconimagelist.h"
#include "user.h"
#include "settings.h"
#include "mainwindow.h"
#include "countrycodes.h"
#include "chatpanel.h"
#include "chatpanelmenu.h"
#include "userlist.h"
#include "usermenu.h"
#include "utils/sortutil.h"
#include "log.h"


BEGIN_EVENT_TABLE(NickListCtrl, NickListCtrl::BaseType)
EVT_LIST_ITEM_ACTIVATED(NICK_LIST, NickListCtrl::OnActivateItem)
EVT_CONTEXT_MENU(NickListCtrl::OnShowMenu)
#if wxUSE_TIPWINDOW
EVT_MOTION(NickListCtrl::OnMouseMotion)
#endif
END_EVENT_TABLE()

template <>
SortOrder NickListCtrl::BaseType::m_sortorder = SortOrder();

NickListCtrl::NickListCtrl(wxWindow* parent, bool show_header, ChatPanelMenu* popup, bool singleSelectList,
			   const wxString& name, bool highlight)
    : NickListCtrl::BaseType(parent, NICK_LIST, wxDefaultPosition, wxDefaultSize,
			     wxLC_VIRTUAL | wxSUNKEN_BORDER | wxLC_REPORT | (int)(!show_header) * wxLC_NO_HEADER | (int)(singleSelectList) * wxLC_SINGLE_SEL,
			     name, 3, &NickListCtrl::CompareOneCrit, highlight, UserActions::ActHighlight, true /*periodic sort*/)
    , m_menu(popup)
{
	AddColumn(0, 20, _("s"), _T( "Status" ));
	AddColumn(1, 20, _("c"), _T( "Country" ));
	AddColumn(2, 20, _("r"), _T( "Rank" ));
	AddColumn(3, 180, _("Nickname"), _T( "Nickname" ));

	if (m_sortorder.size() == 0) {
		m_sortorder[0].col = 0;
		m_sortorder[0].direction = -1;
		m_sortorder[1].col = 3;
		m_sortorder[1].direction = 1;
		m_sortorder[2].col = 2;
		m_sortorder[2].direction = 1;
		m_sortorder[3].col = 1;
		m_sortorder[3].direction = 1;
	}

	//Hide all bots in users list by default.
	m_userFilterShowPlayersOnly = true;
}

NickListCtrl::~NickListCtrl()
{
}

void NickListCtrl::AddUser(const User& user)
{
	if (GetIndexFromRealData(user) != -1) {
		wxLogWarning(_T( "Useralready in list." ));
		return;
	}

	m_real_users_list.push_back(&user);

	DoUsersFilter();
}

void NickListCtrl::RemoveUser(const User& user)
{
	int i = GetIndexFromRealData(user);

	if (i == -1) {
		wxLogError(_T( "Didn't find the user to remove." ));
		return;
	}

	m_real_users_list.erase(m_real_users_list.begin() + (unsigned int)i);
	RemoveItem(&user);
}


void NickListCtrl::UserUpdated(const User& user)
{
	int index = GetIndexFromRealData(user);
	if (index != -1) {
		m_real_users_list[index] = &user;
		DoUsersFilter();
	} else {
		wxLogWarning(_T( "NickListCtrl::UserUpdated error, index == -1 ." ));
	}
}

void NickListCtrl::ClearUsers()
{
	m_real_users_list.clear();
	Clear();
}

void NickListCtrl::SetUsersFilterString(const wxString& fs)
{
	m_UsersFilterString = fs.Lower();
	DoUsersFilter();
}

/**
 * Do filtering of the players list
 * Conditions for filtering: part of nick name (m_UsersFilterString)
 * and PlayersOnly boolean flag (m_userFilterShowPlayersOnly)
 */
void NickListCtrl::DoUsersFilter()
{
	for (const User* user: m_real_users_list) {
		if (checkFilteringConditions(user) == true) {
			//User passed filter. Add him/her to the list.
			AddItem(user);
		} else {
			//Remove user from the list. No need to check if user in the list, method will do it.
			RemoveItem(user);
		}
	}

	Sort();
	Update();
}

int NickListCtrl::GetIndexFromRealData(const User& user)
{
	unsigned int i;

	for (i = 0;
	     i < m_real_users_list.size();
	     i++)
		if (m_real_users_list.at(i)->GetNick().compare(user.GetNick()) == 0)
			return i;

	return -1;
}

void NickListCtrl::OnActivateItem(wxListEvent& event)
{
	int index = event.GetIndex();
	if (index == -1)
		return;

	const User* user = m_data[index];
	if (user) {
		ui().mw().OpenPrivateChat(*user, true); //true --> setfoucs
	}
	SetSelectedIndex(index);
}

void NickListCtrl::OnShowMenu(wxContextMenuEvent& /*unused*/)
{
	slLogDebugFunc("");
	if (m_menu != 0) {
		//no need to popup the menu when there's no user selected
		int selected = GetSelectedIndex();
		if (selected != -1 && m_data[selected]) {
			const User& user = *m_data[selected];
			wxString nick = TowxString(user.GetNick());
			SL_GENERIC::UserMenu<ChatPanelMenu>* popup = m_menu->GetUserMenu();
			popup->EnableItems((selected != -1), nick);
			PopupMenu(popup);
		}
	}
}

void NickListCtrl::SetTipWindowText(const long item_hit, const wxPoint& position)
{

	int column = getColumnFromPosition(position);
	if (column > (int)m_colinfovec.size() || column < 0 || item_hit < 0 || item_hit > (long)m_data.size() || m_data[item_hit] == NULL) {
		m_tiptext = wxEmptyString;
	} else {

		const User& user = *m_data[item_hit];
		{
			switch (column) {
				case 0: // status
					m_tiptext = _T( "This " );
					if (user.GetStatus().bot)
						m_tiptext << _T( "bot " );
					else if (user.GetStatus().moderator)
						m_tiptext << _T( "moderator " );
					else
						m_tiptext << _T( "player " );

					if (user.GetStatus().in_game)
						m_tiptext << _T( "is ingame" );
					else if (user.GetStatus().away)
						m_tiptext << _T( "is away" );
					else
						m_tiptext << _T( "is available" );
					break;

				case 1: // country
					m_tiptext = GetFlagNameFromCountryCode(TowxString(user.GetCountry()).Upper());
					break;

				case 2: // rank
					m_tiptext = TowxString(user.GetRankName(user.GetStatus().rank));
					break;

				case 3: // nickname
					m_tiptext = TowxString(user.GetNick());
					break;

				default:
					m_tiptext = m_colinfovec[column].tip;
					break;
			}
		}
	}
}

wxListItemAttr* NickListCtrl::GetItemAttr(long item) const
{
	if (item > -1 && item < (long)m_data.size() && m_data[item]) {
		const User& u = *m_data[item];
		wxString name = TowxString(u.GetNick());
		return HighlightItemUser(name);
	}
	return NULL;
}

void NickListCtrl::HighlightItem(long /*unused*/)
{
}

int NickListCtrl::GetIndexFromData(const DataType& data) const
{
	const User* user = data;
	static long seekpos;
	seekpos = LSL::Util::Clamp(seekpos, 0l, (long)m_data.size());
	int index = seekpos;

	for (DataCIter f_idx = m_data.begin() + seekpos; f_idx != m_data.end(); ++f_idx) {
		if (user == *f_idx) {
			seekpos = index;
			return seekpos;
		}
		index++;
	}
	//it's ok to init with seekpos, if it had changed this would not be reached
	int r_index = seekpos - 1;
	for (DataRevCIter r_idx = m_data.rbegin() + (m_data.size() - seekpos); r_idx != m_data.rend(); ++r_idx) {
		if (user == *r_idx) {
			seekpos = r_index;
			return seekpos;
		}
		r_index--;
	}

	return -1;
}

void NickListCtrl::Sort()
{
	if (m_data.size() > 0) {
		SaveSelection();
		SLInsertionSort(m_data, m_comparator);
		RestoreSelection();
	}
}

wxString NickListCtrl::GetItemText(long item, long column) const
{
	switch (column) {
		case 0:
		case 1:
		case 2:
		default:
			return wxEmptyString;

		case 3: {
			if (m_data[item])
				return TowxString(m_data[item]->GetNick());
			else
				return wxEmptyString;
		}
	}
	return wxEmptyString;
}

int NickListCtrl::GetItemColumnImage(long item, long column) const
{
	if (m_data[item]) {
		const User& user = *m_data[item];
		switch (column) {
			case 0:
				return user.GetStatusIconIndex();
			case 1:
				return user.GetFlagIconIndex();
			case 2:
				return user.GetRankIconIndex();

			case 3:
			default:
				return -1;
		}
	}
	return -1;
}

int NickListCtrl::CompareOneCrit(DataType u1, DataType u2, int col, int dir) const
{
	if (!(u1 && u2))
		return 0;
	switch (col) {
		case 0:
			return dir * CompareUserStatus(u1, u2);
		case 1:
			return dir * TowxString(u2->GetCountry()).CmpNoCase(TowxString(u1->GetCountry()));
		case 2:
			return dir * compareSimple(TowxString(u2->GetStatus().rank), TowxString(u1->GetStatus().rank));
		case 3:
			return dir * TowxString(u2->GetNick()).CmpNoCase(TowxString(u1->GetNick()));
		default:
			return 0;
	}
}

int NickListCtrl::CompareUserStatus(DataType user1, DataType user2)
{

	ASSERT_LOGIC(user1 != 0, "user1 = 0");
	ASSERT_LOGIC(user2 != 0, "user2 = 0");

	int u1 = 0, u2 = 0;

	if (user1->GetStatus().bot)
		u1 += 1000;
	if (user2->GetStatus().bot)
		u2 += 1000;
	if (user1->GetStatus().moderator)
		u1 += 100;
	if (user2->GetStatus().moderator)
		u2 += 100;
	if (user1->GetStatus().in_game)
		u1 += -10;
	if (user2->GetStatus().in_game)
		u2 += -10;

	// inverse the order
	if (u1 < u2)
		return -1;
	if (u1 > u2)
		return 1;

	return 0;
}

/**
 * Setter for showOnlyPlayer variable
 * Triggers filtering updating
 */
void NickListCtrl::UserFilterShowPlayersOnly(bool showOnlyPlayers) {
	m_userFilterShowPlayersOnly = showOnlyPlayers;
	DoUsersFilter();
}

/**
 * Check conditions for users list filtering
 * return true if User passes filter
 */
bool NickListCtrl::checkFilteringConditions(const User* user) {
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

int NickListCtrl::GetUsersCount() {
	return m_data.size();
}
