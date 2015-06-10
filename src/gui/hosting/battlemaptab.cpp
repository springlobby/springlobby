/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

//
// Class: BattleMapTab
//

#include <wx/splitter.h>
#include <wx/intl.h>
#include <wx/combobox.h>
#include <wx/stattext.h>
#include <wx/statline.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/radiobox.h>
#include <wx/window.h>
#include <wx/listctrl.h>
#include <wx/settings.h>
#include <wx/arrstr.h>
#include <wx/choice.h>

#include <stdexcept>

#include "battlemaptab.h"
#include "user.h"
#include "ibattle.h"
#include "gui/controls.h"
#include "gui/chatpanel.h"
#include "gui/mapselectdialog.h"
#include "gui/uiutils.h"
#include "iserver.h"
#include "settings.h"
#include "aui/auimanager.h"
#include "utils/conversion.h"
#include "utils/globalevents.h"
#include "log.h"
#include "gui/mapctrl.h"

#include <lslutils/conversion.h>

BEGIN_EVENT_TABLE(BattleMapTab, wxPanel)

EVT_CHOICE(BMAP_MAP_SEL, BattleMapTab::OnMapSelect)
EVT_BUTTON(BMAP_MAP_BROWSE, BattleMapTab::OnMapBrowse)
EVT_RADIOBOX(BMAP_START_TYPE, BattleMapTab::OnStartTypeSelect)

END_EVENT_TABLE()


BattleMapTab::BattleMapTab(wxWindow* parent, IBattle* battle)
    : wxPanel(parent, -1)
    , m_battle(battle)
{
	GetAui().manager->AddPane(this, wxLEFT, _T( "battlemaptab" ));

	wxBoxSizer* m_main_sizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* m_map_sizer = new wxBoxSizer(wxVERTICAL);

	m_map_sizer->SetMinSize(wxSize(352, -1));
	m_minimap = new MapCtrl(this, 352, m_battle, true, true, false);

	m_minimap->SetMinSize(wxSize(352, 352));

	m_map_sizer->Add(m_minimap, 1, wxALL | wxEXPAND, 2);

	wxBoxSizer* m_selmap_sizer = new wxBoxSizer(wxHORIZONTAL);

	m_map_combo = new wxChoice(this, BMAP_MAP_SEL, wxDefaultPosition, wxDefaultSize);
	m_selmap_sizer->Add(m_map_combo, 1, wxALL, 2);

	m_browse_btn = new wxButton(this, BMAP_MAP_BROWSE, _("Select"), wxDefaultPosition, wxDefaultSize, 0);

	m_selmap_sizer->Add(m_browse_btn, 0, wxALL, 2);

	m_map_sizer->Add(m_selmap_sizer, 0, wxEXPAND, 5);

	m_main_sizer->Add(m_map_sizer, 1, wxEXPAND, 5);

	wxBoxSizer* m_opts_sizer = new wxBoxSizer(wxVERTICAL);

	//m_opts_sizer->SetMinSize(wxSize( 200,-1 ));
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


	wxString m_start_radiosChoices[] = {_("Fixed"), _("Random"), _("Choose in game"), _("Chose before game")};
	int m_start_radiosNChoices = sizeof(m_start_radiosChoices) / sizeof(wxString);
	//TODO these need to be tooltipped, no idea how yet
	m_start_radios = new wxRadioBox(this, BMAP_START_TYPE, _("Startpositions"), wxDefaultPosition, wxSize(150, -1), m_start_radiosNChoices, m_start_radiosChoices, 1, wxRA_SPECIFY_COLS);

	m_opts_sizer->Add(m_start_radios, 0, wxALL, 2);

	m_map_desc = new wxStaticText(this, -1, wxEmptyString);
	m_map_desc->Wrap(160);

	m_opts_sizer->Add(m_map_desc, 0, wxALL, 2);

	m_main_sizer->Add(m_opts_sizer, 0, wxEXPAND, 5);
	//m_main_sizer->AddStretchSpacer();
	SetSizer(m_main_sizer);
	Layout();

	SetBattle(battle);

	Layout();
	GlobalEventManager::Instance()->Subscribe(this, GlobalEventManager::OnUnitsyncReloaded, wxObjectEventFunction(&BattleMapTab::OnUnitsyncReloaded));
}


BattleMapTab::~BattleMapTab()
{
	if (GetAui().manager)
		GetAui().manager->DetachPane(this);
}

