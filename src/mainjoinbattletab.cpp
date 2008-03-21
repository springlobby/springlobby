/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: MainJoinBattleTab
//

#include <wx/icon.h>
#include <wx/intl.h>
#include <wx/notebook.h>
#include <wx/imaglist.h>
#include <wx/sizer.h>
#include <wx/listbook.h>
#include <stdexcept>

#include "battle.h"
#include "mainjoinbattletab.h"
#include "battlelisttab.h"
#include "battleroomtab.h"
#include "battlemaptab.h"
#include "battleoptionstab.h"
#include "utils.h"
#include "battleroommmoptionstab.h"

#include "images/battle_list.xpm"
#include "images/battle.xpm"
#include "images/battle_map.xpm"
#include "images/battle_settings.xpm"

#include "ui.h"


MainJoinBattleTab::MainJoinBattleTab( wxWindow* parent, Ui& ui ) : wxPanel( parent, -1 ),m_battle_tab(0),m_map_tab(0),m_opts_tab(0),m_ui(ui)
{
  m_main_sizer = new wxBoxSizer( wxVERTICAL );
  m_tabs = new wxNotebook( this, BATTLE_TABS, wxDefaultPosition, wxDefaultSize, wxLB_TOP );

  m_imagelist = new wxImageList( 12, 12 );
  m_imagelist->Add( wxIcon(battle_list_xpm) );
  m_imagelist->Add( wxIcon(battle_xpm) );
  m_imagelist->Add( wxIcon(battle_map_xpm) );
  m_imagelist->Add( wxIcon(battle_settings_xpm) );

  m_tabs->AssignImageList( m_imagelist );

  m_list_tab = new BattleListTab( m_tabs, m_ui );
  m_tabs->AddPage( m_list_tab, _("Battle list"), true, 0 );

  m_main_sizer->Add( m_tabs, 1, wxEXPAND );

  SetSizer( m_main_sizer );
  Layout();
}


MainJoinBattleTab::~MainJoinBattleTab()
{

}


Battle* MainJoinBattleTab::GetCurrentBattle()
{
  if ( m_battle_tab )
    return &m_battle_tab->GetBattle();

  return 0;
}


ChatPanel* MainJoinBattleTab::GetActiveChatPanel()
{
  if ( m_tabs->GetSelection() == 1 ) {
    if ( m_battle_tab != 0 ) return &m_battle_tab->GetChatPanel();
  }
  return 0;
}

void MainJoinBattleTab::ReloadMMoptTab()
{
	int curPage = m_tabs->GetSelection();
	m_tabs->DeletePage (4);
	m_mm_opts_tab = 0;
	m_battle_tab->GetBattle().CustomBattleOptions()->loadMapOptions(m_battle_tab->GetBattle().GetMapName());
	m_mm_opts_tab = new BattleroomMMOptionsTab(m_battle_tab->GetBattle(), m_tabs);
	//m_mm_opts_tab.
	m_tabs->InsertPage( 4, m_mm_opts_tab, _("Map/Mod Options"), false );
	if (curPage == 4)
		m_tabs->SetSelection(curPage);
}

//void MainJoinBattleTab::UpdateCurrentBattle()
void MainJoinBattleTab::UpdateCurrentBattle( bool MapChanged, bool UpdateRestrictions )
{
  if ( m_battle_tab ) {
    m_battle_tab->UpdateBattleInfo( MapChanged );
    if ( UpdateRestrictions ) m_battle_tab->UpdateBattleInfo( wxString::Format(_T("%d_restrictions"), EngineOption ) );
  }
  if ( m_map_tab ) {
    m_map_tab->Update();
  }
  if ( m_opts_tab )
    if ( UpdateRestrictions )
        m_opts_tab->ReloadRestrictions();
  if ( m_mm_opts_tab ){
	 if ( !m_battle_tab->GetBattle().IsFounderMe() )
	  {
	    if ( MapChanged )
	    {
	      m_mm_opts_tab->OnRefreshControls(MapOption);
	    }
	  }
  }
}

void MainJoinBattleTab::UpdateCurrentBattle( const wxString& Tag )
{
  if ( m_battle_tab ) {
    m_battle_tab->UpdateBattleInfo( Tag );
  }

  if ( m_map_tab ) {
    m_map_tab->Update( Tag );
  }

  if ( m_opts_tab ) {
    m_opts_tab->UpdateBattle( Tag );
  }

  if ( m_mm_opts_tab ){
	  if ( !m_battle_tab->GetBattle().IsFounderMe() )
      m_mm_opts_tab->UpdateOptControls( Tag );
  }
}


BattleListTab& MainJoinBattleTab::GetBattleListTab()
{
  ASSERT_LOGIC( m_list_tab != 0, _T("m_list_tab = 0") );
  return *m_list_tab;
}


void MainJoinBattleTab::JoinBattle( Battle& battle )
{
  LeaveCurrentBattle();

  m_battle_tab = new BattleRoomTab( m_tabs, m_ui, battle );
  m_map_tab = new BattleMapTab( m_tabs, m_ui, battle );
  m_opts_tab = new BattleOptionsTab( m_tabs, m_ui, battle, false );
  m_mm_opts_tab = new BattleroomMMOptionsTab( battle, m_tabs);
  m_tabs->InsertPage( 1, m_battle_tab, _("Battleroom"), true, 1 );
  m_tabs->InsertPage( 2, m_map_tab, _("Map"), false, 2 );
  m_tabs->InsertPage( 3, m_opts_tab, _("Options"), false, 3 );
  m_tabs->InsertPage( 4, m_mm_opts_tab, _("Map/Mod Options"), false, 3 );
  #ifdef __WXMSW__
    Refresh(); /// this is needed to avoid a weird frame overlay glitch in windows
  #endif
}


void MainJoinBattleTab::HostBattle( Battle& battle )
{
  JoinBattle( battle );
}


void MainJoinBattleTab::LeaveCurrentBattle()
{
	if ( m_mm_opts_tab ) {
	    m_tabs->DeletePage( 4 );
	    m_mm_opts_tab = 0;
	  }
if ( m_opts_tab ) {
    m_tabs->DeletePage( 3 );
    m_opts_tab = 0;
  }
  if ( m_map_tab ) {
    m_tabs->DeletePage( 2 );
    m_map_tab = 0;
  }
  if ( m_battle_tab ) {
    m_tabs->DeletePage( 1 );
    m_battle_tab = 0;
  }


}


void MainJoinBattleTab::BattleUserUpdated( User& user )
{
  ASSERT_LOGIC( m_battle_tab != 0, _T("m_battle_tab = 0") );
  ASSERT_LOGIC( m_map_tab != 0, _T("m_map_tab = 0") );
  m_battle_tab->UpdateUser( user );
  m_map_tab->UpdateUser( user );
}


void MainJoinBattleTab::OnUnitSyncReloaded()
{
  wxLogDebugFunc( _T("") );
  GetBattleListTab().OnUnitSyncReloaded();
  wxLogMessage( _T("Battle list tab reloaded") );
  if ( GetBattleRoomTab() ) {
    wxLogMessage( _T("Reloading battleroom") );
    GetBattleRoomTab()->OnUnitSyncReloaded();
    wxLogMessage( _T("Battleroom reloaded") );
  }
  if ( GetBattleMapTab() ) {
    wxLogMessage( _T("Reloading battlemap") );
    GetBattleMapTab()->OnUnitSyncReloaded();
    wxLogMessage( _T("Battlemap reloaded") );
  }
}

