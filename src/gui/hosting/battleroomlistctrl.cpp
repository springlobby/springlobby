/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

//
// Class: BattleroomListCtrl
//

#include <wx/intl.h>
#include <wx/menu.h>
#include <wx/numdlg.h>
#include <wx/colordlg.h>
#include <wx/colour.h>
#include <wx/log.h>

#include <stdexcept>
#include <vector>


#include "battleroomlistctrl.h"
#include "iconimagelist.h"
#include "ibattle.h"
#include "gui/uiutils.h"
#include "user.h"
#include "iserver.h"
#include "utils/conversion.h"
#include "gui/uiutils.h"
#include "countrycodes.h"
#include "gui/mainwindow.h"
#include "aui/auimanager.h"
#include "gui/customdialogs.h"
#include "settings.h"
#include "log.h"
#include "utils/lslconversion.h"

#include <lslunitsync/unitsync.h>


template <>
SortOrder CustomVirtListCtrl<User*, BattleroomListCtrl>::m_sortorder = SortOrder();

BEGIN_EVENT_TABLE(BattleroomListCtrl, BattleroomListCtrl::BaseType)

EVT_LIST_ITEM_RIGHT_CLICK(BRLIST_LIST, BattleroomListCtrl::OnListRightClick)
EVT_MENU(BRLIST_SPEC, BattleroomListCtrl::OnSpecSelect)
EVT_MENU(BRLIST_KICK, BattleroomListCtrl::OnKickPlayer)
EVT_LIST_ITEM_ACTIVATED(BRLIST_LIST, BattleroomListCtrl::OnActivateItem)
//  EVT_MENU                 ( BRLIST_ADDCREATEGROUP, BattleroomListCtrl::OnPlayerAddToGroup )
//  EVT_MENU                 ( BRLIST_ADDTOGROUP, BattleroomListCtrl::OnPlayerAddToGroup )
EVT_MENU(BRLIST_RING, BattleroomListCtrl::OnRingPlayer)
EVT_MENU(BRLIST_COLOUR, BattleroomListCtrl::OnColourSelect)
EVT_MENU(BRLIST_HANDICAP, BattleroomListCtrl::OnHandicapSelect)
#if wxUSE_TIPWINDOW
EVT_MOTION(BattleroomListCtrl::OnMouseMotion)
#endif
END_EVENT_TABLE()


