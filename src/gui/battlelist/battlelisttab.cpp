/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include <set>
#include <stdexcept>
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/combobox.h>
#include <wx/intl.h>
#include <wx/log.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/textdlg.h>
#if wxUSE_TOGGLEBTN
#include <wx/tglbtn.h>
#define EVT_TOGGLEORCHECK EVT_TOGGLEBUTTON
#else
#define EVT_TOGGLEORCHECK EVT_CHECKBOX
#endif

#include "aui/auimanager.h"
#include "battledataviewctrl.h"
#include "battlelistfilter.h"
#include "battlelisttab.h"
#include "exception.h"
#include "gui/chatpanel.h"
#include "gui/controls.h"
#include "gui/customdialogs.h"
#include "gui/hosting/hostbattledialog.h"
#include "gui/hosting/mainjoinbattletab.h"
#include "gui/mainwindow.h"
#include "gui/mapctrl.h"
#include "gui/nickdataviewctrl.h"
#include "gui/ui.h"
#include "gui/uiutils.h"
#include "iconimagelist.h"
#include "iserver.h"
#include "log.h"
#include "servermanager.h"
#include "serverselector.h"
#include "useractions.h"
#include "utils/conversion.h"
#include "utils/globalevents.h"
#include "utils/slconfig.h"

//const unsigned int BATTLELIST_COLUMNCOUNT = 10;

BEGIN_EVENT_TABLE(BattleListTab, wxPanel)
EVT_BUTTON(BattleListTab::BATTLE_JOIN, BattleListTab::OnJoin)
EVT_BUTTON(BattleListTab::BATTLE_HOST, BattleListTab::OnHost)
EVT_DATAVIEW_ITEM_ACTIVATED(BattleDataViewCtrl::BATTLELIST_DATAVIEW_ID, BattleListTab::OnListJoin)
EVT_DATAVIEW_SELECTION_CHANGED(BattleDataViewCtrl::BATTLELIST_DATAVIEW_ID, BattleListTab::OnSelect)
EVT_CHECKBOX(BattleListTab::BATTLE_LIST_FILTER_ACTIV, BattleListTab::OnFilterActiv)
EVT_TOGGLEORCHECK(BattleListTab::BATTLE_LIST_FILTER_BUTTON, BattleListTab::OnFilter)
EVT_TOGGLEORCHECK(BattleListTab::BATTLE_LIST_INFO_BUTTON, BattleListTab::OnInfoShow)
END_EVENT_TABLE()

SLCONFIG("/BattleFilter/Active", false, "determines if battle list filter is active");
SLCONFIG("/BattleListTab/ShowExtendedInfos", true, "determines if panels with battle infos is shown");


