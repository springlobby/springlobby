/* This file is part of the Springlobby (GPL v2 or later), see COPYING */


#include <wx/intl.h>
#include <wx/sizer.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/checkbox.h>
#include <wx/colordlg.h>
#include <wx/listctrl.h>
#include <wx/settings.h>
#include <wx/log.h>

#include <lslutils/conversion.h>

#include "singleplayertab.h"
#include "mapctrl.h"
#include "mapselectdialog.h"
#include "gui/controls.h"
#include "utils/conversion.h"
#include "uiutils.h"
#include "ui.h"
#include "hosting/addbotdialog.h"
#include "iserver.h"
#include "settings.h"
#include "gui/colorbutton.h"
#include "aui/auimanager.h"
#include "gui/customdialogs.h"
#include "utils/slpaths.h"
#include "utils/globalevents.h"
#include "log.h"
#include "utils/lslconversion.h"
#include "utils/slconfig.h"

BEGIN_EVENT_TABLE(SinglePlayerTab, wxPanel)

EVT_CHOICE(SP_MAP_PICK, SinglePlayerTab::OnMapSelect)
EVT_CHOICE(SP_MOD_PICK, SinglePlayerTab::OnModSelect)
EVT_CHOICE(SP_ENGINE_PICK, SinglePlayerTab::OnEngineSelect)
EVT_BUTTON(SP_BROWSE_MAP, SinglePlayerTab::OnMapBrowse)
EVT_BUTTON(SP_ADD_BOT, SinglePlayerTab::OnAddBot)
EVT_BUTTON(SP_RESET, SinglePlayerTab::OnReset)
EVT_BUTTON(SP_START, SinglePlayerTab::OnStart)
EVT_CHECKBOX(SP_RANDOM, SinglePlayerTab::OnRandomCheck)
EVT_CHECKBOX(SP_SPECTATE, SinglePlayerTab::OnSpectatorCheck)
EVT_BUTTON(SP_COLOUR, SinglePlayerTab::OnColorButton)

END_EVENT_TABLE()