BattleroomListCtrl::BattleroomListCtrl(wxWindow* parent, IBattle* battle, bool readonly, bool showingame)
    : CustomVirtListCtrl<User*, BattleroomListCtrl>(parent, BRLIST_LIST, wxDefaultPosition, wxDefaultSize,
                                                    wxSUNKEN_BORDER | wxLC_REPORT | wxLC_SINGLE_SEL, _T("BattleroomListCtrl"),
                                                    3, &BattleroomListCtrl::CompareOneCrit,
                                                    true /*highlight*/, UserActions::ActHighlight, !readonly /*periodic sort*/)
    , m_battle(battle)
    , m_popup(0)
    , m_sel_user(0)
    , m_sides(0)
    , m_spec_item(0)
    , m_handicap_item(0)
    , m_ro(readonly)
    , m_showingame(showingame)
    , m_status_column_index(-1)
    , m_ingame_column_index(-1)
    , m_faction_column_index(-1)
    , m_colour_column_index(-1)
    , m_country_column_index(-1)
    , m_rank_column_index(-1)
    , m_trueskill_column_index(-1)
    , m_nick_column_index(-1)
    , m_team_column_index(-1)
    , m_ally_column_index(-1)
    , m_resourcebonus_column_index(-1)
{
        GetAui().manager->AddPane(this, wxLEFT, _T("battleroomlistctrl"));

        wxListItem col;

	int count = 0;
	AddColumn(count, wxLIST_AUTOSIZE_USEHEADER, _("Status"), _T("Player/Bot"));
	m_status_column_index = count;
	count++;
	if (m_showingame) {
		AddColumn(count, 55, _("Ingame"), _T("Battleroom status"));
		m_ingame_column_index = count;
		count++;
	}

	AddColumn(count, wxLIST_AUTOSIZE_USEHEADER, _("Faction"), _T("Faction icon"));
	m_faction_column_index = count;
	count++;
	AddColumn(count, wxLIST_AUTOSIZE_USEHEADER, _("Colour"), _T("Teamcolour"));
	m_colour_column_index = count;
	count++;
	AddColumn(count, wxLIST_AUTOSIZE_USEHEADER, _("Country"), _T("Country"));
	m_country_column_index = count;
	count++;
	AddColumn(count, wxLIST_AUTOSIZE_USEHEADER, _("Rank"), _T("Rank"));
	m_rank_column_index = count;
	count++;
	AddColumn(count, 165, _("Nickname"), _T("Ingame name"));
	m_nick_column_index = count;
	count++;
	AddColumn(count, wxLIST_AUTOSIZE_USEHEADER, _T("TrueSkill"), _T("TrueSkill")); //No translation for TrueSkill word
	m_trueskill_column_index = count;
	count++;
	AddColumn(count, wxLIST_AUTOSIZE_USEHEADER, _("Team"), _T("Team number"));
	m_team_column_index = count;
	count++;
	AddColumn(count, wxLIST_AUTOSIZE_USEHEADER, _("Ally"), _T("Ally number"));
	m_ally_column_index = count;
	count++;
	AddColumn(count, wxLIST_AUTOSIZE_USEHEADER, _("Resource Bonus"), _T("Resource Bonus"));
	m_resourcebonus_column_index = count;

	if (m_sortorder.size() == 0) {
		m_sortorder[0].col = m_team_column_index;
		m_sortorder[0].direction = true;
		m_sortorder[1].col = m_ally_column_index;
		m_sortorder[1].direction = true;
		m_sortorder[2].col = m_nick_column_index;
		m_sortorder[2].direction = true;
	}

	if (!m_ro) {
		m_popup = new UserMenu(this, this);
		wxMenu* m_teams;
		m_teams = new wxMenu();

		for (unsigned int i = 0; i < SPRING_MAX_TEAMS; i++) {
			wxMenuItem* team = new wxMenuItem(m_teams, BRLIST_TEAM + i, wxString::Format(_T("%d"), i + 1), wxEmptyString, wxITEM_NORMAL);
			m_teams->Append(team);
			Connect(BRLIST_TEAM + i, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(BattleroomListCtrl::OnTeamSelect));
		}
		m_popup->Append(-1, _("Team"), m_teams);

		wxMenu* m_allies = new wxMenu();
		for (unsigned int i = 0; i < SPRING_MAX_ALLIES; i++) {
			wxMenuItem* ally = new wxMenuItem(m_allies, BRLIST_ALLY + i, wxString::Format(_T("%d"), i + 1), wxEmptyString, wxITEM_NORMAL);
			m_allies->Append(ally);
			Connect(BRLIST_ALLY + i, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(BattleroomListCtrl::OnAllySelect));
		}
		m_popup->Append(-1, _("Ally"), m_allies);

		m_sides = new wxMenu();
		m_popup->Append(-1, _("Side"), m_sides);

		m_popup->AppendSeparator();

		wxMenuItem* m_colours = new wxMenuItem(m_popup, BRLIST_COLOUR, _("Set color"), wxEmptyString, wxITEM_NORMAL);
		m_popup->Append(m_colours);

		m_popup->AppendSeparator();

		m_handicap_item = new wxMenuItem(m_popup, BRLIST_HANDICAP, _("Set Resource Bonus"), wxEmptyString, wxITEM_NORMAL);
		m_popup->Append(m_handicap_item);

		m_popup->AppendSeparator();

		m_spec_item = new wxMenuItem(m_popup, BRLIST_SPEC, wxString(_("Spectator")), wxEmptyString, wxITEM_CHECK);
		m_popup->Append(m_spec_item);

		m_popup->AppendSeparator();

		wxMenuItem* kick = new wxMenuItem(m_popup, BRLIST_KICK, wxString(_("Kick")), wxEmptyString, wxITEM_NORMAL);
		m_popup->Append(kick);
		wxMenuItem* ring = new wxMenuItem(m_popup, BRLIST_RING, wxString(_("Ring")), wxEmptyString, wxITEM_NORMAL);
		m_popup->Append(ring);

		SetBattle(battle);
	}
}


