/* This file is part of the Springlobby (GPL v2 or later), see COPYING */
#include "battleroomdataviewctrl.h"

#include <wx/generic/numdlgg.h>

#include "battleroomdataviewmodel.h"
#include "gui/mainwindow.h"
#include "gui/ui.h"
#include "gui/uiutils.h"
#include "ibattle.h"
#include "iserver.h"
#include "lslunitsync/unitsync.h"
#include "user.h"
#include "utils/lslconversion.h"

BEGIN_EVENT_TABLE(BattleroomDataViewCtrl, BaseDataViewCtrl)
EVT_MENU(BATTLEROOM_VIEW_SPEC, BattleroomDataViewCtrl::OnSpecSelect)
EVT_MENU(BATTLEROOM_VIEW_KICK, BattleroomDataViewCtrl::OnKickPlayer)
EVT_DATAVIEW_ITEM_CONTEXT_MENU(BATTLEROOM_VIEW_ID, BattleroomDataViewCtrl::OnContextMenuEvent)
EVT_DATAVIEW_ITEM_ACTIVATED(BATTLEROOM_VIEW_ID, BattleroomDataViewCtrl::OnItemActivatedEvent)
EVT_MENU(BATTLEROOM_VIEW_RING, BattleroomDataViewCtrl::OnRingPlayer)
EVT_MENU(BATTLEROOM_VIEW_COLOUR, BattleroomDataViewCtrl::OnColourSelect)
EVT_MENU(BATTLEROOM_VIEW_HANDICAP, BattleroomDataViewCtrl::OnHandicapSelect)
END_EVENT_TABLE()

