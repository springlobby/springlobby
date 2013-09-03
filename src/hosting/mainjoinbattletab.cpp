/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
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

#include "ui.h"
#include "mainwindow.h" //used only for global pers load/save, remove when signals are merged
#include "settings.h"
#include "battle.h"
#include "mainjoinbattletab.h"
#include "battleroomtab.h"
#include "battlemaptab.h"
#include "battleoptionstab.h"
#include "utils/debug.h"
#include "utils/conversion.h"
#include "battleroommmoptionstab.h"
#include "aui/auimanager.h"
#include "aui/artprovider.h"
#include "aui/slbook.h"

#include "images/battle_list.xpm"
#include "images/battle.xpm"
#include "images/battle_map.xpm"
#include "images/battle_settings.xpm"

#include <stdexcept>

MainJoinBattleTab::MainJoinBattleTab( wxWindow* parent )
    : wxScrolledWindow( parent, -1 ),
    m_battle_tab( 0 ),
    m_map_tab( 0 ),
    m_opts_tab( 0 ),
    m_mm_opts_tab( 0 )
{
	GetAui().manager->AddPane( this, wxLEFT, _T( "mainjoinbattletab" ) );

	m_main_sizer = new wxBoxSizer( wxVERTICAL );

	m_tabs = new SLNotebook( this, _T( "mainjoinbattletab" ), BATTLE_TABS ,wxDefaultPosition, wxDefaultSize, wxAUI_NB_TAB_SPLIT | wxAUI_NB_TAB_MOVE | wxAUI_NB_SCROLL_BUTTONS | wxAUI_NB_TOP | wxAUI_NB_TAB_EXTERNAL_MOVE );
	m_tabs->SetArtProvider( new SLArtProvider );

	m_imagelist = new wxImageList( 12, 12 );
	m_imagelist->Add( wxIcon( battle_list_xpm ) );
	m_imagelist->Add( wxIcon( battle_xpm ) );
	m_imagelist->Add( wxIcon( battle_map_xpm ) );
	m_imagelist->Add( wxIcon( battle_settings_xpm ) );

	m_battle_tab = new BattleRoomTab( m_tabs, 0 );
	m_tabs->InsertPage( 0, m_battle_tab, _( "Battleroom" ), true, wxIcon( battle_xpm ) );

	m_map_tab = new BattleMapTab( m_tabs, 0 );
	m_tabs->InsertPage( 1, m_map_tab, _( "Map" ), false, wxIcon( battle_map_xpm ) );

	m_mm_opts_tab = new BattleroomMMOptionsTab<Battle>( 0, m_tabs );
	m_tabs->InsertPage( 2, m_mm_opts_tab, _( "Options" ), false, wxIcon( battle_settings_xpm ) );

	m_opts_tab = new BattleOptionsTab( m_tabs, 0 );
	m_tabs->InsertPage( 3, m_opts_tab, _( "Unit Restrictions" ), false, wxIcon( battle_settings_xpm ) );

	m_main_sizer->Add( m_tabs, 1, wxEXPAND );

	SetScrollRate( SCROLL_RATE, SCROLL_RATE );
	SetSizer( m_main_sizer );
	Layout();
}


MainJoinBattleTab::~MainJoinBattleTab()
{
}


Battle* MainJoinBattleTab::GetCurrentBattle()
{
	return m_battle_tab->GetBattle();
}


ChatPanel* MainJoinBattleTab::GetActiveChatPanel()
{
	if ( m_tabs->GetSelection() == 1 ) {
		return &GetBattleRoomTab().GetChatPanel();
	}
	return 0;
}

void MainJoinBattleTab::UpdateCurrentBattle()
{
	GetBattleRoomTab().UpdateBattleInfo();
	GetBattleMapTab().Update();
}

void MainJoinBattleTab::UpdateCurrentBattle( const wxString& Tag )
{
	GetBattleRoomTab().UpdateBattleInfo( Tag );
	GetBattleMapTab().Update( Tag );
	GetOptionsTab().UpdateBattle( Tag );
	GetMMOptionsTab().UpdateOptControls( Tag );
}


void MainJoinBattleTab::JoinBattle( Battle& battle )
{
    PreSwitchBattlePerspective();
	m_mm_opts_tab->SetBattle( &battle );
	m_opts_tab->SetBattle( &battle );
	m_map_tab->SetBattle( &battle );
	m_battle_tab->SetBattle( &battle );
	PostSwitchBattlePerspective();
}


void MainJoinBattleTab::HostBattle( Battle& battle )
{
	JoinBattle( battle );
}


void MainJoinBattleTab::LeaveCurrentBattle( bool /*called_from_join*/ )
{
    PreSwitchBattlePerspective();
	m_mm_opts_tab->SetBattle( 0 );
	m_opts_tab->SetBattle( 0 );
	m_map_tab->SetBattle( 0 );
	m_battle_tab->SetBattle( 0 );
	PostSwitchBattlePerspective();
}


void MainJoinBattleTab::BattleUserUpdated( User& user )
{
	try
	{
		GetBattleRoomTab().UpdateUser( user );
	} catch ( ... ) {}
	try
	{
		GetBattleMapTab().UpdateUser( user );
	} catch ( ... ) {}
}

void MainJoinBattleTab::ReloadPresetList()
{
	try
	{
		//GetBattleRoomTab().UpdatePresetList();
	} catch ( ... ) {}
	try
	{
		GetMMOptionsTab().UpdatePresetList();
	} catch ( ... ) {}

}


BattleRoomTab& MainJoinBattleTab::GetBattleRoomTab()
{
	ASSERT_EXCEPTION( m_battle_tab, _T( "m_battle_tab == 0" ) );
	return *m_battle_tab;
}


BattleMapTab& MainJoinBattleTab::GetBattleMapTab()
{
	ASSERT_EXCEPTION( m_map_tab, _T( "m_map_tab == 0" ) );
	return *m_map_tab;
}


BattleOptionsTab& MainJoinBattleTab::GetOptionsTab()
{
	ASSERT_EXCEPTION( m_opts_tab, _T( "m_opts_tab == 0" ) );
	return *m_opts_tab;
}


BattleroomMMOptionsTab<Battle>& MainJoinBattleTab::GetMMOptionsTab()
{
	ASSERT_EXCEPTION( m_mm_opts_tab, _T( "m_mm_opts_tab == 0" ) );
	return *m_mm_opts_tab;
}

void MainJoinBattleTab::LoadPerspective( const wxString& perspective_name  )
{
    LoadNotebookPerspective( m_tabs, perspective_name );
}

void MainJoinBattleTab::SavePerspective( const wxString& perspective_name )
{
    SaveNotebookPerspective( m_tabs, perspective_name );
}

void MainJoinBattleTab::PostSwitchBattlePerspective( )
{
    ui().mw().LoadPerspectives( );
}

void MainJoinBattleTab::PreSwitchBattlePerspective( )
{
    ui().mw().SavePerspectives(  );
}

bool MainJoinBattleTab::UseBattlePerspective()
{
    return GetBattleRoomTab().GetBattle() != 0;
}

void MainJoinBattleTab::FocusBattleRoomTab()
{
	m_tabs->SetSelection( 0 );
}

void MainJoinBattleTab::OnUpdate()
{
//	m_battle_tab->OnUpdate();
}