BattleroomListCtrl::~BattleroomListCtrl()
{
}

void BattleroomListCtrl::SetBattle(IBattle* battle)
{
	if (m_battle != NULL) {
		for (unsigned int i = 0; i < side_vector.size(); i++) {
			m_sides->Destroy(side_vector[i]); // delete side;
			Disconnect(BRLIST_SIDE + i, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(BattleroomListCtrl::OnSideSelect));
		}
	}

	m_data.clear();
	side_vector.clear();

	if ((battle != NULL) && m_sides) {
		try {
			const wxArrayString sides = lslTowxArrayString(LSL::usync().GetSides(battle->GetHostModName()));
			for (unsigned int i = 0; i < sides.GetCount(); i++) {
				wxMenuItem* side = new wxMenuItem(m_sides, BRLIST_SIDE + i, sides[i], wxEmptyString, wxITEM_NORMAL);
				m_sides->Append(side);
				side_vector.push_back(side);
				Connect(BRLIST_SIDE + i, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(BattleroomListCtrl::OnSideSelect));
			}
		} catch (...) {
		}
	}
	m_battle = battle;
}

IBattle& BattleroomListCtrl::GetBattle()
{
	ASSERT_EXCEPTION(m_battle, _T("m_battle == 0"));
	return *m_battle;
}

void BattleroomListCtrl::AddUser(User& user)
{
	//first time setting is necessary to have color in replay/savegame used controls
	if (!user.BattleStatus().spectator) {
		icons().SetColourIcon(user.BattleStatus().colour);
		UpdateUser(user);
	}
	if (AddItem(&user))
		return;

	wxLogWarning(_T("user already in battleroom list."));
}

void BattleroomListCtrl::RemoveUser(User& user)
{
	if (RemoveItem(&user))
		return;

	wxLogWarning(_T("Didn't find the user to remove in battleroomlistctrl."));
}

void BattleroomListCtrl::UpdateUser(User& user)
{
	if (!user.BattleStatus().spectator)
		icons().SetColourIcon(user.BattleStatus().colour);
	try {
		wxArrayString sides = lslTowxArrayString(LSL::usync().GetSides(m_battle->GetHostModName()));
		if (user.BattleStatus().side < (long)sides.GetCount()) {
			user.SetSideiconIndex(icons().GetSideIcon(m_battle->GetHostModName(), user.BattleStatus().side));
		}
	} catch (...) {
	}
	int index = GetIndexFromData(&user);
	UpdateUser(index);
}

wxListItemAttr* BattleroomListCtrl::GetItemAttr(long item) const
{
	if (item == -1 || item >= (long)m_data.size())
		return NULL;

	const User& user = *GetDataFromIndex(item);
	bool is_bot = user.BattleStatus().IsBot();

	if (!is_bot) {
		return HighlightItemUser(TowxString(user.GetNick()));
	}

	return NULL;
}

