/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

//
// Class: HostBattleDialog
//
#include "hostbattledialog.h"

#include <lslunitsync/optionswrapper.h>
#include <lslunitsync/springbundle.h>
#include <lslunitsync/unitsync.h>

#include <wx/log.h>
#include <wx/menu.h>

#include "gui/controls.h"
#include "gui/customdialogs.h"
#include "gui/ui.h"
#include "gui/uiutils.h"
#include "ibattle.h"
#include "images/arrow_refresh.png.h"
#include "images/rank0.xpm"
#include "images/rank1.xpm"
#include "images/rank2.xpm"
#include "images/rank3.xpm"
#include "images/rank4.xpm"
#include "images/rank5.xpm"
#include "images/rank6.xpm"
#include "images/rank7.xpm"
#include "iserver.h"
#include "serverselector.h"
#include "settings.h"
#include "user.h"
#include "utils/conversion.h"
#include "utils/lslconversion.h"
#include "utils/slpaths.h"

BEGIN_EVENT_TABLE(HostBattleDialog, wxDialog)
EVT_MENU(wxID_ANY, HostBattleDialog::OnRelayChoice)
END_EVENT_TABLE()

HostBattleDialog::HostBattleDialog(wxWindow* parent)
    : HostBattleDialogBase(parent)
    , WindowHintsPickle(_T("hostbattledialog"), this, wxSize(410, 520))
    , m_last_relayhost(sett().GetLastRelayedHost())
{
	m_battle_name_text->SetValue(sett().GetLastHostDescription());
	m_password_text->SetValue(sett().GetLastHostPassword());
	m_port_spinctrl->SetValue(sett().GetLastHostPort());
	m_relayhost_pick->SetLabel(m_last_relayhost.IsEmpty() ? _T("automatic") : m_last_relayhost);


	m_relayhost_list = new wxMenu();
	wxMenuItem* automatic_pick = new wxMenuItem(m_relayhost_list, AUTO_PICK_HOST, _("Chose automatically"), _("Randomly picks an available one"), wxITEM_RADIO);
	m_relayhost_list->Append(automatic_pick);
	automatic_pick->Check(m_last_relayhost.IsEmpty());
	m_relayhost_list->AppendSeparator();
	wxMenuItem* manual_pick_relay = new wxMenuItem(m_relayhost_list, MANUAL_PICK_HOST, _("Manually enter the manager name"), _("You'll get prompted for the exact manager name"), wxITEM_RADIO);
	m_relayhost_list->Append(manual_pick_relay);
	m_relayhost_list->AppendSeparator();
	m_relayhost_array_list = lslTowxArrayString(serverSelector().GetServer().GetRelayHostList());
	for (unsigned int i = 0; i < m_relayhost_array_list.GetCount(); i++) {
		wxMenuItem* newitem = new wxMenuItem(m_relayhost_list, MANUAL_PICK_HOST + 1 + i, m_relayhost_array_list[i], wxEmptyString, wxITEM_RADIO);
		m_relayhost_list->Append(newitem);
		newitem->Check(m_last_relayhost == m_relayhost_array_list[i]);
	}

	m_use_relayhost_check->SetValue(sett().GetLastHostRelayedMode());
	m_relayhost_pick->Show(m_use_relayhost_check->IsChecked());
	m_nat_traversal_radios->SetSelection(sett().GetLastHostNATSetting());


	m_rank_combo->Append(User::GetRankName(UserStatus::RANK_1), wxBitmap(rank0_xpm));
	m_rank_combo->Append(User::GetRankName(UserStatus::RANK_2), wxBitmap(rank1_xpm));
	m_rank_combo->Append(User::GetRankName(UserStatus::RANK_3), wxBitmap(rank2_xpm));
	m_rank_combo->Append(User::GetRankName(UserStatus::RANK_4), wxBitmap(rank3_xpm));
	m_rank_combo->Append(User::GetRankName(UserStatus::RANK_5), wxBitmap(rank4_xpm));
	m_rank_combo->Append(User::GetRankName(UserStatus::RANK_6), wxBitmap(rank5_xpm));
	m_rank_combo->Append(User::GetRankName(UserStatus::RANK_7), wxBitmap(rank6_xpm));
	m_rank_combo->Append(User::GetRankName(UserStatus::RANK_8), wxBitmap(rank7_xpm));
	int prevRankLimit = sett().GetLastRankLimit();
	if (prevRankLimit < 0)
		prevRankLimit = -(prevRankLimit + 1);
	m_rank_combo->SetSelection(prevRankLimit);


	this->SetSizer( m_main_sizer );
	this->Layout();
	m_main_sizer->Fit( this );

	m_host_button->SetFocus();

	ReloadEngineList();
}