void BattleMapTab::Update()
{
	if (!m_battle)
		return;
	const long longval = LSL::Util::FromIntString(
	    m_battle->CustomBattleOptions().getSingleValue("startpostype", LSL::Enum::EngineOption));
	m_start_radios->SetSelection(longval);

	m_minimap->UpdateMinimap();

	if (!m_battle->MapExists())
		return;

	LSL::UnitsyncMap map = m_battle->LoadMap();

	m_map_opts_list->SetItem(0, 1, wxString::Format(_T("%dx%d"), map.info.width / 512, map.info.height / 512));
	m_map_opts_list->SetItem(1, 1, wxString::Format(_T("%d-%d"), map.info.minWind, map.info.maxWind));
	m_map_opts_list->SetItem(2, 1, wxString::Format(_T("%d"), map.info.tidalStrength));
	m_map_opts_list->SetItem(3, 1, wxString::Format(_T("%d"), map.info.gravity));
	m_map_opts_list->SetItem(4, 1, wxString::Format(_T("%d"), map.info.extractorRadius));
	m_map_opts_list->SetItem(5, 1, wxString::Format(_T("%.3f"), map.info.maxMetal));

	m_map_desc->SetLabel(TowxString(map.info.description));
	m_map_desc->Wrap(160);

	int index = m_map_combo->FindString(TowxString(map.name));
	if (index == wxNOT_FOUND)
		return;
	m_map_combo->SetSelection(index);
}


void BattleMapTab::Update(const wxString& Tag)
{
	if (!m_battle)
		return;
	long type;
	Tag.BeforeFirst('_').ToLong(&type);
	const std::string key = STD_STRING(Tag.AfterFirst('_'));
	if (type == LSL::Enum::EngineOption) {
		if (key == "startpostype") {
			const long longval = LSL::Util::FromIntString(m_battle->CustomBattleOptions().getSingleValue(key, (LSL::Enum::GameOption)type));
			m_start_radios->SetSelection(longval);
			m_minimap->UpdateMinimap();
		}
	} else if (type == LSL::Enum::PrivateOptions) {
		if (key == "mapname") {
			Update();
		}
	}
}


void BattleMapTab::ReloadMaplist()
{
	if (!m_battle)
		return;
	m_map_combo->Clear();
	LSL::StringVector maps = LSL::usync().GetMapList();
	for (const std::string map : maps) {
		m_map_combo->Append(TowxString(map));
	}
}


void BattleMapTab::UpdateUser(User& user)
{
	if (!m_battle)
		return;
	if (&m_battle->GetMe() == &user) {
		try {
			m_minimap->UpdateMinimap();
		} catch (...) {
		}
	}
}


void BattleMapTab::SetMap(int index)
{
	if (!m_battle)
		return;
	try {
		m_battle->SetLocalMap(STD_STRING(m_map_combo->GetString(index)));
		m_battle->SendHostInfo(IBattle::HI_Map);
	} catch (...) {
	}
}


void BattleMapTab::OnMapSelect(wxCommandEvent& /*unused*/)
{
	if (!m_battle)
		return;
	if (!m_battle->IsFounderMe()) {
		try {
			m_battle->DoAction("suggests " + STD_STRING(m_map_combo->GetString(m_map_combo->GetCurrentSelection())));
		} catch (...) {
		}
		return;
	}
	SetMap(m_map_combo->GetCurrentSelection());
}


void BattleMapTab::OnMapBrowse(wxCommandEvent& /*unused*/)
{
	if (!m_battle)
		return;
	slLogDebugFunc("");

	wxString mapname = mapSelectDialog();
	if (!mapname.empty()) {
		slLogDebugFunc("");
		if (!m_battle->IsFounderMe()) {
			m_battle->DoAction("suggests " + STD_STRING(mapname));
			return;
		}
		const int idx = m_map_combo->FindString(mapname, true /*case sensitive*/);
		if (idx != wxNOT_FOUND)
			SetMap(idx);
	}
}


void BattleMapTab::OnStartTypeSelect(wxCommandEvent& /*unused*/)
{
	if (!m_battle)
		return;
	const std::string pos = stdprintf("%d", m_start_radios->GetSelection());
	m_battle->CustomBattleOptions().setSingleOption("startpostype", pos, LSL::Enum::EngineOption);
	m_battle->SendHostInfo(stdprintf("%d_startpostype", LSL::Enum::EngineOption));
}


void BattleMapTab::OnUnitsyncReloaded(wxCommandEvent& /*data*/)
{
	if (!m_battle)
		return;
	ReloadMaplist();
}

void BattleMapTab::SetBattle(IBattle* battle)
{
	m_battle = battle;
	bool isBattleEnabled = (battle != nullptr);

	m_start_radios->Enable(isBattleEnabled);
	m_minimap->Enable(isBattleEnabled);
	m_map_combo->Enable(isBattleEnabled);
	m_browse_btn->Enable(isBattleEnabled);
	m_map_opts_list->Enable(isBattleEnabled);

	m_minimap->SetBattle(m_battle);

	if (isBattleEnabled) {
		m_minimap->SetReadOnly(!m_battle->IsFounderMe());
		m_start_radios->Enable(m_battle->IsFounderMe());
		ReloadMaplist();
		Update();
	}
}
