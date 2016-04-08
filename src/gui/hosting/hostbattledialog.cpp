/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

//
// Class: HostBattleDialog
//

#include "hostbattledialog.h"

#include <wx/intl.h>
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/choice.h>
#include <wx/slider.h>
#include <wx/statline.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/statline.h>
#include <wx/statbox.h>
#include <wx/radiobox.h>
#include <wx/radiobut.h>
#include <wx/textctrl.h>
#include <wx/statbmp.h>
#include <wx/checkbox.h>
#include <wx/log.h>
#include <wx/menu.h>
#include <wx/image.h>
#include <wx/bmpbuttn.h>
#include <wx/scrolwin.h>


#include "settings.h"
#include "user.h"
#include "gui/uiutils.h"
#include "gui/controls.h"
#include "gui/customdialogs.h"
#include "utils/slpaths.h"
#include "utils/conversion.h"
#include "utils/lslconversion.h"
#include "gui/ui.h"
#include "iserver.h"
#include "ibattle.h"
#include "serverselector.h"
#include <lslunitsync/springbundle.h>
#include <lslunitsync/unitsync.h>
#include <lslunitsync/optionswrapper.h>

#include "images/rank0.xpm"
#include "images/rank1.xpm"
#include "images/rank2.xpm"
#include "images/rank3.xpm"
#include "images/rank4.xpm"
#include "images/rank5.xpm"
#include "images/rank6.xpm"
#include "images/rank7.xpm"
#include "images/arrow_refresh.png.h"

BEGIN_EVENT_TABLE(HostBattleDialog, wxDialog)
EVT_BUTTON(HOST_CANCEL, HostBattleDialog::OnCancel)
EVT_BUTTON(HOST_OK, HostBattleDialog::OnOk)
EVT_BUTTON(BTN_REFRESH, HostBattleDialog::OnReloadMods)
EVT_BUTTON(PICK_RELAYHOST, HostBattleDialog::OnPickRelayHost)
EVT_MENU(wxID_ANY, HostBattleDialog::OnRelayChoice)
EVT_RADIOBOX(CHOSE_NAT, HostBattleDialog::OnNatChange)
EVT_CHECKBOX(CHK_USE_RELAY, HostBattleDialog::OnUseRelay)
EVT_CHOICE(CHOOSE_ENGINE, HostBattleDialog::OnEngineSelect)
END_EVENT_TABLE()