int BattleroomListCtrl::GetItemColumnImage(long item, long column) const
{
	if ((item == -1) || (item >= (long)m_data.size()) || (m_battle == NULL))
		return -1;

	const User& user = *GetDataFromIndex(item);
	const bool is_bot = user.BattleStatus().IsBot();
	const bool is_spec = user.BattleStatus().spectator;
	if (column == m_status_column_index) {
		if (!is_bot) {
			if (m_battle->IsFounder(user)) {
				return icons().GetHostIcon(is_spec);
			} else {
				return icons().GetReadyIcon(is_spec, user.BattleStatus().ready, user.BattleStatus().sync, is_bot);
			}
		} else {
			return icons().ICON_BOT;
		}
	}
	if (column == m_ingame_column_index)
		return user.GetStatusIconIndex();
	if (column == m_colour_column_index)
		return is_spec ? -1 : icons().GetColourIcon(user.BattleStatus().colour);
	if (column == m_country_column_index)
		return is_bot ? -1 : icons().GetFlagIcon(user.GetCountry());
	if (column == m_rank_column_index)
		return is_bot ? -1 : icons().GetRankIcon(user.GetStatus().rank);
	if (column == m_faction_column_index)
		return is_spec ? -1 : user.GetSideiconIndex();
	if (column == m_nick_column_index)
		return -1;
	if (column == m_team_column_index)
		return -1;
	if (column == m_ally_column_index)
		return -1;
	if (column == m_resourcebonus_column_index)
		return -1;
	if (column == m_trueskill_column_index)
		return -1;

	const wxString msg = wxString::Format(_("column oob in BattleroomListCtrl::OnGetItemColumnImage: %d"), column);
	wxLogWarning(msg);
	return -1;
}

wxString BattleroomListCtrl::GetItemText(long item, long column) const
{
	if ((item == -1) || (item >= (long)m_data.size()) || (m_battle == NULL))
		return wxEmptyString;

	const User& user = *GetDataFromIndex(item);
	bool is_bot = user.BattleStatus().IsBot();
	bool is_spec = user.BattleStatus().spectator;

	if (column == m_faction_column_index) {
		try {
			auto sides = LSL::usync().GetSides(m_battle->GetHostModName());
			ASSERT_EXCEPTION(user.BattleStatus().side < (long)sides.size(), _T("Side index too high"));
		} catch (...) {
			return wxString::Format(_T("s%d"), user.BattleStatus().side + 1);
		}
		return wxEmptyString;
	}
	if (column == m_nick_column_index) {
		if (is_bot) {
			wxString botname = TowxString(user.BattleStatus().aishortname);
			if (!user.BattleStatus().aiversion.empty())
				botname += _T(" ") + TowxString(user.BattleStatus().aiversion);
			return wxString::Format(_T("%s - %s (%s)"), TowxString(user.GetNick()).c_str(), botname.c_str(), TowxString(user.BattleStatus().owner).c_str());
		} else
			return TowxString(user.GetNick());
	}
	if (column == m_team_column_index)
		return is_spec ? wxString(wxEmptyString) : (wxString::Format(_T("%d"), user.BattleStatus().team + 1));
	if (column == m_ally_column_index)
		return is_spec ? wxString(wxEmptyString) : (wxString::Format(_T("%d"), user.BattleStatus().ally + 1));
	if (column == m_resourcebonus_column_index)
		return is_spec ? wxString(wxEmptyString) : (wxString::Format(_T("%d%%"), user.BattleStatus().handicap));
	if (column == m_country_column_index)
		return wxEmptyString;
	if (column == m_trueskill_column_index)
		return user.GetTrueSkill() == 0 ? _T("-") : (wxString::Format(_T("%d"), user.GetTrueSkill()));

	return wxEmptyString;
}

void BattleroomListCtrl::UpdateUser(const int& index)
{
	wxWindowUpdateLocker lock(this);
	RefreshItem(index);
	MarkDirtySort();
}