void HostBattleDialog::ReloadGameList()
{
	m_game_choice->Clear();

	wxArrayString modlist = lslTowxArrayString(LSL::usync().GetGameList());
	wxString last = sett().GetLastHostMod();

	size_t nummods = modlist.Count();
	for (size_t i = 0; i < nummods; i++) {
		m_game_choice->Insert(modlist[i], i);
		if (last == modlist[i])
			m_game_choice->SetSelection(i);
	}

	if (m_game_choice->GetSelection() == wxNOT_FOUND) {
		m_game_choice->SetSelection(0);
	}
}


void HostBattleDialog::ReloadEngineList()
{
	m_engine_choice->Clear();
	std::map<std::string, LSL::SpringBundle> versions = SlPaths::GetSpringVersionList();
	const std::string last = SlPaths::GetCurrentUsedSpringIndex();
	int i = 0;
	for (auto pair : versions) {
		m_engine_choice->Insert(TowxString(pair.first), i);
		if (last == pair.first) {
			m_engine_choice->SetSelection(i);
		}
		i++;
	}

	if (m_engine_choice->GetSelection() == wxNOT_FOUND) {
		m_engine_choice->SetSelection(0);
	}
	//unitsync change needs a refresh of games as well
	ReloadGameList();
}


void HostBattleDialog::OnOk(wxCommandEvent& /*unused*/)
{
	if (m_game_choice->GetSelection() == wxNOT_FOUND) {
		wxLogWarning(_T( "no game selected" ));
		customMessageBox(SL_MAIN_ICON, _("You have to select a game first."), _("No game selected."), wxOK);
		return;
	}

	if (m_engine_choice->GetSelection() == wxNOT_FOUND) {
		wxLogWarning(_T( "no engine selected" ));
		customMessageBox(SL_MAIN_ICON, _("You have to select a engine version first."), _("No engine selected."), wxOK);
		return;
	}

	if (m_battle_name_text->GetValue().IsEmpty())
		m_battle_name_text->SetValue(_T( "(none)" ));
	sett().SetLastHostDescription(m_battle_name_text->GetValue());

	sett().SetLastHostMod(m_game_choice->GetString(m_game_choice->GetSelection()));
	wxString password = m_password_text->GetValue();
	password.Replace(_T(" "), wxEmptyString);
	sett().SetLastHostPassword(password);
	sett().SetLastHostPort(m_port_spinctrl->GetValue());
	sett().SetTestHostPort(false);
	sett().SetLastHostPlayerNum(m_max_players_slider->GetValue());
	sett().SetLastHostNATSetting(m_nat_traversal_radios->GetSelection());
	sett().SetLastRankLimit(GetSelectedRank());
	sett().SetLastHostRelayedMode(m_use_relayhost_check->GetValue());
	sett().SetLastRelayedHost(m_last_relayhost);
	sett().SaveSettings();
	EndModal(wxID_OK);
}


void HostBattleDialog::OnCancel(wxCommandEvent& /*unused*/)
{
	EndModal(wxID_CANCEL);
}


int HostBattleDialog::GetSelectedRank()
{
	int ret = m_rank_combo->GetSelection();
	if (wxNOT_FOUND == ret)
		ret = 0;
	if (m_rank_direction->GetSelection() > 0)
		ret = -ret - 1;
	return ret;
}

void HostBattleDialog::OnNatChange(wxCommandEvent& /*unused*/)
{
	//  m_port_test_check->Enable( m_nat_radios->GetSelection() == 0 );
	m_port_spinctrl->Enable(m_nat_traversal_radios->GetSelection() == 0);
}


