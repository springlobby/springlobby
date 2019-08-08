/* This file is part of the Springlobby (GPL v2 or later), see COPYING */
#include "gui/hosting/mainsingleplayertab.h"

#include <wx/icon.h>
#include <wx/imaglist.h>
#include <wx/intl.h>
#include <wx/log.h>
#include <wx/sizer.h>
#include <wx/window.h>
#include <stdexcept>

#include "aui/auimanager.h"
#include "gui/controls.h"
#include "gui/hosting/battlerestrictionstab.h"
#include "gui/hosting/battleroommmoptionstab.h"
#include "gui/singleplayertab.h"
#include "gui/slbook.h"
#include "images/battle.xpm"
#include "images/battle_settings.xpm"
#include "log.h"
#include "utils/conversion.h"

MainSinglePlayerTab::MainSinglePlayerTab(wxWindow* parent)
    : wxPanel(parent, -1),
	// These three null-initialisations are because in the following code, SinglePlayerTab's
	// constructor creates a chain of function calls that culminate in m_mm_opts_tab being
	// accessed while the latter is not yet initialised. However, there is already a protective
	// (!= nullptr) wrapper, so these starting calls get converted into what is right now a group
	// of harmless startup warnings.
	m_sp_tab(nullptr), m_opts_tab(nullptr), m_mm_opts_tab(nullptr)
{
	m_main_sizer = new wxBoxSizer(wxVERTICAL);
	GetAui().manager->AddPane(this, wxLEFT, _T( "mainsingleplayertab" ));
	m_tabs = new SLNotebook(this, _T( "mainsingleplayertab" ), -1, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TAB_SPLIT | wxAUI_NB_TAB_MOVE | wxAUI_NB_SCROLL_BUTTONS | wxAUI_NB_TOP | wxAUI_NB_TAB_EXTERNAL_MOVE);
	m_tabs->SetArtProvider(new wxAuiDefaultTabArt);

	m_imagelist = new wxImageList(12, 12);
	m_imagelist->Add(wxIcon(battle_xpm));
	m_imagelist->Add(wxIcon(battle_settings_xpm));

	m_sp_tab = new SinglePlayerTab(m_tabs, *this);
	m_tabs->AddPage(m_sp_tab, _("Game"), true, wxNullBitmap);
	m_mm_opts_tab = new BattleroomMMOptionsTab(&m_sp_tab->GetBattle(), m_tabs);
	m_tabs->InsertPage(1, m_mm_opts_tab, _("Options"), false, wxIcon(battle_settings_xpm));
	m_opts_tab = new BattleRestrictionsTab(m_tabs, &m_sp_tab->GetBattle());
	m_tabs->InsertPage(2, m_opts_tab, _("Unit Restrictions"), false, wxIcon(battle_settings_xpm));

	m_main_sizer->Add(m_tabs, 1, wxEXPAND);

	SetSizer(m_main_sizer);
	Layout();
}


MainSinglePlayerTab::~MainSinglePlayerTab()
{
	wxDELETE(m_imagelist);
	//    if ( sett().GetAutosavePerspective() )
	//        SavePerspective();
}

void MainSinglePlayerTab::LoadPerspective(const wxString& perspective_name)
{
	LoadNotebookPerspective(m_tabs, perspective_name);
}

void MainSinglePlayerTab::SavePerspective(const wxString& perspective_name)
{
	SaveNotebookPerspective(m_tabs, perspective_name);
}

void MainSinglePlayerTab::UpdateMinimap()
{
	try {
		GetSinglePlayerTab().UpdateMinimap();
	} catch (...) {
	}
}


void MainSinglePlayerTab::OnUnitSyncReloaded()
{
	slLogDebugFunc("");
	try {
		GetSinglePlayerTab().ResetUsername();
		wxLogMessage(_T( "Reloading map list" ));
		GetSinglePlayerTab().ReloadMapList();
		wxLogMessage(_T( "Reloading game list" ));
		GetSinglePlayerTab().ReloadGameList();
		wxLogMessage(_T( "Reloading minimap" ));
		GetSinglePlayerTab().UpdateMinimap();
	} catch (...) {
	}
}


void MainSinglePlayerTab::ReloadRestrictions()
{
	try {
		GetOptionsTab().ReloadRestrictions();
	} catch (...) {
	}
}


void MainSinglePlayerTab::ReloadMapOptContrls()
{
	try {
		GetMMOptionsTab().OnReloadControls(LSL::Enum::MapOption);
	} catch (...) {
	}
}


void MainSinglePlayerTab::ReloadModOptContrls()
{
	try {
		GetMMOptionsTab().OnReloadControls(LSL::Enum::ModOption);
	} catch (...) {
	}
}

SinglePlayerTab& MainSinglePlayerTab::GetSinglePlayerTab()
{
	ASSERT_EXCEPTION(m_sp_tab, _T( "m_sp_tab == 0" ));
	return *m_sp_tab;
}

BattleRestrictionsTab& MainSinglePlayerTab::GetOptionsTab()
{
	ASSERT_EXCEPTION(m_opts_tab, _T( "m_opts_tab == 0" ));
	return *m_opts_tab;
}


BattleroomMMOptionsTab& MainSinglePlayerTab::GetMMOptionsTab()
{
	ASSERT_EXCEPTION(m_mm_opts_tab, _T( "m_mm_opts_tab == 0" ));
	return *m_mm_opts_tab;
}


void MainSinglePlayerTab::ReloadPresetList()
{
	try {
		GetSinglePlayerTab().UpdatePresetList();
	} catch (...) {
	}
	try {
		GetMMOptionsTab().UpdatePresetList();
	} catch (...) {
	}
}