void BattleroomListCtrl::OnListRightClick(wxListEvent& event)
{
	slLogDebugFunc("");
	if (m_ro)
		return;
	int index = event.GetIndex();

	if (index == -1 || index >= (long)m_data.size())
		return;

	User& user = *GetDataFromIndex(event.GetIndex());
	m_sel_user = &user; //this is set for event handlers

	if (user.BattleStatus().IsBot()) {
		wxLogMessage(_T("Bot"));

		int item = m_popup->FindItem(_("Spectator"));
		m_popup->Enable(item, false);
		m_popup->Check(item, false);
		m_popup->Enable(m_popup->FindItem(_("Ring")), false);
		m_popup->Enable(m_popup->FindItem(_("Kick")), true);
	} else {
		wxLogMessage(_T("User"));
		assert(m_popup);
		int item = m_popup->FindItem(_("Spectator"));
		m_popup->Check(item, m_sel_user->BattleStatus().spectator);
		m_popup->Enable(item, true);
		m_popup->Enable(m_popup->FindItem(_("Ring")), true);
		bool isSelUserMe = (ui().IsThisMe(user));
		m_popup->Enable(m_popup->FindItem(_("Kick")), !isSelUserMe);
	}

	wxLogMessage(_T("Popup"));
	m_popup->EnableItems(!user.BattleStatus().IsBot(), GetSelectedUserNick()); //this updates groups, therefore we need to update the connection to evt handlers too
	std::vector<int> groups_ids = m_popup->GetGroupIds();
	for (std::vector<int>::const_iterator it = groups_ids.begin(); it != groups_ids.end(); ++it) {
		Connect(*it, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(BattleroomListCtrl::OnUserMenuAddToGroup), 0, this);
	}
	Connect(GROUP_ID_NEW, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(BattleroomListCtrl::OnUserMenuCreateGroup), 0, this);
	PopupMenu(m_popup);
	wxLogMessage(_T("Done"));
}


void BattleroomListCtrl::OnTeamSelect(wxCommandEvent& event)
{
	slLogDebugFunc("");
	int team = event.GetId() - BRLIST_TEAM;
	if (m_sel_user)
		m_battle->ForceTeam(*m_sel_user, team);
}


void BattleroomListCtrl::OnAllySelect(wxCommandEvent& event)
{
	slLogDebugFunc("");
	int ally = event.GetId() - BRLIST_ALLY;
	if (m_sel_user)
		m_battle->ForceAlly(*m_sel_user, ally);
}


void BattleroomListCtrl::OnColourSelect(wxCommandEvent& /*unused*/)
{
	slLogDebugFunc("");

	wxColour CurrentColour = lslTowxColour(m_sel_user->BattleStatus().colour);
	CurrentColour = GetColourFromUser(this, CurrentColour);
	if (!CurrentColour.IsOk())
		return;
	if (m_sel_user)
		m_battle->ForceColour(*m_sel_user, wxColourTolsl(CurrentColour));
}


void BattleroomListCtrl::OnSideSelect(wxCommandEvent& event)
{
	slLogDebugFunc("");
	int side = event.GetId() - BRLIST_SIDE;
	if (m_sel_user)
		m_battle->ForceSide(*m_sel_user, side);
}


void BattleroomListCtrl::OnHandicapSelect(wxCommandEvent& /*unused*/)
{
	slLogDebugFunc("");
	if (!m_sel_user)
		return;
	long handicap = wxGetNumberFromUser(_("Please enter a value between 0 and 100"), _("Set Resource Bonus"), wxEmptyString, m_sel_user->BattleStatus().handicap, 0, 100, (wxWindow*)&ui().mw(), wxDefaultPosition);
	if (handicap != -1) {
		m_battle->SetHandicap(*m_sel_user, handicap);
	}
}


void BattleroomListCtrl::OnSpecSelect(wxCommandEvent& /*unused*/)
{
	slLogDebugFunc("");
	if (m_sel_user)
		m_battle->ForceSpectator(*m_sel_user, m_spec_item->IsChecked());
}


void BattleroomListCtrl::OnKickPlayer(wxCommandEvent& /*unused*/)
{
	slLogDebugFunc("");
	if (m_sel_user)
		m_battle->KickPlayer(*m_sel_user);
}


void BattleroomListCtrl::OnRingPlayer(wxCommandEvent& /*unused*/)
{
	slLogDebugFunc("");
	if (m_sel_user)
		m_battle->GetServer().Ring(m_sel_user->GetNick());
}

void BattleroomListCtrl::Sort()
{
	if (m_data.size() > 0) {
		SaveSelection();
		SLInsertionSort(m_data, m_comparator);
		RestoreSelection();
	}
}

