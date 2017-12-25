/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include <wx/intl.h>
#include <wx/stattext.h>
#include <wx/statline.h>
#include <wx/textdlg.h>
#include <wx/combobox.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/checkbox.h>
#include <wx/log.h>
#include <stdexcept>
#if wxUSE_TOGGLEBTN
#include <wx/tglbtn.h>
#endif
#include <set>

#include "aui/auimanager.h"
#include "battlelisttab.h"
#include "battledataviewcttrl.h"
#include "gui/ui.h"
#include "gui/chatpanel.h"
#include "utils/conversion.h"
#include "gui/controls.h"
#include "gui/uiutils.h"
#include "gui/hosting/hostbattledialog.h"
#include "gui/hosting/mainjoinbattletab.h"
#include "iserver.h"
#include "serverselector.h"
#include "gui/mapctrl.h"
#include "gui/nickdataviewctrl.h"
#include "gui/mainwindow.h"
#include "battlelistfilter.h"
#include "iconimagelist.h"
#include "useractions.h"
#include "gui/customdialogs.h"
#include "utils/slconfig.h"
#include "utils/globalevents.h"
#include "log.h"
#include "servermanager.h"
#include "exception.h"

//const unsigned int BATTLELIST_COLUMNCOUNT = 10;

BEGIN_EVENT_TABLE(BattleListTab, wxPanel)
EVT_BUTTON(BattleListTab::BATTLE_JOIN, BattleListTab::OnJoin)
EVT_BUTTON(BattleListTab::BATTLE_HOST, BattleListTab::OnHost)
EVT_DATAVIEW_ITEM_ACTIVATED(BattleDataViewCtrl::BATTLELIST_DATAVIEW_ID, BattleListTab::OnListJoin)
EVT_DATAVIEW_SELECTION_CHANGED(BattleDataViewCtrl::BATTLELIST_DATAVIEW_ID, BattleListTab::OnSelect)
EVT_CHECKBOX(BattleListTab::BATTLE_LIST_FILTER_ACTIV, BattleListTab::OnFilterActiv)
#if wxUSE_TOGGLEBTN
EVT_TOGGLEBUTTON(BattleListTab::BATTLE_LIST_FILTER_BUTTON, BattleListTab::OnFilter)
EVT_TOGGLEBUTTON(BattleListTab::BATTLE_LIST_INFO_BUTTON, BattleListTab::OnInfoShow)
#else
EVT_CHECKBOX(BattleListTab::BATTLE_LIST_FILTER_BUTTON, BattleListTab::OnFilter)
EVT_CHECKBOX(BattleListTab::BATTLE_LIST_INFO_BUTTON, BattleListTab::OnOnInfoShow)
#endif
END_EVENT_TABLE()

SLCONFIG("/BattleFilter/Active", false, "determines if battle list filter is active");
SLCONFIG("/BattleListTab/ShowExtendedInfos", true, "determines if panels with battle infos is shown");