HostBattleDialog::HostBattleDialog(wxWindow* parent)
    : wxDialog(parent, -1, _("Host new battle"), wxDefaultPosition, wxSize(420, 520), wxRESIZE_BORDER | wxDEFAULT_DIALOG_STYLE)
    , WindowHintsPickle(_T("hostbattledialog"), this, wxSize(410, 520))
    , m_last_relayhost(sett().GetLastRelayedHost())
{
	SetMinSize(wxSize(400, 560));

	m_panel = new wxPanel(this);
	wxBoxSizer* all_sizer = new wxBoxSizer(wxVERTICAL);

	wxFlexGridSizer* topsizer = new wxFlexGridSizer(2, 0, 10);
	wxBoxSizer* m_main_sizer;
	m_main_sizer = new wxBoxSizer(wxVERTICAL);

	m_desc_lbl = new wxStaticText(m_panel, wxID_ANY, _("Description"), wxDefaultPosition, wxDefaultSize, 0);
	m_desc_lbl->Wrap(-1);
	topsizer->Add(m_desc_lbl, 1, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	m_desc_text = new wxTextCtrl(m_panel, wxID_ANY, sett().GetLastHostDescription(), wxDefaultPosition, wxDefaultSize, 0);
	m_desc_text->SetToolTip(_("A short description of the game, this will show up in the battle list."));
	topsizer->Add(m_desc_text, 1, wxALL | wxEXPAND, 5);

	m_desc_check = new wxCheckBox(m_panel, wxID_ANY, _("Autopaste description"));
	m_desc_check->SetValue(sett().GetBattleLastAutoAnnounceDescription());
	m_desc_check->SetToolTip(_("Automatically write the battle description when a user joins."));

	topsizer->AddStretchSpacer();
	topsizer->Add(m_desc_check, 0, wxLEFT, 5);

	m_mod_lbl = new wxStaticText(m_panel, wxID_ANY, _("Engine"), wxDefaultPosition, wxDefaultSize, 0);
	m_mod_lbl->Wrap(-1);
	topsizer->Add(m_mod_lbl, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	wxArrayString m_engine_picChoices;
	wxBoxSizer* mod_choice_button_sizer2 = new wxBoxSizer(wxHORIZONTAL);
	m_engine_pic = new wxChoice(m_panel, CHOOSE_ENGINE, wxDefaultPosition, wxDefaultSize, m_engine_picChoices, 0);
	m_engine_pic->SetToolTip(_("Select the engine version to play."));
	mod_choice_button_sizer2->Add(m_engine_pic, 0, wxALL, 5);
	topsizer->Add(mod_choice_button_sizer2, 0, wxEXPAND | wxALL, 1);

	m_mod_lbl = new wxStaticText(m_panel, wxID_ANY, _("Game"), wxDefaultPosition, wxDefaultSize, 0);
	m_mod_lbl->Wrap(-1);
	topsizer->Add(m_mod_lbl, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	wxArrayString m_mod_picChoices;
	wxBoxSizer* mod_choice_button_sizer = new wxBoxSizer(wxHORIZONTAL);
	m_mod_pic = new wxChoice(m_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_mod_picChoices, 0);
	m_mod_pic->SetToolTip(_("Select the game to play."));
	mod_choice_button_sizer->Add(m_mod_pic, 0, wxALL, 5);

	wxBitmap mp = charArr2wxBitmap(arrow_refresh_png, sizeof(arrow_refresh_png));
	m_refresh_btn = new wxBitmapButton(m_panel, BTN_REFRESH, mp);
	mod_choice_button_sizer->Add(m_refresh_btn, 0, wxEXPAND | wxTOP | wxBOTTOM, 5);

	topsizer->Add(mod_choice_button_sizer, 0, wxEXPAND | wxALL, 1);

	m_pwd_lbl = new wxStaticText(m_panel, wxID_ANY, _("Password\n(no spaces)"), wxDefaultPosition, wxDefaultSize, 0);
	m_pwd_lbl->Wrap(-1);
	topsizer->Add(m_pwd_lbl, 1, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	m_pwd_text = new wxTextCtrl(m_panel, wxID_ANY, sett().GetLastHostPassword(), wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);
	m_pwd_text->SetToolTip(_("Password needed to join game. Keep empty for no password"));
	topsizer->Add(m_pwd_text, 1, wxALL | wxEXPAND, 5);

	m_port_lbl = new wxStaticText(m_panel, wxID_ANY, _("Port"), wxDefaultPosition, wxDefaultSize, 0);
	m_port_lbl->Wrap(-1);
	topsizer->Add(m_port_lbl, 1, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	m_port_text = new wxTextCtrl(m_panel, wxID_ANY, wxString::Format(_T( "%d" ), sett().GetLastHostPort()), wxDefaultPosition, wxDefaultSize, 0);
	m_port_text->SetToolTip(_("UDP port to host game on. Default is 8452."));
	topsizer->Add(m_port_text, 1, wxALL | wxEXPAND, 5);

	//	m_port_test_check = new wxCheckBox( m_panel, wxID_ANY, _("Test firewall"), wxDefaultPosition, wxDefaultSize, 0 );
	//	m_port_test_check->SetValue( sett().GetTestHostPort() );
	//	m_port_sizer->Add( m_port_test_check, 1, wxALL|wxEXPAND, 5 );


	m_relayed_host_check = new wxCheckBox(m_panel, CHK_USE_RELAY, _("Use relayhost"), wxDefaultPosition, wxDefaultSize, 0);
	m_relayed_host_check->SetToolTip(_("host and control game on remote server, helps if you have trouble hosting"));
	m_relayed_host_pick = new wxButton(m_panel, PICK_RELAYHOST, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_relayed_host_pick->SetLabel(m_last_relayhost.IsEmpty() ? _T("automatic") : m_last_relayhost);

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

	m_relayed_host_check->SetValue(sett().GetLastHostRelayedMode());
	m_relayed_host_pick->Show(m_relayed_host_check->IsChecked());

	topsizer->Add(m_relayed_host_check, 1, wxALL | wxEXPAND, 5);
	topsizer->Add(m_relayed_host_pick, 0, wxALL | wxEXPAND, 5);

	m_main_sizer->Add(topsizer, 0, wxEXPAND, 0);

	wxStaticBoxSizer* m_players_box;
	m_players_box = new wxStaticBoxSizer(new wxStaticBox(m_panel, -1, _("Number of players")), wxVERTICAL);

	m_players_box->SetMinSize(wxSize(-1, 60));
	m_players_slide = new wxSlider(m_panel, wxID_ANY, sett().GetLastHostPlayerNum(), 2, SPRING_MAX_USERS, wxDefaultPosition, wxDefaultSize, wxSL_AUTOTICKS | wxSL_BOTH | wxSL_HORIZONTAL | wxSL_LABELS);
	m_players_slide->SetToolTip(_("The maximum number of players to allow in the battle."));
	m_players_box->Add(m_players_slide, 0, wxALL | wxEXPAND, 5);

	m_main_sizer->Add(m_players_box, 0, wxALL | wxEXPAND, 5);

	wxBoxSizer* m_pl_nat_sizer;
	m_pl_nat_sizer = new wxBoxSizer(wxHORIZONTAL);

	wxString m_nat_radiosChoices[] = {
	    _("None"), _("Hole punching") /*, _("Fixed source ports")*/
	};
	int m_nat_radiosNChoices = sizeof(m_nat_radiosChoices) / sizeof(wxString);
	m_nat_radios = new wxRadioBox(m_panel, CHOSE_NAT, _("NAT traversal"), wxDefaultPosition, wxDefaultSize, m_nat_radiosNChoices, m_nat_radiosChoices, 1, wxRA_SPECIFY_COLS);
	m_nat_radios->SetSelection(sett().GetLastHostNATSetting());

	m_nat_radios->Enable(true);

	m_nat_radios->SetToolTip(_("NAT traversal to use."));

	m_pl_nat_sizer->Add(m_nat_radios, 1, wxALL | wxEXPAND, 5);

	wxStaticBoxSizer* m_rank_box;
	m_rank_box = new wxStaticBoxSizer(new wxStaticBox(m_panel, -1, _("Rank requirement")), wxVERTICAL);
	wxArrayString rankFilterChoices;
	rankFilterChoices.Add(_("At least"));
	rankFilterChoices.Add(_("No greater than"));
	m_rank_direction = new wxChoice(m_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, rankFilterChoices, 0);
	m_rank_direction->SetToolTip(_("Select the type of rank enforcement."));
	m_rank_box->Add(m_rank_direction);

	wxFlexGridSizer* m_rank_sizer;
	m_rank_sizer = new wxFlexGridSizer(2, 0, 0, 0);
	m_rank_sizer->SetFlexibleDirection(wxBOTH);

	m_rank0_radio = new wxRadioButton(m_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	m_rank_sizer->Add(m_rank0_radio, 0, wxALL, 5);

	m_rank0_img = new wxStaticBitmap(m_panel, wxID_ANY, wxBitmap(rank0_xpm), wxDefaultPosition, wxSize(16, 16), 0);
	m_rank_sizer->Add(m_rank0_img, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL, 5);

	m_rank1_radio = new wxRadioButton(m_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_rank_sizer->Add(m_rank1_radio, 0, wxALL, 5);

	m_rank1_img = new wxStaticBitmap(m_panel, wxID_ANY, wxBitmap(rank1_xpm), wxDefaultPosition, wxSize(16, 16), 0);
	m_rank_sizer->Add(m_rank1_img, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL | wxALL, 2);

	m_rank2_radio = new wxRadioButton(m_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_rank_sizer->Add(m_rank2_radio, 0, wxALL, 5);

	m_rank2_img = new wxStaticBitmap(m_panel, wxID_ANY, wxBitmap(rank2_xpm), wxDefaultPosition, wxSize(16, 16), 0);
	m_rank_sizer->Add(m_rank2_img, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL, 2);

	m_rank3_radio = new wxRadioButton(m_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_rank_sizer->Add(m_rank3_radio, 0, wxALL, 5);

	m_rank3_img = new wxStaticBitmap(m_panel, wxID_ANY, wxBitmap(rank3_xpm), wxDefaultPosition, wxSize(16, 16), 0);
	m_rank_sizer->Add(m_rank3_img, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL, 2);

	m_rank4_radio = new wxRadioButton(m_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_rank_sizer->Add(m_rank4_radio, 0, wxALL, 5);

	m_rank4_img = new wxStaticBitmap(m_panel, wxID_ANY, wxBitmap(rank4_xpm), wxDefaultPosition, wxSize(16, 16), 0);
	m_rank_sizer->Add(m_rank4_img, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL, 2);

	m_rank5_radio = new wxRadioButton(m_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_rank_sizer->Add(m_rank5_radio, 0, wxALL, 5);

	m_rank5_img = new wxStaticBitmap(m_panel, wxID_ANY, wxBitmap(rank5_xpm), wxDefaultPosition, wxSize(16, 16), 0);
	m_rank_sizer->Add(m_rank5_img, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL, 5);

	m_rank6_radio = new wxRadioButton(m_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_rank_sizer->Add(m_rank6_radio, 0, wxALL, 5);

	m_rank6_img = new wxStaticBitmap(m_panel, wxID_ANY, wxBitmap(rank6_xpm), wxDefaultPosition, wxSize(16, 16), 0);
	m_rank_sizer->Add(m_rank6_img, 0, wxALL, 5);

	m_rank7_radio = new wxRadioButton(m_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_rank_sizer->Add(m_rank7_radio, 0, wxALL, 5);

	m_rank7_img = new wxStaticBitmap(m_panel, wxID_ANY, wxBitmap(rank7_xpm), wxDefaultPosition, wxSize(16, 16), 0);
	m_rank_sizer->Add(m_rank7_img, 0, wxALL, 5);

	m_rank_box->Add(m_rank_sizer, 1, wxEXPAND, 5);

	m_pl_nat_sizer->Add(m_rank_box, 1, wxALL | wxEXPAND, 5);

	m_main_sizer->Add(m_pl_nat_sizer, 0, wxEXPAND, 5);
	m_main_sizer->Add(0, 0, 1, wxEXPAND, 0);

	m_buttons_sep = new wxStaticLine(m_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
	m_main_sizer->Add(m_buttons_sep, 0, wxALL | wxEXPAND, 2);

	wxBoxSizer* m_buttons_sizer;
	m_buttons_sizer = new wxBoxSizer(wxHORIZONTAL);

	m_cancel_btn = new wxButton(m_panel, HOST_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0);
	m_buttons_sizer->Add(0, 0, 1, wxEXPAND, 0);
	m_buttons_sizer->Add(m_cancel_btn, 0, wxALL, 5);

	m_host_btn = new wxButton(m_panel, HOST_OK, _("Host"), wxDefaultPosition, wxDefaultSize, 0);
	m_host_btn->SetToolTip(_("Start hosting the battle."));

	m_buttons_sizer->Add(m_host_btn, 0, wxALL, 5);

	m_main_sizer->Add(m_buttons_sizer, 0, wxEXPAND, 5);

	m_panel->SetSizer(m_main_sizer);
	all_sizer->Add(m_panel, 1, wxEXPAND, 0);
	this->SetSizer(all_sizer);
	this->Layout();
	m_host_btn->SetFocus();

	ReloadEngineList();
}


void HostBattleDialog::ReloadModList()
{
	m_mod_pic->Clear();

	wxArrayString modlist = lslTowxArrayString(LSL::usync().GetGameList());
	wxString last = sett().GetLastHostMod();

	size_t nummods = modlist.Count();
	for (size_t i = 0; i < nummods; i++) {
		m_mod_pic->Insert(modlist[i], i);
		if (last == modlist[i])
			m_mod_pic->SetSelection(i);
	}

	if (m_mod_pic->GetSelection() == wxNOT_FOUND) {
		m_mod_pic->SetSelection(0);
	}
}

void HostBattleDialog::ReloadEngineList()
{
	m_engine_pic->Clear();
	std::map<std::string, LSL::SpringBundle> versions = SlPaths::GetSpringVersionList();
	const std::string last = SlPaths::GetCurrentUsedSpringIndex();
	int i = 0;
	for (auto pair : versions) {
		m_engine_pic->Insert(TowxString(pair.first), i);
		if (last == pair.first) {
			m_engine_pic->SetSelection(i);
		}
		i++;
	}

	if (m_engine_pic->GetSelection() == wxNOT_FOUND) {
		m_engine_pic->SetSelection(0);
	}
	//unitsync change needs a refresh of games as well
	ReloadModList();
}


void HostBattleDialog::OnOk(wxCommandEvent& /*unused*/)
{
	if (m_mod_pic->GetSelection() == wxNOT_FOUND) {
		wxLogWarning(_T( "no game selected" ));
		customMessageBox(SL_MAIN_ICON, _("You have to select a game first."), _("No game selected."), wxOK);
		return;
	}

	if (m_engine_pic->GetSelection() == wxNOT_FOUND) {
		wxLogWarning(_T( "no engine selected" ));
		customMessageBox(SL_MAIN_ICON, _("You have to select a engine version first."), _("No engine selected."), wxOK);
		return;
	}

	if (m_desc_text->GetValue().IsEmpty())
		m_desc_text->SetValue(_T( "(none)" ));
	sett().SetLastHostDescription(m_desc_text->GetValue());
	sett().SetLastHostMod(m_mod_pic->GetString(m_mod_pic->GetSelection()));
	wxString password = m_pwd_text->GetValue();
	password.Replace(_T(" "), wxEmptyString);
	sett().SetLastHostPassword(password);
	long tmp = DEFSETT_SPRING_PORT;
	m_port_text->GetValue().ToLong(&tmp);
	sett().SetLastHostPort(tmp);
	//  sett().SetTestHostPort( m_port_test_check->GetValue() );
	sett().SetTestHostPort(false);
	sett().SetLastHostPlayerNum(m_players_slide->GetValue());
	sett().SetLastHostNATSetting(m_nat_radios->GetSelection());
	sett().SetLastRankLimit(GetSelectedRank());
	sett().SetLastHostRelayedMode(m_relayed_host_check->GetValue());
	sett().SetBattleLastAutoAnnounceDescription(m_desc_check->GetValue());
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
	int ret = 0;
	if (m_rank0_radio->GetValue())
		ret = 0;
	if (m_rank1_radio->GetValue())
		ret = 1;
	if (m_rank2_radio->GetValue())
		ret = 2;
	if (m_rank3_radio->GetValue())
		ret = 3;
	if (m_rank4_radio->GetValue())
		ret = 4;
	if (m_rank5_radio->GetValue())
		ret = 5;
	if (m_rank6_radio->GetValue())
		ret = 6;
	if (m_rank7_radio->GetValue())
		ret = 7;
	if (m_rank_direction->GetSelection() > 0)
		ret = -ret - 1;
	return ret;
}

void HostBattleDialog::OnNatChange(wxCommandEvent& /*unused*/)
{
	//  m_port_test_check->Enable( m_nat_radios->GetSelection() == 0 );
	m_port_text->Enable(m_nat_radios->GetSelection() == 0);
}

void HostBattleDialog::OnReloadMods(wxCommandEvent&)
{
	ReloadModList();
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
	m_relayed_host_pick->SetLabel(m_last_relayhost.IsEmpty() ? _T("automatic") : m_last_relayhost);
}

void HostBattleDialog::OnUseRelay(wxCommandEvent&)
{
	m_relayed_host_pick->Show(m_relayed_host_check->IsChecked());
	m_port_text->Enable(!m_relayed_host_check->IsChecked() && m_nat_radios->GetSelection() == 0);
	m_nat_radios->Enable(!m_relayed_host_check->IsChecked());
	Layout();
}

void HostBattleDialog::OnEngineSelect(wxCommandEvent& /*event*/)
{
	SlPaths::SetUsedSpringIndex(STD_STRING(m_engine_pic->GetString(m_engine_pic->GetSelection())));
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
	bo.engineName = "spring";
	bo.engineVersion = LSL::usync().GetSpringVersion();

	// Get selected mod from unitsync.
	LSL::UnitsyncGame mod;
	try {
		mod = LSL::usync().GetGame(STD_STRING(sett().GetLastHostMod()));
		bo.gamehash = mod.hash;
		bo.gamename = mod.name;
	} catch (...) {
		wxLogWarning(_T( "can't host: game not found" ));
		customMessageBoxModal(SL_MAIN_ICON, _("Battle not started beacuse the game you selected could not be found. "), _("Error starting battle."), wxOK);
		return;
	}

	LSL::UnitsyncMap map;
	const auto mname = STD_STRING(sett().GetLastHostMap());
	if (!mname.empty() && LSL::usync().MapExists(mname)) {
		map = LSL::usync().GetMap(mname);
	} else {
		std::vector<std::string> maps = LSL::usync().GetMapList();
		if (maps.size() <= 0) {
			wxLogWarning(_T( "no maps found" ));
			customMessageBoxModal(SL_MAIN_ICON, _("Couldn't find any maps in your spring installation. This could happen when you set the Spring settings incorrectly."), _("No maps found"), wxOK);
			return;
		}
		map = LSL::usync().GetMap(maps[0]);
	}

	bo.maphash = map.hash;
	bo.mapname = map.name;

	serverSelector().GetServer().HostBattle(bo, STD_STRING(sett().GetLastHostPassword()));

}