int BattleroomListCtrl::CompareOneCrit(DataType u1, DataType u2, int col, int dir) const
{
	if (col == m_status_column_index)
		return dir * CompareStatus(u1, u2, m_battle);
	if (col == m_ingame_column_index)
		return dir * CompareLobbyStatus(u1, u2);
	if (col == m_faction_column_index)
		return dir * CompareSide(u1, u2);
	if (col == m_colour_column_index)
		return dir * CompareColor(u1, u2);
	if (col == m_country_column_index)
		return dir * compareSimple(u1, u2);
	if (col == m_rank_column_index)
		return dir * CompareRank(u1, u2);
	if (col == m_nick_column_index)
		return dir * TowxString(u1->GetNick()).CmpNoCase(TowxString(u2->GetNick()));
	if (col == m_team_column_index)
		return dir * CompareTeam(u1, u2);
	if (col == m_ally_column_index)
		return dir * CompareAlly(u1, u2);
	if (col == m_resourcebonus_column_index)
		return dir * CompareHandicap(u1, u2);
	if (col == m_trueskill_column_index)
		return dir * CompareTrueSkill(u1, u2);

	return 0;
}

int BattleroomListCtrl::CompareTrueSkill(const DataType user1, const DataType user2)
{
	if(user1->GetTrueSkill() > user2->GetTrueSkill())
	{
		return 1;
	}

	if(user1->GetTrueSkill() < user2->GetTrueSkill())
	{
		return -1;
	}

	return 0;
}

int BattleroomListCtrl::CompareLobbyStatus(const DataType user1, const DataType user2)
{
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
	if (user1->GetStatus().away)
		u1 += -5;
	if (user2->GetStatus().away)
		u2 += -5;

	// inverse the order
	if (u1 < u2)
		return -1;
	if (u1 > u2)
		return 1;

	return 0;
}

int BattleroomListCtrl::CompareStatus(const DataType user1, const DataType user2, const IBattle* battle)
{
	int status1 = 0;
	if (user1->BattleStatus().IsBot()) {
		status1 = 9;
	} else {
		try {
			if (&battle->GetFounder() != user1)
				status1 = 1;
		} catch (...) {
		}
		if (user1->BattleStatus().ready)
			status1 += 5;
		if (user1->BattleStatus().sync)
			status1 += 7;
		if (user1->BattleStatus().spectator)
			status1 += 10;
	}

	int status2 = 0;
	if (user2->BattleStatus().IsBot()) {
		status2 = 9;
	} else {
		try {
			if (&battle->GetFounder() != user2)
				status2 = 1;
		} catch (...) {
		}
		if (user2->BattleStatus().ready)
			status2 += 5;
		if (user2->BattleStatus().sync)
			status2 += 7;
		if (user2->BattleStatus().spectator)
			status2 += 10;
	}

	if (status1 < status2)
		return -1;
	if (status1 > status2)
		return 1;

	return 0;
}

int BattleroomListCtrl::CompareSide(const DataType user1, const DataType user2)
{
	if (!user1 && !user2)
		return 0;
	else if (!user1)
		return 1;
	else if (!user2)
		return -1;

	int side1;
	if (user1->BattleStatus().spectator)
		side1 = 1000;
	else
		side1 = user1->BattleStatus().side;

	int side2;
	if (user2->BattleStatus().spectator)
		side2 = 1000;
	else
		side2 = user2->BattleStatus().side;

	if (side1 < side2)
		return -1;
	if (side1 > side2)
		return 1;

	return 0;
}