BattleListTab::BattleListTab(wxWindow* parent)
    : wxPanel(parent, -1)
    , m_sel_battle(0)
{
	GetAui().manager->AddPane(this, wxLEFT, _T( "battlelisttab" ));

	m_main_sizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* m_filter_sizer;
	m_filter_sizer = new wxBoxSizer(wxVERTICAL);


	m_battlelist_sizer = new wxBoxSizer(wxVERTICAL);

	m_battle_list = new BattleDataViewCtrl(_T("BattleDataViewCtrl_BattleList"), this);
//	m_battle_list->SetHighLightAction(UserActions::ActHighlight);
	m_battlelist_sizer->Add(m_battle_list, 1, wxEXPAND);

	m_main_sizer->Add(m_battlelist_sizer, 1, wxEXPAND);

	m_info_sizer = new wxBoxSizer(wxHORIZONTAL);

	m_minimap = new MapCtrl(this, 100, 0, true, false, false);
	m_info_sizer->Add(m_minimap, 0, wxALL, 5);

	m_data_sizer = new wxFlexGridSizer(4, 2, 0, 0);

	m_map_lbl = new wxStaticText(this, wxID_ANY, _("Map:"), wxDefaultPosition, wxDefaultSize, 0);
	m_data_sizer->Add(m_map_lbl, 0, wxALL, 5);

	m_map_text = new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_data_sizer->Add(m_map_text, 0, wxALL, 5);

	m_mod_lbl = new wxStaticText(this, wxID_ANY, _("Game:"), wxDefaultPosition, wxDefaultSize, 0);
	m_data_sizer->Add(m_mod_lbl, 0, wxALL, 5);

	m_mod_text = new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_data_sizer->Add(m_mod_text, 0, wxALL, 5);

	m_players_lbl = new wxStaticText(this, wxID_ANY, _("Players:"), wxDefaultPosition, wxDefaultSize, 0);
	m_data_sizer->Add(m_players_lbl, 0, wxALL, 5);

	m_players_text = new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_data_sizer->Add(m_players_text, 0, wxALL, 5);

	m_spec_lbl = new wxStaticText(this, wxID_ANY, _("Spectators:"), wxDefaultPosition, wxDefaultSize, 0);
	m_data_sizer->Add(m_spec_lbl, 0, wxALL, 5);

	m_spec_text = new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_data_sizer->Add(m_spec_text, 0, wxALL, 5);

	m_info_sizer->Add(m_data_sizer, 1, wxEXPAND, 5);

	m_players = new NickDataViewCtrl(_T("NickDataViewCtrl_BattleListTab"), this, false, 0, true);
	m_info_sizer->Add(m_players, 1, wxALL | wxEXPAND, 5);

	m_main_sizer->Add(m_info_sizer, 0, wxEXPAND, 5);


	m_filter = new BattleListFilter(this, wxID_ANY, this, wxDefaultPosition, wxSize(-1, -1), wxEXPAND);
	m_filter_sizer->Add(m_filter, 0, wxEXPAND, 5);

	m_main_sizer->Add(m_filter_sizer, 0, wxEXPAND, 5);

	m_buttons_sep = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
	m_main_sizer->Add(m_buttons_sep, 0, wxALL | wxEXPAND, 5);

	wxBoxSizer* m_buttons_sizer;
	m_buttons_sizer = new wxBoxSizer(wxHORIZONTAL);

	m_buttons_sizer->Add(0, 0, 1, wxEXPAND, 0);

#if wxUSE_TOGGLEBTN
	m_filter_show = new wxToggleButton(this, BATTLE_LIST_FILTER_BUTTON, _(" Filter "), wxDefaultPosition, wxSize(-1, 28), 0);
#else
	m_filter_show = new wxCheckBox(this, BATTLE_LIST_FILTER_BUTTON, _(" Filter "), wxDefaultPosition, wxSize(-1, 28), 0);
#endif
	m_buttons_sizer->Add(m_filter_show, 0, 0, 5);

	m_filter_activ = new wxCheckBox(this, BATTLE_LIST_FILTER_ACTIV, _("Activated"), wxDefaultPosition, wxDefaultSize, 0);
	m_buttons_sizer->Add(m_filter_activ, 0, wxALL, 5);

#if wxUSE_TOGGLEBTN
	m_info_show = new wxToggleButton(this, BATTLE_LIST_INFO_BUTTON, _(" Battle infos "), wxDefaultPosition, wxSize(-1, 28), 0);
#else
	m_info_show = new wxCheckBox(this, BATTLE_LIST_INFO_BUTTON, _(" Battle infos "), wxDefaultPosition, wxSize(-1, 28), 0);
#endif
	m_buttons_sizer->Add(m_info_show, 0, 0, 5);

	m_battle_num = new wxStaticText(this, wxID_ANY, _("0 battles displayed"), wxDefaultPosition, wxDefaultSize, 0);
	m_buttons_sizer->Add(m_battle_num, 0, wxALIGN_CENTER | wxLEFT | wxRIGHT, 4);
	m_buttons_sizer->Add(0, 0, 1, wxEXPAND, 0);

	m_host_btn = new wxButton(this, BATTLE_HOST, _("Host new..."), wxDefaultPosition, wxSize(-1, 28), 0);
	m_buttons_sizer->Add(m_host_btn, 0, wxBOTTOM | wxLEFT | wxRIGHT, 5);

	m_join_btn = new wxButton(this, BATTLE_JOIN, _("Join"), wxDefaultPosition, wxSize(-1, 28), 0);
	m_buttons_sizer->Add(m_join_btn, 0, wxBOTTOM | wxRIGHT, 5);

	m_main_sizer->Add(m_buttons_sizer, 0, wxEXPAND, 5);

	m_filter->Hide();

	SetSizer(m_main_sizer);
	Layout();

	SelectBattle(0);
	ShowExtendedInfos(cfg().ReadBool(_T("/BattleListTab/ShowExtendedInfos")));
	SUBSCRIBE_GLOBAL_EVENT(GlobalEventManager::OnUnitsyncReloaded, BattleListTab::OnUnitsyncReloaded);
}


BattleListTab::~BattleListTab()
{
	GlobalEventManager::Instance()->UnSubscribeAll(this);

	if (m_filter != 0)
		m_filter->SaveFilterValues();
}