BattleListTab::BattleListTab(wxWindow* parent)
    : wxPanel(parent, -1)
    , m_sel_battle(0)
{
	auto AddToSizer = [&](wxSizer* sizer, int height, wxWindow* window) {
		return sizer->Add (window, 0, wxALL, height);
	};

	GetAui().manager->AddPane(this, wxLEFT, _T( "battlelisttab" ));

	m_battle_list = new BattleDataViewCtrl(_T("BattleDataViewCtrl_BattleList2"), this);
//	m_battle_list->SetHighLightAction(UserActions::ActHighlight);


	m_players_lbl  = new wxStaticText(this, wxID_ANY, _("Players:"));
	m_players_text = new wxStaticText(this, wxID_ANY, wxEmptyString);
	m_map_lbl      = new wxStaticText(this, wxID_ANY, _("Map:"));
	m_map_text     = new wxStaticText(this, wxID_ANY, wxEmptyString);
	m_game_lbl     = new wxStaticText(this, wxID_ANY, _("Game:"));
	m_game_text    = new wxStaticText(this, wxID_ANY, wxEmptyString);
	m_engine_lbl   = new wxStaticText(this, wxID_ANY, _("Engine:"));
	m_engine_text  = new wxStaticText(this, wxID_ANY, wxEmptyString);
	m_host_lbl     = new wxStaticText(this, wxID_ANY, _("Host:"));
	m_host_text    = new wxStaticText(this, wxID_ANY, wxEmptyString);

	const int bib_height = 5; // Battle Info element border height
	m_data_sizer = new wxFlexGridSizer(5, 2, 0, 0);
	AddToSizer(m_data_sizer, bib_height, m_players_lbl);
	AddToSizer(m_data_sizer, bib_height, m_players_text);
	AddToSizer(m_data_sizer, bib_height, m_map_lbl);
	AddToSizer(m_data_sizer, bib_height, m_map_text);
	AddToSizer(m_data_sizer, bib_height, m_game_lbl);
	AddToSizer(m_data_sizer, bib_height, m_game_text);
	AddToSizer(m_data_sizer, bib_height, m_engine_lbl);
	AddToSizer(m_data_sizer, bib_height, m_engine_text);
	AddToSizer(m_data_sizer, bib_height, m_host_lbl);
	AddToSizer(m_data_sizer, bib_height, m_host_text);


	wxSize s = m_data_sizer->CalcMin();
	wxLogDebug (_T("Minimum size for data sizer: %d x %d"), s.GetHeight(), s.GetWidth() );
	m_minimap = new MapCtrl(this, s.GetHeight(), 0, true, false, false);
	m_players = new NickDataViewCtrl(_T("NickDataViewCtrl_BattleListTab"), this, false, 0, true);

	m_info_sizer = new wxBoxSizer(wxHORIZONTAL);
	m_info_sizer->Add(m_minimap, 0, wxALL, bib_height);
	m_info_sizer->Add(m_data_sizer, 1, wxEXPAND, bib_height);
	m_info_sizer->Add(m_players, 1, wxALL | wxEXPAND, bib_height);


	m_filter = new BattleListFilter(this, wxID_ANY, this, wxDefaultPosition, wxDefaultSize, wxEXPAND);
	m_filter->Hide();


	const wxSize br_size (-1, 28); // Button row size
	m_filter_activ = new wxCheckBox(this, BATTLE_LIST_FILTER_ACTIV, _("Activated"));
	m_filter_show = new wxToggleOrCheck(this, BATTLE_LIST_FILTER_BUTTON, _(" Filter "), wxDefaultPosition, br_size, 0);
	m_info_show = new wxToggleOrCheck(this, BATTLE_LIST_INFO_BUTTON, _(" Battle infos "), wxDefaultPosition, br_size, 0);
	m_battle_num = new wxStaticText(this, wxID_ANY, _("0 battles displayed"));
	m_host_btn = new wxButton(this, BATTLE_HOST, _("Host new..."), wxDefaultPosition, br_size, 0);
	m_join_btn = new wxButton(this, BATTLE_JOIN, _("Join"), wxDefaultPosition, br_size, 0);

	wxBoxSizer* m_buttons_sizer = new wxBoxSizer(wxHORIZONTAL);
	m_buttons_sizer->Add(0, 0, 1, wxEXPAND, 0);
	m_buttons_sizer->Add(m_filter_show, 0, 0, 5);
	m_buttons_sizer->Add(m_filter_activ, 0, wxALL, 5);
	m_buttons_sizer->Add(m_info_show, 0, 0, 5);
	m_buttons_sizer->Add(m_battle_num, 0, wxALIGN_CENTER | wxLEFT | wxRIGHT, 4);
	m_buttons_sizer->Add(0, 0, 1, wxEXPAND, 0);
	m_buttons_sizer->Add(m_host_btn, 0, wxBOTTOM | wxLEFT | wxRIGHT, 5);
	m_buttons_sizer->Add(m_join_btn, 0, wxBOTTOM | wxRIGHT, 5);


	m_buttons_sep = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);

	m_main_sizer = new wxBoxSizer(wxVERTICAL);
	m_main_sizer->Add(m_battle_list, 1, wxEXPAND);
	m_main_sizer->Add(m_info_sizer, 0, wxEXPAND, 5);
	m_main_sizer->Add(m_filter, 0, wxEXPAND, 5);
	m_main_sizer->Add(m_buttons_sep, 0, wxALL | wxEXPAND, 5);
	m_main_sizer->Add(m_buttons_sizer, 0, wxEXPAND, 5);
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
		m_engine_text->SetLabel(m_sel_battle->GetEngineName() + _T(" ") + m_sel_battle->GetEngineVersion());
		m_game_text->SetLabel(TowxString(m_sel_battle->GetHostGameName()));
		m_host_text->SetLabel(m_sel_battle->GetFounder().GetNick());
		m_map_text->SetLabel(TowxString(m_sel_battle->GetHostMapName()));
		m_players->SetUsers(m_sel_battle->GetUsers());
		m_players_text->SetLabel(wxString::Format(_T("%d + %d (Max: %d)"),
		  static_cast<int>(m_sel_battle->GetNumUsers()) - m_sel_battle->GetSpectators(),
		  m_sel_battle->GetSpectators(),
		  static_cast<int>(m_sel_battle->GetMaxPlayers()) ));
	} else {
		m_engine_text->SetLabel(wxEmptyString);
		m_game_text->SetLabel(wxEmptyString);
		m_host_text->SetLabel(wxEmptyString);
		m_map_text->SetLabel(wxEmptyString);
		m_players_text->SetLabel(wxEmptyString);
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

	//If player already in another battle
	if (curbattle != 0) {
		//If trying to join same battle again
		if (curbattle->GetID() == battle.GetID()) {
			ui().mw().ShowTab(MainWindow::PAGE_BATTLEROOM);
			return;
		}

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
	ui().mw().ShowTab(MainWindow::PAGE_BATTLEROOM);
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