int BattleroomListCtrl::CompareColor(const DataType user1, const DataType user2)
{
	if (!user1 && !user2)
		return 0;
	else if (!user1)
		return 1;
	else if (!user2)
		return -1;

	int color1_r, color1_g, color1_b;

	if (user1->BattleStatus().spectator)
		return -1;
	color1_r = user1->BattleStatus().colour.Red();
	color1_g = user1->BattleStatus().colour.Green();
	color1_b = user1->BattleStatus().colour.Blue();

	int color2_r, color2_g, color2_b;

	if (user2->BattleStatus().spectator)
		return 1;
	color2_r = user2->BattleStatus().colour.Red();
	color2_g = user2->BattleStatus().colour.Green();
	color2_b = user2->BattleStatus().colour.Blue();

	if ((color1_r + color1_g + color1_b) / 3 < (color2_r + color2_g + color2_b) / 3)
		return -1;
	if ((color1_r + color1_g + color1_b) / 3 > (color2_r + color2_g + color2_b) / 3)
		return 1;

	return 0;
}


int BattleroomListCtrl::CompareRank(const DataType user1, const DataType user2)
{
	if (!user1 && !user2)
		return 0;
	else if (!user1)
		return 1;
	else if (!user2)
		return -1;

	int rank1;
	if (user1->BattleStatus().IsBot())
		rank1 = 1000;
	else
		rank1 = user1->GetStatus().rank;

	int rank2;
	if (user2->BattleStatus().IsBot())
		rank2 = 1000;
	else
		rank2 = user2->GetStatus().rank;

	if (rank1 < rank2)
		return -1;
	if (rank1 > rank2)
		return 1;

	return 0;
}

int BattleroomListCtrl::CompareTeam(const DataType user1, const DataType user2)
{
	if (!user1 && !user2)
		return 0;
	else if (!user1)
		return 1;
	else if (!user2)
		return -1;

	int team1;
	if (user1->BattleStatus().spectator)
		team1 = 1000;
	else
		team1 = user1->BattleStatus().team;

	int team2;
	if (user2->BattleStatus().spectator)
		team2 = 1000;
	else
		team2 = user2->BattleStatus().team;

	if (team1 < team2)
		return -1;
	if (team1 > team2)
		return 1;

	return 0;
}

int BattleroomListCtrl::CompareAlly(const DataType user1, const DataType user2)
{
	if (!user1 && !user2)
		return 0;
	else if (!user1)
		return 1;
	else if (!user2)
		return -1;

	int ally1;
	if (user1->BattleStatus().spectator)
		ally1 = 1000;
	else
		ally1 = user1->BattleStatus().ally;

	int ally2;
	if (user2->BattleStatus().spectator)
		ally2 = 1000;
	else
		ally2 = user2->BattleStatus().ally;

	if (ally1 < ally2)
		return -1;
	if (ally1 > ally2)
		return 1;

	return 0;
}


int BattleroomListCtrl::CompareHandicap(const DataType user1, const DataType user2)
{
	int handicap1;
	if (user1->BattleStatus().spectator)
		handicap1 = 1000;
	else
		handicap1 = user1->BattleStatus().handicap;

	int handicap2;
	if (user2->BattleStatus().spectator)
		handicap2 = 1000;
	else
		handicap2 = user2->BattleStatus().handicap;

	if (handicap1 < handicap2)
		return -1;
	if (handicap1 > handicap2)
		return 1;

	return 0;
}