void BattleListTab::OnConnected()
{
	bool filter = cfg().ReadBool(_T("/BattleFilter/Active"));
	SetFilterActiv(filter);
}

void BattleListTab::SetNumDisplayed()
{
	int num = m_battle_list->GetItemsCount();
	m_battle_num->SetLabel(wxString::Format(_("%d battles displayed"), num));
}

void BattleListTab::SelectBattle(IBattle* battle)
{
	m_sel_battle = battle;
	m_minimap->SetBattle(m_sel_battle);
	m_players->ClearUsers();
	if (m_sel_battle != 0) {
		m_map_text->SetLabel(TowxString(m_sel_battle->GetHostMapName()));
		m_mod_text->SetLabel(TowxString(m_sel_battle->GetHostGameName()));
		m_players_text->SetLabel(wxString::Format(_T( "%d / %d" ), int(m_sel_battle->GetNumUsers()) - int(m_sel_battle->GetSpectators()), int(m_sel_battle->GetMaxPlayers())));
		m_spec_text->SetLabel(wxString::Format(_T( "%d" ), m_sel_battle->GetSpectators()));
		m_players->SetUsers(m_sel_battle->GetUsers());
	} else {
		m_map_text->SetLabel(wxEmptyString);
		m_mod_text->SetLabel(wxEmptyString);
		m_players_text->SetLabel(_T( "0 / 0" ));
		m_spec_text->SetLabel(_T( "0" ));
	}
}

void BattleListTab::AddBattle(IBattle& battle)
{
	if (battle.GetGUIListActiv() || (m_filter->GetActiv() && !m_filter->FilterBattle(battle))) {
		return;
	}
	m_battle_list->AddBattle(battle);
	battle.SetGUIListActiv(true);
	SetNumDisplayed();
}

void BattleListTab::RemoveBattle(IBattle& battle)
{
	if (&battle == m_sel_battle) {
		SelectBattle(0);
	}

	m_battle_list->RemoveBattle(battle);

	battle.SetGUIListActiv(false);
	SetNumDisplayed();
}

void BattleListTab::UserUpdate(User& user)
{
	if (m_sel_battle && user.GetBattle() == m_sel_battle) {
		m_players->UserUpdated(user);
	}
}


void BattleListTab::UpdateBattle(IBattle& battle)
{
	if (!battle.GetGUIListActiv()) {
		AddBattle(battle);
	}
	if (m_filter->GetActiv() && !m_filter->FilterBattle(battle)) {
		RemoveBattle(battle);
		if (&battle == m_sel_battle) {
			m_sel_battle = NULL;
			SelectBattle(NULL);
		}
		return;
	}
	m_battle_list->UpdateBattle(battle);
	if (&battle == m_sel_battle)
		SelectBattle(m_sel_battle);
}

/**
 * Process "Disconnected From Server" global event
 */
void BattleListTab::OnDisconnected()
{
	SelectBattle(0);
	m_battle_list->Clear();
	SetNumDisplayed();
}


void BattleListTab::UpdateList()
{
	serverSelector().GetServer().battles_iter->IteratorBegin();
	while (!serverSelector().GetServer().battles_iter->EOL()) {
		IBattle* b = serverSelector().GetServer().battles_iter->GetBattle();
		if (b != 0)
			UpdateBattle(*b);
	}
	m_battle_list->Resort();
	m_battle_list->Refresh();
}


void BattleListTab::SetFilterActiv(bool activ)
{
	m_filter->SetActiv(activ);
	m_filter_activ->SetValue(activ);
	cfg().Write(_T( "/BattleFilter/Active" ), activ);
}

void BattleListTab::OnHost(wxCommandEvent& /*unused*/)
{
	if (!ServerManager::Instance()->IsConnected()) {
		wxLogWarning(_T( "Trying to host while offline" ));
		customMessageBoxModal(SL_MAIN_ICON, _("You cannot host a game while being offline. Please connect to a lobby server."), _("Not Online."), wxOK);
		ui().ShowConnectWindow();
		return;
	}

	IBattle* battle = ui().mw().GetJoinTab().GetCurrentBattle();
	if (battle != 0) {
		if (ui().Ask(_("Already in a battle"), _("You are already in a battle.\n\nDo you want to leave current battle to start a new?"))) {
			battle->Leave();
			ui().mw().GetJoinTab().LeaveCurrentBattle();
		} else {
			return;
		}
	}

	HostBattleDialog::RunHostBattleDialog(this);
}

/**
 * Procees "Show Filtes" toggle buttton clicked. Show/hides filters options panel
 * @param
 */