BattleroomDataViewCtrl::BattleroomDataViewCtrl(const wxString& dataViewName, wxWindow* parent, IBattle* /*battle*/, bool readOnly, bool showInGame)
    : BaseDataViewCtrl(dataViewName, parent, BATTLEROOM_VIEW_ID)
{

	m_ViewIsReadOnly = readOnly;
	m_ShowInGame = showInGame;
	m_Battle = nullptr;
	m_sides = nullptr;

	BattleroomDataViewModel* model = new BattleroomDataViewModel();
	AssociateModel(model);

	const int DEFAULT_SIZE = wxCOL_WIDTH_AUTOSIZE;
	AppendBitmapColumn(_("Status"), STATUS, wxDATAVIEW_CELL_INERT, DEFAULT_SIZE, wxALIGN_CENTER, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendBitmapColumn(_("Ingame"), INGAME, wxDATAVIEW_CELL_INERT, DEFAULT_SIZE, wxALIGN_CENTER, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendBitmapColumn(_("Faction"), FACTION, wxDATAVIEW_CELL_INERT, DEFAULT_SIZE, wxALIGN_CENTER, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendBitmapColumn(_("Colour"), COLOUR, wxDATAVIEW_CELL_INERT, DEFAULT_SIZE, wxALIGN_CENTER, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendBitmapColumn(_("Country"), COUNTRY, wxDATAVIEW_CELL_INERT, DEFAULT_SIZE, wxALIGN_CENTER, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendBitmapColumn(_("Rank"), RANK, wxDATAVIEW_CELL_INERT, DEFAULT_SIZE, wxALIGN_CENTER, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn(_("Nickname"), NICKNAME, wxDATAVIEW_CELL_INERT, DEFAULT_SIZE, wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn(_("TrueSkill"), TRUESKILL, wxDATAVIEW_CELL_INERT, DEFAULT_SIZE, wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn(_("Team"), TEAM, wxDATAVIEW_CELL_INERT, DEFAULT_SIZE, wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn(_("Ally"), ALLY, wxDATAVIEW_CELL_INERT, DEFAULT_SIZE, wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn(_("Resource Bonus"), BONUS, wxDATAVIEW_CELL_INERT, DEFAULT_SIZE, wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);

	//Hide "ingame" column if not needed
	if (!showInGame) {
		GetColumn(INGAME)->SetHidden(true);
	}
	//ReadOnly means quick info about battle
	if (!readOnly) {
		CreateContextMenu();
	}

	LoadColumnProperties();
}

BattleroomDataViewCtrl::~BattleroomDataViewCtrl()
{
	//TODO: implement
	//	wxDELETE(m_PopupMenu);
	m_Battle = nullptr;

	SaveColumnProperties();
}

void BattleroomDataViewCtrl::SetBattle(IBattle* battle)
{
	//TODO: implement
	//Remove all users from view (if any)
	Clear();

	m_Battle = battle;
	//Inform model about new battle
	static_cast<BattleroomDataViewModel*>(GetModel())->SetBattle(battle);

	UpdateContextMenuSides();
}

void BattleroomDataViewCtrl::AddUser(User& user)
{
	//TODO: implement
	if (ContainsItem(user)) {
		return;
	}
	AddItem(user);
}

void BattleroomDataViewCtrl::RemoveUser(User& user)
{
	//TODO: implement
	if (!ContainsItem(user)) {
		return;
	}
	RemoveItem(user);
}

void BattleroomDataViewCtrl::SetTipWindowText(const long /*item_hit*/,
					      const wxPoint& /*position*/)
{
	//TODO: implement
}

void BattleroomDataViewCtrl::UpdateUser(User& user)
{
	RefreshItem(user);
}

void BattleroomDataViewCtrl::OnItemActivatedEvent(wxDataViewEvent& /*event*/)
{
	if (m_ViewIsReadOnly) {
		return;
	}

	User* user = GetSelectedItem();

	if (user == nullptr || user->IsBot()) {
		return;
	}

	ui().mw().OpenPrivateChat(*user);
}

void BattleroomDataViewCtrl::OnContextMenuEvent(wxDataViewEvent& /*event*/)
{
	if (m_ViewIsReadOnly) {
		return;
	}

	const User* user = GetSelectedItem();

	if (user == nullptr) {
		return;
	}

	if (user->IsBot()) {
		int item = m_contextMenu->FindItem(_("Spectator"));
		m_contextMenu->Enable(item, false);
		m_contextMenu->Check(item, false);
		m_contextMenu->Enable(m_contextMenu->FindItem(_("Ring")), false);
		m_contextMenu->Enable(m_contextMenu->FindItem(_("Kick")), true);
	} else {
		int item = m_contextMenu->FindItem(_("Spectator"));
		m_contextMenu->Check(item, user->BattleStatus().spectator);
		m_contextMenu->Enable(item, true);
		m_contextMenu->Enable(m_contextMenu->FindItem(_("Ring")), true);
		bool isSelUserMe = (ui().IsThisMe(user));
		m_contextMenu->Enable(m_contextMenu->FindItem(_("Kick")), !isSelUserMe);
	}

	m_contextMenu->EnableItems(!user->IsBot(), wxString(user->GetNick())); //this updates groups, therefore we need to update the connection to evt handlers too
	std::vector<int> groups_ids = m_contextMenu->GetGroupIds();
	for (std::vector<int>::const_iterator it = groups_ids.begin(); it != groups_ids.end(); ++it) {
		Connect(*it, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(BattleroomDataViewCtrl::OnUserMenuAddToGroup), 0, this);
	}
	Connect(GROUP_ID_NEW, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(BattleroomDataViewCtrl::OnUserMenuCreateGroup), 0, this);
	PopupMenu(m_contextMenu);
}

void BattleroomDataViewCtrl::CreateContextMenu()
{

	m_contextMenu = new SL_GENERIC::UserMenu<BattleroomDataViewCtrl>(nullptr, this);

	wxMenu* m_teams = new wxMenu();
	for (unsigned int i = 0; i < SPRING_MAX_TEAMS; i++) {
		wxMenuItem* team = new wxMenuItem(m_teams, BATTLEROOM_VIEW_TEAM + i, wxString::Format(_T("%d"), i + 1), wxEmptyString, wxITEM_NORMAL);
		m_teams->Append(team);
		Connect(BATTLEROOM_VIEW_TEAM + i, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(BattleroomDataViewCtrl::OnTeamSelectEvent));
	}
	m_contextMenu->Append(-1, _("Team"), m_teams);

	wxMenu* m_allies = new wxMenu();
	for (unsigned int i = 0; i < SPRING_MAX_ALLIES; i++) {
		wxMenuItem* ally = new wxMenuItem(m_allies, BATTLEROOM_VIEW_ALLY + i, wxString::Format(_T("%d"), i + 1), wxEmptyString, wxITEM_NORMAL);
		m_allies->Append(ally);
		Connect(BATTLEROOM_VIEW_ALLY + i, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(BattleroomDataViewCtrl::OnAllySelectEvent));
	}
	m_contextMenu->Append(-1, _("Ally"), m_allies);

	m_sides = new wxMenu();
	m_contextMenu->Append(-1, _("Side"), m_sides);

	m_contextMenu->AppendSeparator();

	wxMenuItem* m_colours = new wxMenuItem(m_contextMenu, BATTLEROOM_VIEW_COLOUR, _("Set color"), wxEmptyString, wxITEM_NORMAL);
	m_contextMenu->Append(m_colours);

	m_contextMenu->AppendSeparator();

	m_handicap_item = new wxMenuItem(m_contextMenu, BATTLEROOM_VIEW_HANDICAP, _("Set Resource Bonus"), wxEmptyString, wxITEM_NORMAL);
	m_contextMenu->Append(m_handicap_item);

	m_contextMenu->AppendSeparator();

	m_spec_item = new wxMenuItem(m_contextMenu, BATTLEROOM_VIEW_SPEC, wxString(_("Spectator")), wxEmptyString, wxITEM_CHECK);
	m_contextMenu->Append(m_spec_item);

	m_contextMenu->AppendSeparator();

	wxMenuItem* kick = new wxMenuItem(m_contextMenu, BATTLEROOM_VIEW_KICK, wxString(_("Kick")), wxEmptyString, wxITEM_NORMAL);
	m_contextMenu->Append(kick);
	wxMenuItem* ring = new wxMenuItem(m_contextMenu, BATTLEROOM_VIEW_RING, wxString(_("Ring")), wxEmptyString, wxITEM_NORMAL);
	m_contextMenu->Append(ring);
}

void BattleroomDataViewCtrl::OnAllySelectEvent(wxCommandEvent& event)
{
	int ally = event.GetId() - BATTLEROOM_VIEW_ALLY;

	const User* user = GetSelectedItem();
	if (user != nullptr && !user->IsBridged()) {
		m_Battle->ForceAlly(*const_cast<User*>(user), ally);
	}
}

void BattleroomDataViewCtrl::OnTeamSelectEvent(wxCommandEvent& event)
{
	int team = event.GetId() - BATTLEROOM_VIEW_TEAM;

	const User* user = GetSelectedItem();

	if (user != nullptr  && !user->IsBridged()) {
		m_Battle->ForceTeam(*const_cast<User*>(user), team);
	}
}

void BattleroomDataViewCtrl::OnUserMenuDeleteFromGroup(
    wxCommandEvent& /*event*/)
{
	const User* user = GetSelectedItem();

	if (user == nullptr) {
		return;
	}

	wxString nick = wxString(user->GetNick());

	useractions().RemoveUser(nick);
}

void BattleroomDataViewCtrl::OnUserMenuCreateGroup(wxCommandEvent& /*event*/)
{
	const User* user = GetSelectedItem();

	if (user == nullptr) {
		return;
	}

	wxString name;
	if (ui().AskText(_("Enter name"),
			 _("Please enter the name for the new group.\nAfter clicking ok you will be taken to adjust its settings."), name)) {
		wxString nick = wxString(user->GetNick());

		useractions().AddGroup(name);
		useractions().AddUserToGroup(name, nick);
		ui().mw().ShowConfigure(MainWindow::OPT_PAGE_GROUPS);
	}
}

void BattleroomDataViewCtrl::OnUserMenuAddToGroup(wxCommandEvent& event)
{
	const User* user = GetSelectedItem();

	if (user == nullptr) {
		return;
	}

	int id = event.GetId();
	wxString groupname = m_contextMenu->GetGroupByEvtID(id);

	wxString nick = wxString(user->GetNick());

	useractions().AddUserToGroup(groupname, nick);

	Disconnect(GROUP_ID_NEW, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(BattleroomDataViewCtrl::OnUserMenuCreateGroup), 0, this);
}

void BattleroomDataViewCtrl::OnColourSelect(wxCommandEvent& /*event*/)
{
	const User* user = GetSelectedItem();

	if (user == nullptr || user->IsBridged()) {
		return;
	}

	wxColour CurrentColour = lslTowxColour(user->BattleStatus().colour);
	CurrentColour = GetColourFromUser(this, CurrentColour);

	if (!CurrentColour.IsOk()) {
		return;
	}

	m_Battle->ForceColour(*const_cast<User*>(user), wxColourTolsl(CurrentColour));
}

void BattleroomDataViewCtrl::OnSideSelect(wxCommandEvent& event)
{
	int side = event.GetId() - BATTLEROOM_VIEW_SIDE;

	const User* user = GetSelectedItem();

	if (user != nullptr  && !user->IsBridged()) {
		m_Battle->ForceSide(*const_cast<User*>(user), side);
	}
}

void BattleroomDataViewCtrl::OnHandicapSelect(wxCommandEvent& /*event*/)
{
	const User* user = GetSelectedItem();

	if (user == nullptr || user->IsBridged()) {
		return;
	}

	long handicap = wxGetNumberFromUser(_("Please enter a value between 0 and 100"), _("Set Resource Bonus"), wxEmptyString, user->BattleStatus().handicap, 0, 100, (wxWindow*)&ui().mw(), wxDefaultPosition);
	if (handicap != -1) {
		m_Battle->SetHandicap(*const_cast<User*>(user), handicap);
	}
}

void BattleroomDataViewCtrl::OnSpecSelect(wxCommandEvent& /*event*/)
{
	const User* user = GetSelectedItem();

	if (user != nullptr && !user->IsBridged()) {
		m_Battle->ForceSpectator(*const_cast<User*>(user), m_spec_item->IsChecked());
	}
}

void BattleroomDataViewCtrl::OnKickPlayer(wxCommandEvent& /*event*/)
{
	const User* user = GetSelectedItem();

	if (user != nullptr && !user->IsBridged()) {
		m_Battle->KickPlayer(*const_cast<User*>(user));
	}
}

void BattleroomDataViewCtrl::OnRingPlayer(wxCommandEvent& /*event*/)
{
	const User* user = GetSelectedItem();

	if (user != nullptr && !user->IsBridged()) {
		m_Battle->GetServer().Ring(user->GetNick());
	}
}

void BattleroomDataViewCtrl::UpdateContextMenuSides()
{
	if (m_sides == nullptr) {
		return;
	}

	for (unsigned int i = 0; i < side_vector.size(); i++) {
		m_sides->Destroy(side_vector[i]); // delete side;
		Disconnect(BATTLEROOM_VIEW_SIDE + i, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(BattleroomDataViewCtrl::OnSideSelect));
	}

	side_vector.clear();

	if (m_Battle == nullptr) {
		return;
	}

	try {
		const wxArrayString sides = lslTowxArrayString(LSL::usync().GetSides(m_Battle->GetHostGameName()));
		for (unsigned int i = 0; i < sides.GetCount(); i++) {
			wxMenuItem* side = new wxMenuItem(m_sides, BATTLEROOM_VIEW_SIDE + i, sides[i], wxEmptyString, wxITEM_NORMAL);
			m_sides->Append(side);
			side_vector.push_back(side);
			Connect(BATTLEROOM_VIEW_SIDE + i, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(BattleroomDataViewCtrl::OnSideSelect));
		}
	} catch (...) {
	}
}