void BattleroomListCtrl::SetTipWindowText(const long item_hit, const wxPoint& position)
{
	if (item_hit < 0 || item_hit >= (long)m_data.size())
		return;
	if (!m_battle)
		return;
	const User& user = *GetDataFromIndex(item_hit);

	int column = getColumnFromPosition(position);
	if (column > (int)m_colinfovec.size() || column < 0) {
		m_tiptext = wxEmptyString;
	} else {
		if (column == m_status_column_index) // is bot?
		{
			m_tiptext = wxEmptyString;

			if (user.BattleStatus().IsBot()) {
				m_tiptext += _("AI (bot)\n");
			} else {
				m_tiptext += _("Human\n");
			}

			if (user.BattleStatus().spectator) {
				m_tiptext += _("Spectator\n");
			} else {
				m_tiptext += _("Player\n");
			}

			if (m_battle->IsFounder(user)) {
				m_tiptext += _("Host\n");
			} else {
				m_tiptext += _("Client\n");
			}

			if (user.BattleStatus().ready) {
				m_tiptext += _("Ready\n");
			} else {
				m_tiptext += _("Not ready\n");
			}

			if (user.BattleStatus().sync == SYNC_SYNCED) {
				m_tiptext += _("In sync");
			} else {
				m_tiptext += _("Not in sync");
			}
		} else if (column == m_faction_column_index) // icon
		{
			m_tiptext = wxEmptyString;
			if (user.BattleStatus().spectator) {
				m_tiptext = _T("Spectators have no side");
			} else {
				try {
					const auto sides = LSL::usync().GetSides(m_battle->GetHostModName());
					const int side = user.BattleStatus().side;
					if (side < (int)sides.size())
						m_tiptext = TowxString(sides[side]);
				} catch (...) {
				}
			}
		} else if (column == m_ingame_column_index) // lobby status
		{
			m_tiptext = _T( "This " );
			if (user.GetStatus().bot) {
				m_tiptext << _T( "bot " );
			} else if (user.GetStatus().moderator) {
				m_tiptext << _T( "moderator " );
			} else {
				m_tiptext << _T( "player " );
			}

			if (user.GetStatus().in_game) {
				m_tiptext << _T( "is ingame" );
			} else if (user.GetStatus().away) {
				m_tiptext << _T( "is away" );
			} else {
				m_tiptext << _T( "is available" );
			}
		} else if (column == m_country_column_index) // country
		{
			m_tiptext = user.BattleStatus().IsBot() ? _T("This bot is from nowhere particular") : GetFlagNameFromCountryCode(TowxString(user.GetCountry()));
		} else if (column == m_rank_column_index) // rank
		{
			m_tiptext = user.BattleStatus().IsBot() ? _T("This bot has no rank") : TowxString(user.GetRankName(user.GetStatus().rank));
		} else if (column == m_nick_column_index) //name
		{
			m_tiptext = TowxString(user.GetNick());
		}else if (column == m_trueskill_column_index) //TrueSkill
		{
			m_tiptext = _("Player's TrueSkill rank");
		}else
			m_tiptext = m_colinfovec[column].tip;
	}
}

void BattleroomListCtrl::OnUserMenuAddToGroup(wxCommandEvent& event)
{
	int id = event.GetId();
	wxString groupname = m_popup->GetGroupByEvtID(id);
	wxString nick = GetSelectedUserNick();
	useractions().AddUserToGroup(groupname, nick);
	Disconnect(GROUP_ID_NEW, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(BattleroomListCtrl::OnUserMenuCreateGroup), 0, this);
}

void BattleroomListCtrl::OnUserMenuDeleteFromGroup(wxCommandEvent& /*unused*/)
{
	wxString nick = GetSelectedUserNick();
	useractions().RemoveUser(nick);
}

void BattleroomListCtrl::OnUserMenuCreateGroup(wxCommandEvent& /*unused*/)
{
	wxString name;
	if (ui().AskText(_("Enter name"),
			 _("Please enter the name for the new group.\nAfter clicking ok you will be taken to adjust its settings."), name)) {
		wxString nick = GetSelectedUserNick();

		useractions().AddGroup(name);
		useractions().AddUserToGroup(name, nick);
		ui().mw().ShowConfigure(MainWindow::OPT_PAGE_GROUPS);
	}
}

wxString BattleroomListCtrl::GetSelectedUserNick()
{
	if (GetSelectedIndex() < 0)
		return wxEmptyString;
	else
		return TowxString(m_data[GetSelectedIndex()]->GetNick());
}

void BattleroomListCtrl::OnActivateItem(wxListEvent& /*unused*/)
{
	if (m_ro)
		return;
	if (GetSelectedIndex() < 0)
		return;

	const User* usr = m_data[GetSelectedIndex()];
	if (usr != NULL && !usr->BattleStatus().IsBot())
		ui().mw().OpenPrivateChat(*usr);
}

int BattleroomListCtrl::GetIndexFromData(const DataType& data) const
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
