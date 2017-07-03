/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

//
// Class: MainJoinBattleTab
//

#include <wx/icon.h>
#include <wx/intl.h>
#include <wx/notebook.h>
#include <wx/imaglist.h>
#include <wx/sizer.h>
#include <wx/wupdlock.h>
#include <wx/log.h>

#include "settings.h"
#include "mainjoinbattletab.h"
#include "battleroomtab.h"
#include "battlemaptab.h"
#include "battleoptionstab.h"
#include "utils/conversion.h"
#include "battleroommmoptionstab.h"
#include "aui/auimanager.h"
#include "gui/slbook.h"
#include "gui/controls.h"
#include "log.h"

#include "images/battle_list.xpm"
#include "images/battle.xpm"
#include "images/battle_map.xpm"
#include "images/battle_settings.xpm"

#include <stdexcept>

MainJoinBattleTab::MainJoinBattleTab(wxWindow* parent)
    : wxPanel(parent, -1)
    , m_battle_tab(0)
    , m_map_tab(0)
    , m_opts_tab(0)
    , m_mm_opts_tab(0)
{
	GetAui().manager->AddPane(this, wxLEFT, _T( "mainjoinbattletab" ));

	m_main_sizer = new wxBoxSizer(wxVERTICAL);

	m_tabs = new SLNotebook(this, _T( "mainjoinbattletab" ), BATTLE_TABS, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TAB_SPLIT | wxAUI_NB_TAB_MOVE | wxAUI_NB_SCROLL_BUTTONS | wxAUI_NB_TOP | wxAUI_NB_TAB_EXTERNAL_MOVE);
	m_tabs->SetArtProvider(new wxAuiDefaultTabArt);

	m_imagelist = new wxImageList(12, 12);
	m_imagelist->Add(wxIcon(battle_list_xpm));
	m_imagelist->Add(wxIcon(battle_xpm));
	m_imagelist->Add(wxIcon(battle_map_xpm));
	m_imagelist->Add(wxIcon(battle_settings_xpm));

	m_battle_tab = new BattleRoomTab(m_tabs, 0);
	m_tabs->InsertPage(0, m_battle_tab, _("Battleroom"), true, wxIcon(battle_xpm));

	m_map_tab = new BattleMapTab(m_tabs, 0);
	m_tabs->InsertPage(1, m_map_tab, _("Map"), false, wxIcon(battle_map_xpm));

	m_mm_opts_tab = new BattleroomMMOptionsTab(0, m_tabs);
	m_tabs->InsertPage(2, m_mm_opts_tab, _("Options"), false, wxIcon(battle_settings_xpm));

	m_opts_tab = new BattleOptionsTab(m_tabs, 0);
	m_tabs->InsertPage(3, m_opts_tab, _("Unit Restrictions"), false, wxIcon(battle_settings_xpm));

	m_main_sizer->Add(m_tabs, 1, wxEXPAND);

	SetSizer(m_main_sizer);
	Layout();
}


MainJoinBattleTab::~MainJoinBattleTab()
{
}


IBattle* MainJoinBattleTab::GetCurrentBattle()
{
	return m_battle_tab->GetBattle();
}


ChatPanel* MainJoinBattleTab::GetActiveChatPanel()
{
	if (m_tabs->GetSelection() == 0) { //not really needed
		return &GetBattleRoomTab().GetChatPanel();
	}
	return NULL;
}

void MainJoinBattleTab::UpdateCurrentBattle(const wxString& Tag)
{
	GetBattleRoomTab().UpdateBattleInfo(Tag);
	GetBattleMapTab().Update(Tag);
	GetOptionsTab().UpdateBattle(Tag);
	GetMMOptionsTab().UpdateOptControls(Tag);
}

void MainJoinBattleTab::JoinBattle(IBattle& battle)
{
	m_mm_opts_tab->SetBattle(&battle);
	m_opts_tab->SetBattle(&battle);
	m_map_tab->SetBattle(&battle);
	m_battle_tab->SetBattle(&battle);
}


void MainJoinBattleTab::HostBattle(IBattle& battle)
{
	JoinBattle(battle);
}


void MainJoinBattleTab::LeaveCurrentBattle(bool /*called_from_join*/)
{
	m_mm_opts_tab->SetBattle(0);
	m_opts_tab->SetBattle(0);
	m_map_tab->SetBattle(0);
	m_battle_tab->SetBattle(0);
}


void MainJoinBattleTab::BattleUserUpdated(User& user)
{
	try {
		GetBattleRoomTab().UpdateUser(user);
	} catch (...) {
	}
	try {
		GetBattleMapTab().UpdateUser(user);
	} catch (...) {
	}
}

void MainJoinBattleTab::ReloadPresetList()
{
	try {
		//GetBattleRoomTab().UpdatePresetList();
	} catch (...) {
	}
	try {
		GetMMOptionsTab().UpdatePresetList();
	} catch (...) {
	}
}


BattleRoomTab& MainJoinBattleTab::GetBattleRoomTab()
{
	ASSERT_EXCEPTION(m_battle_tab, _T( "m_battle_tab == 0" ));
	return *m_battle_tab;
}


BattleMapTab& MainJoinBattleTab::GetBattleMapTab()
{
	ASSERT_EXCEPTION(m_map_tab, _T( "m_map_tab == 0" ));
	return *m_map_tab;
}


BattleOptionsTab& MainJoinBattleTab::GetOptionsTab()
{
	ASSERT_EXCEPTION(m_opts_tab, _T( "m_opts_tab == 0" ));
	return *m_opts_tab;
}


BattleroomMMOptionsTab& MainJoinBattleTab::GetMMOptionsTab()
{
	ASSERT_EXCEPTION(m_mm_opts_tab, _T( "m_mm_opts_tab == 0" ));
	return *m_mm_opts_tab;
}

void MainJoinBattleTab::LoadPerspective(const wxString& perspective_name)
{
	LoadNotebookPerspective(m_tabs, perspective_name);
}

void MainJoinBattleTab::SavePerspective(const wxString& perspective_name)
{
	SaveNotebookPerspective(m_tabs, perspective_name);
}

bool MainJoinBattleTab::UseBattlePerspective()
{
	return GetBattleRoomTab().GetBattle() != 0;
}