void BattleListTab::OnFilter(wxCommandEvent& /*unused*/)
{
	if (m_filter_show->GetValue()) {
		m_filter->Show();
		this->Layout();
	} else {
		m_filter->Hide();
		this->Layout();
	}
}

/**
 * Process "Turn On Filters" button event. Activates/deactivates battlelist filtering
 * @param
 */
void BattleListTab::OnFilterActiv(wxCommandEvent& /*unused*/)
{
	bool active = m_filter_activ->GetValue();
	if (!ServerManager::Instance()->IsConnected()) {
		m_filter_activ->SetValue(!active);
		return;
	}
	m_filter->SetActiv(active);
	cfg().Write(_T( "/BattleFilter/Active" ), active);
	SetNumDisplayed();
}

/**
 * Process "JOIN" button clicked. Join selected battle, if any
 * @param
 */
void BattleListTab::OnJoin(wxCommandEvent& /*unused*/)
{
	wxASSERT(m_battle_list != nullptr);

	//Is there any battle selected?
	const IBattle* battle = m_battle_list->GetSelectedItem();
	if (battle == nullptr) {
		return;
	}

	const int id = battle->GetBattleId();
	DoJoin(serverSelector().GetServer().battles_iter->GetBattle(id));
}

/**
 * Process double clicking on battle in battlelist. Join selected battle
 * @param event Selected battle
 */
void BattleListTab::OnListJoin(wxDataViewEvent& /*event*/)
{
	wxASSERT(m_battle_list != nullptr);

	IBattle* battle = m_battle_list->GetSelectedItem();
	if (battle == nullptr) {
		return;
	}

	SelectBattle(battle);

	int id = battle->GetBattleId();
	DoJoin(serverSelector().GetServer().battles_iter->GetBattle(id));
}

/**
 * Check conditions for joining battle and try to join
 * @param battle Battle been joined
 */
void BattleListTab::DoJoin(IBattle& battle)
{
	wxString password = wxEmptyString;

	IBattle* curbattle = ui().mw().GetJoinTab().GetCurrentBattle();

	//If trying to join battle player already joined
	if (curbattle != 0 && curbattle->GetID() == battle.GetID()) {
		ui().mw().ShowTab(MainWindow::PAGE_JOIN);
		return;
	}

	//If player already in another battle
	if (curbattle != 0) {
		if (ui().Ask(_("Already in another battle"), _("You are already in a battle.") + _("Do you want to leave your current battle and join this one?"))) {
			curbattle->Leave();
			ui().mw().GetJoinTab().LeaveCurrentBattle();
		} else {
			return;
		}
	}

	//Ask password from player if it is needed
	if (battle.IsPassworded()) {
		wxPasswordEntryDialog pw(this, _("Battle password"), _("Enter password (spaces will be stripped)"));
		pw.SetFocus();
		//Player did not gave password
		if (pw.ShowModal() != wxID_OK) {
			return;
		}
		password = pw.GetValue();
		password.Replace(_T(" "), wxEmptyString);
	}


	battle.Join(STD_STRING(password));
	//Change Tab to show user joining to battle is in progress
	ui().mw().ShowTab(MainWindow::PAGE_JOIN);
}

/**
 * Process "select" for battlelist. Selects currentbattle for info panel.
 * @param event Battle been selected
 */
void BattleListTab::OnSelect(wxDataViewEvent& /*event*/)
{
	IBattle* battle = m_battle_list->GetSelectedItem();
	if (battle == nullptr) {
		SelectBattle(nullptr);
		return;
	} else {
		SelectBattle(battle);
	}
}

/**
 * Global event handler. Fires when unitsync library is reloaded
 * @param
 */
void BattleListTab::OnUnitsyncReloaded(wxCommandEvent& /*data*/)
{
	ASSERT_LOGIC(wxThread::IsMain(), "wxThread::IsMain()");

	if (!serverSelector().IsServerAvailible())
		return;

	UpdateList();
}

void BattleListTab::UpdateHighlights()
{
	m_battle_list->Refresh();
}


void BattleListTab::SortBattleList()
{
	m_battle_list->Resort();
}

void BattleListTab::ShowExtendedInfos(bool show)
{
	const bool recursive = true;
	m_main_sizer->Show(m_info_sizer, show, recursive);
	m_main_sizer->Show(m_buttons_sep, show, recursive);
	m_info_show->SetValue(show);
	Layout();
	cfg().Write(_T("/BattleListTab/ShowExtendedInfos"), show);
}

void BattleListTab::OnInfoShow(wxCommandEvent& /*unused*/)
{
	ShowExtendedInfos(m_info_show->GetValue());
}