SinglePlayerTab::SinglePlayerTab(wxWindow* parent, MainSinglePlayerTab& msptab)
    : wxPanel(parent, -1)
    , m_battle(msptab)
{
	GetAui().manager->AddPane(this, wxLEFT, _T("singleplayertab"));

	m_main_sizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* m_mapabour_sizer = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* m_map_sizer = new wxBoxSizer(wxHORIZONTAL);
	// m_map_sizer->SetMinSize( wxSize( 352, -1 ) );


	// empty panel to replace minimap
	m_nominimap = new wxWindow(this, -1, wxDefaultPosition, wxSize(100, 100), wxSIMPLE_BORDER | wxFULL_REPAINT_ON_RESIZE);

	m_nominimap->SetToolTip(_("No Unitsync configured."));
	m_nominimap->SetBackgroundStyle(wxBG_STYLE_CUSTOM);
	m_nominimap->SetBackgroundColour(*wxLIGHT_GREY);
	m_nominimap->Hide();
	m_map_sizer->Add(m_nominimap, 1, wxALL | wxEXPAND, 2);

//	Regular minimap

	m_minimap = new MapCtrl(this, 100, &m_battle, false, true, true);
	m_minimap->SetToolTip(_("You can drag the sun/bot icon around to define start position.\n "
				"Hover over the icon for a popup that lets you change side, ally and bonus."));
	m_map_sizer->Add(m_minimap, 1, wxALL | wxEXPAND, 2);
	m_mapabour_sizer->Add(m_map_sizer, 1, wxEXPAND, 2);

	//map description and parametrs like in battletab
	wxBoxSizer* m_opts_sizer = new wxBoxSizer(wxVERTICAL);

	m_map_opts_list = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(150, 160), wxLC_NO_HEADER | wxLC_REPORT);
	m_map_opts_list->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
	m_map_opts_list->SetFont(wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_LIGHT));

	wxListItem col;

	col.SetText(_("Option"));
	m_map_opts_list->InsertColumn(0, col);
	col.SetText(_("Value"));
	m_map_opts_list->InsertColumn(1, col);
	m_map_opts_list->SetColumnWidth(0, 90);
	m_map_opts_list->SetColumnWidth(1, 50);

	m_map_opts_list->InsertItem(0, _("Size"));
	m_map_opts_list->InsertItem(1, _("Windspeed"));
	m_map_opts_list->InsertItem(2, _("Tidal strength"));
	m_map_opts_list->InsertItem(3, _("Gravity"));
	m_map_opts_list->InsertItem(4, _("Extractor radius"));
	m_map_opts_list->InsertItem(5, _("Max metal"));

	m_opts_sizer->Add(m_map_opts_list, 0, wxALL, 2);

	m_map_desc = new wxStaticText(this, -1, wxEmptyString);
	m_map_desc->Wrap(160);

	m_opts_sizer->Add(m_map_desc, 0, wxALL, 2);
	m_mapabour_sizer->Add(m_opts_sizer, 0, wxALL | wxEXPAND, 2);
	m_main_sizer->Add(m_mapabour_sizer, 1, wxEXPAND, 5);

	wxBoxSizer* m_ctrl_sizer = new wxBoxSizer(wxHORIZONTAL);

	m_mod_lbl = new wxStaticText(this, -1, _("Game:"));
	m_ctrl_sizer->Add(m_mod_lbl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	m_game_choice = new wxChoice(this, SP_MOD_PICK);
	m_game_choice->SetToolTip(_("No games? Download them by joining a multiplayer game room"));
	m_ctrl_sizer->Add(m_game_choice, 1, wxALL, 5);

	m_map_lbl = new wxStaticText(this, -1, _("Map:"));
	m_ctrl_sizer->Add(m_map_lbl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	m_map_pick = new wxChoice(this, SP_MAP_PICK);
	m_map_pick->SetToolTip(_("No maps? Download them by joining a multiplayer game room"));
	m_ctrl_sizer->Add(m_map_pick, 1, wxALL, 5);

	m_select_btn = new wxButton(this, SP_BROWSE_MAP, _("Choose map..."), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	m_ctrl_sizer->Add(m_select_btn, 0, wxBOTTOM | wxRIGHT | wxTOP, 5);

	m_mod_lbl = new wxStaticText(this, -1, _("Engine:"));
	m_ctrl_sizer->Add(m_mod_lbl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	m_engine_choice = new wxChoice(this, SP_ENGINE_PICK);
	m_engine_choice->SetToolTip(_("No engines? Download them by joining a multiplayer game room"));
	m_ctrl_sizer->Add(m_engine_choice, 1, wxALL, 5);

	//  m_ctrl_sizer->Add( 0, 0, 1, wxEXPAND, 0 );

	m_addbot_btn = new wxButton(this, SP_ADD_BOT, _("Add bot..."), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	m_addbot_btn->SetToolTip(_("First select game, map and engine"));
	m_ctrl_sizer->Add(m_addbot_btn, 0, wxALL, 5);

	m_main_sizer->Add(m_ctrl_sizer, 0, wxEXPAND, 5);

	m_buttons_sep = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
	m_main_sizer->Add(m_buttons_sep, 0, wxLEFT | wxRIGHT | wxEXPAND, 5);

	wxBoxSizer* m_buttons_sizer = new wxBoxSizer(wxHORIZONTAL);

	m_buttons_sizer->Add(0, 0, 1, wxEXPAND, 0);

	m_color_btn = new ColorButton(this, SP_COLOUR, sett().GetBattleLastColour(), wxDefaultPosition, wxSize(30, CONTROL_HEIGHT));
	m_buttons_sizer->Add(m_color_btn, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);

	m_spectator_check = new wxCheckBox(this, SP_SPECTATE, _("Spectate only"));
	m_buttons_sizer->Add(m_spectator_check, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	m_random_check = new wxCheckBox(this, SP_RANDOM, _("Random start positions"));
	m_buttons_sizer->Add(m_random_check, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	m_start_btn = new wxButton(this, SP_START, _("Start"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	m_start_btn->SetToolTip(_("First select game, map and engine"));
	m_buttons_sizer->Add(m_start_btn, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	m_main_sizer->Add(m_buttons_sizer, 0, wxEXPAND, 5);

	m_battle.SetEngineName("spring");
	m_battle.SetEngineVersion(SlPaths::GetCurrentUsedSpringIndex());

	ReloadMaplist();
//	ReloadModlist(); //Called from ReloadEngineList() too
	ReloadEngineList();
	SUBSCRIBE_GLOBAL_EVENT(GlobalEventManager::OnUnitsyncReloaded, SinglePlayerTab::OnUnitsyncReloaded);

	this->SetSizer(m_main_sizer);
	this->Layout();
}


SinglePlayerTab::~SinglePlayerTab()
{
	GlobalEventManager::Instance()->UnSubscribeAll(this);
	wxDELETE(m_minimap);
}


void SinglePlayerTab::UpdateMinimap()
{
	m_minimap->UpdateMinimap();
}


void SinglePlayerTab::ReloadMaplist()
{
	m_map_pick->Clear();
	m_map_pick->Append(lslTowxArrayString(LSL::usync().GetMapList()));
	m_map_pick->Insert(_("-- Select one --"), m_map_pick->GetCount());
	if (m_battle.GetHostMapName().empty()) {
		m_map_pick->SetSelection(m_map_pick->GetCount() - 1);
		m_addbot_btn->Enable(false);
	} else {
		m_map_pick->SetStringSelection(TowxString(m_battle.GetHostMapName()));
		if (m_map_pick->GetStringSelection().IsEmpty()) {
			SetMap(m_map_pick->GetCount() - 1);
		}
	}
}


void SinglePlayerTab::ReloadModlist()
{
	m_game_choice->Clear();
	m_game_choice->Append(lslTowxArrayString(LSL::usync().GetGameList()));
	m_game_choice->Insert(_("-- Select one --"), m_game_choice->GetCount());
	if (m_battle.GetHostGameName().empty()) {
		m_game_choice->SetSelection(m_game_choice->GetCount() - 1);
	} else {
		m_game_choice->SetStringSelection(TowxString(m_battle.GetHostGameName()));
		if (m_game_choice->GetStringSelection().empty()) {
			SetMod(m_game_choice->GetCount() - 1);
		}
	}
}


void SinglePlayerTab::ReloadEngineList()
{
	SlPaths::ValidatePaths();

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

	if (i == 0) {
		m_minimap->Hide();
		m_nominimap->Show();
		m_start_btn->Enable(false);
	} else {
		m_nominimap->Hide();
		m_minimap->Show();
		m_start_btn->Enable(true);
	}
	//unitsync change needs a refresh of games as well
	ReloadModlist();
}

void SinglePlayerTab::SetMap(unsigned int index)
{
	//ui().ReloadUnitSync();
	m_addbot_btn->Enable(false);
	if (index >= m_map_pick->GetCount() - 1) {
		m_battle.SetHostMap("", "");
		int count = m_map_opts_list->GetItemCount();
		for (int i = 0; i < count; i++)
			m_map_opts_list->SetItem(i, 1, wxEmptyString);
		m_map_desc->SetLabel(wxEmptyString);
	} else {
		try {
			LSL::UnitsyncMap map = LSL::usync().GetMap(index);
			m_battle.SetHostMap(map.name, map.hash);
			m_addbot_btn->Enable(true);
			m_map_opts_list->SetItem(0, 1, wxString::Format(_T( "%dx%d" ), map.info.width / 512, map.info.height / 512));
			m_map_opts_list->SetItem(1, 1, wxString::Format(_T( "%d-%d" ), map.info.minWind, map.info.maxWind));
			m_map_opts_list->SetItem(2, 1, wxString::Format(_T( "%d" ), map.info.tidalStrength));
			m_map_opts_list->SetItem(3, 1, wxString::Format(_T( "%d" ), map.info.gravity));
			m_map_opts_list->SetItem(4, 1, wxString::Format(_T( "%d" ), map.info.extractorRadius));
			m_map_opts_list->SetItem(5, 1, wxString::Format(_T( "%.3f" ), map.info.maxMetal));
			m_map_desc->SetLabel(TowxString(map.info.description));
			m_map_desc->Wrap(160);
		} catch (...) {
		}
	}
	m_minimap->UpdateMinimap();
	m_battle.SendHostInfo(IBattle::HI_Map_Changed); // reload map options
	m_map_pick->SetSelection(index);
}

void SinglePlayerTab::ResetUsername()
{
	m_battle.GetMe().SetNick(STD_STRING(cfg().ReadString("/Spring/DefaultName")));
}

void SinglePlayerTab::SetMod(unsigned int index)
{
	//ui().ReloadUnitSync();
	if (index >= m_game_choice->GetCount() - 1) {
		m_battle.SetHostGame("", "");
	} else {
		try {
			LSL::UnitsyncGame mod = LSL::usync().GetGame(index);
			m_battle.SetLocalGame(mod);
			m_battle.SetHostGame(mod.name, mod.hash);
		} catch (...) {
		}
	}
	m_minimap->UpdateMinimap();
	m_battle.SendHostInfo(IBattle::HI_Restrictions); // Update restrictions in options.
	m_battle.SendHostInfo(IBattle::HI_Game_Changed); // reload mod options
	m_game_choice->SetSelection(index);
}


bool SinglePlayerTab::ValidSetup() const
{
	if (m_game_choice->GetSelection() == wxNOT_FOUND) {
		wxLogWarning(_T("no game selected"));
		customMessageBox(SL_MAIN_ICON, _("You have to select a game first."), _("Game setup error"));
		return false;
	}

	if (m_map_pick->GetSelection() == wxNOT_FOUND) {
		wxLogWarning(_T("no map selected"));
		customMessageBox(SL_MAIN_ICON, _("You have to select a map first."), _("Game setup error"));
		return false;
	}

	if (m_battle.GetNumUsers() == 1) {
		wxLogWarning(_T("trying to start sp game without bot"));
	}
	return true;
}


void SinglePlayerTab::OnMapSelect(wxCommandEvent& /*unused*/)
{
	const int index = m_map_pick->GetCurrentSelection();

	if (index == wxNOT_FOUND) { return; }

	SetMap(index);
}


void SinglePlayerTab::OnModSelect(wxCommandEvent& /*unused*/)
{
	const int index = m_game_choice->GetCurrentSelection();

	if (index == wxNOT_FOUND) { return; }

	size_t num_bots = m_battle.GetNumBots();
	SetMod(index);
	if (num_bots != m_battle.GetNumBots())
		customMessageBoxModal(SL_MAIN_ICON, _("Incompatible bots have been removed after game selection changed."), _("Bots removed"));
}

void SinglePlayerTab::OnEngineSelect(wxCommandEvent& /*event*/)
{
	const int index = m_engine_choice->GetSelection();

	if (index == wxNOT_FOUND) {
		wxLogError("Invalid index selected: %d > %d", index, m_engine_choice->GetCount());
		return;
	}
	const std::string selection = STD_STRING(m_engine_choice->GetString(index));
	wxLogMessage("Selected engine version %s", selection.c_str());

	SlPaths::SetUsedSpringIndex(selection);
	m_battle.SetEngineVersion(selection);
	LSL::usync().ReloadUnitSyncLib();
}

void SinglePlayerTab::OnMapBrowse(wxCommandEvent& /*unused*/)
{
	slLogDebugFunc("");
	const wxString mapname = mapSelectDialog();
	if (!mapname.empty()) {
		const int idx = m_map_pick->FindString(mapname, true /*case sensitive*/);
		if (idx != wxNOT_FOUND) {
			SetMap(idx);
		}
	}
}


void SinglePlayerTab::OnAddBot(wxCommandEvent& /*unused*/)
{
	AddBotDialog dlg(this, m_battle, true);
	if (dlg.ShowModal() == wxID_OK) {
		UserBattleStatus bs;
		bs.owner = m_battle.GetMe().GetNick();
		bs.aishortname = STD_STRING(dlg.GetAIShortName());
		bs.airawname = STD_STRING(dlg.GetAiRawName());
		bs.aiversion = STD_STRING(dlg.GetAIVersion());
		bs.aitype = dlg.GetAIType();
		bs.team = m_battle.GetFreeTeam();
		bs.ally = m_battle.GetFreeAlly();
		bs.colour = m_battle.GetNewColour();
		m_battle.OnBotAdded(STD_STRING(dlg.GetNick()), bs);
		m_minimap->UpdateMinimap();
	}
}

void SinglePlayerTab::OnUnitsyncReloaded(wxCommandEvent& /*data*/)
{
	try {
		ReloadMaplist();
		ReloadModlist();
		ReloadEngineList();
		UpdateMinimap();
	} catch (...) {
		slLogDebugFunc("");
		wxLogError(_T("unitsync reload sink failed"));
	}
	Layout();
}


void SinglePlayerTab::OnStart(wxCommandEvent& /*unused*/)
{
	slLogDebugFunc("SP: ");

	if (ui().IsSpringRunning()) {
		wxLogWarning(_T("You cannot start a game (engine) while another engine is already running"));
		customMessageBoxModal(SL_MAIN_ICON,
		  _("You cannot start a game (engine) while another engine is already running"),
		  _("Spring error"), wxICON_EXCLAMATION);
		return;
	}

	if (ValidSetup())
		m_battle.StartSpring();
}


void SinglePlayerTab::OnRandomCheck(wxCommandEvent& /*unused*/)
{
	if (m_random_check->IsChecked())
		m_battle.CustomBattleOptions().setSingleOption("startpostype",
							       LSL::Util::ToIntString(IBattle::ST_Random), LSL::Enum::EngineOption);
	else
		m_battle.CustomBattleOptions().setSingleOption("startpostype",
							       LSL::Util::ToIntString(IBattle::ST_Pick), LSL::Enum::EngineOption);
	m_battle.SendHostInfo(IBattle::HI_StartType);
}

void SinglePlayerTab::OnSpectatorCheck(wxCommandEvent& /*unused*/)
{
	m_battle.GetMe().BattleStatus().spectator = m_spectator_check->IsChecked();
	UpdateMinimap();
}

void SinglePlayerTab::OnColorButton(wxCommandEvent& /*unused*/)
{
	User& u = m_battle.GetMe();
	wxColour CurrentColour = lslTowxColour(u.BattleStatus().colour);
	CurrentColour = GetColourFromUser(this, CurrentColour);
	if (!CurrentColour.IsOk())
		return;
	sett().SetBattleLastColour(CurrentColour);
	m_battle.ForceColour(u, wxColourTolsl(CurrentColour));
	UpdateMinimap();
}

void SinglePlayerTab::UpdatePresetList()
{
}

void SinglePlayerTab::OnReset(wxCommandEvent& /*unused*/)
{
}