void HostBattleDialog::OnPickRelayHost(wxCommandEvent&)
{
	PopupMenu(m_relayhost_list);
}

void HostBattleDialog::OnRelayChoice(wxCommandEvent& event)
{
	int index = event.GetId();
	if (index == AUTO_PICK_HOST)
		m_last_relayhost = wxEmptyString;
	else if (index == MANUAL_PICK_HOST) {
		ui().AskText(_("Manually chose a manager"), _("Please type the nick of the manager you want to use ( case sensitive )"), m_last_relayhost);
	} else if (!(index - MANUAL_PICK_HOST - 1 > int(m_relayhost_array_list.GetCount()))) {
		index = index - MANUAL_PICK_HOST - 1;
		m_last_relayhost = m_relayhost_array_list[index];
	}
	m_relayhost_pick->SetLabel(m_last_relayhost.IsEmpty() ? _T("automatic") : m_last_relayhost);
}

void HostBattleDialog::OnUseRelay(wxCommandEvent&)
{
	m_relayhost_pick->Show(m_use_relayhost_check->IsChecked());
	m_port_spinctrl->Enable(!m_use_relayhost_check->IsChecked() && m_nat_traversal_radios->GetSelection() == 0);
	m_nat_traversal_radios->Enable(!m_use_relayhost_check->IsChecked());
	Layout();
}

void HostBattleDialog::OnEngineSelect(wxCommandEvent& /*event*/)
{
	SlPaths::SetUsedSpringIndex(STD_STRING(m_engine_choice->GetString(m_engine_choice->GetSelection())));
	LSL::usync().ReloadUnitSyncLib();
	ReloadEngineList();
}

void HostBattleDialog::RunHostBattleDialog(wxWindow* parent)
{
	HostBattleDialog dlg(parent);
	if (dlg.ShowModal() != wxID_OK) {
		return;
	}

	BattleOptions bo;
	bo.description = STD_STRING(sett().GetLastHostDescription());
	bo.port = sett().GetLastHostPort();
	bo.userelayhost = sett().GetLastHostRelayedMode();
	if (bo.userelayhost) {
		bo.nattype = NAT_None;
	} else {
		bo.nattype = NatType(sett().GetLastHostNATSetting());
	}

	bo.rankneeded = sett().GetLastRankLimit();
	bo.maxplayers = sett().GetLastHostPlayerNum();
	bo.relayhost = STD_STRING(sett().GetLastRelayedHost());
	bo.engineName = "Spring";
	bo.engineVersion = LSL::usync().GetSpringVersion();

	// Get selected mod from unitsync.
	LSL::UnitsyncGame mod;
	try {
		mod = LSL::usync().GetGame(STD_STRING(sett().GetLastHostMod()));
		bo.gamehash = mod.hash;
		bo.gamename = mod.name;
	} catch (const std::exception& e) {
		wxLogWarning(_T("Exception: %s"), e.what());
		wxLogWarning(_T("Can't host: game not found. Exception: %s"), e.what());
		customMessageBoxModal(SL_MAIN_ICON, wxString::Format(
		  _("Battle not started beacuse the game you selected could not be found. Exception: %s"),
		  e.what()), _("Error starting battle."), wxOK);
		return;
	}

	LSL::UnitsyncMap map;
	const auto mname = STD_STRING(sett().GetLastHostMap());
	if (!mname.empty() && LSL::usync().MapExists(mname)) {
		map = LSL::usync().GetMap(mname);
	} else {
		std::vector<std::string> maps = LSL::usync().GetMapList();
		if (maps.size() <= 0) {
			wxLogWarning(_T("no maps found"));
			customMessageBoxModal(SL_MAIN_ICON, _(
			  "Could not find any maps in engine's search path. Have you downloaded any?"
			  " You can join any multiplayer room for automatic download or use search in the Downloads tab. "
			  " Note that this can also happen when you set the paths in preferences incorrectly."),
			  _("No maps found"), wxOK);
			return;
		}
		map = LSL::usync().GetMap(maps[0]);
	}

	bo.maphash = map.hash;
	bo.mapname = map.name;

	serverSelector().GetServer().HostBattle(bo, STD_STRING(sett().GetLastHostPassword()));
}
