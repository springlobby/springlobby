//
// Class: MainJoinBattleTab
//

#include <wx/icon.h>
#include "mainjoinbattletab.h"

#include "images/battle_list.xpm"
#include "images/battle.xpm"
#include "images/battle_map.xpm"
#include "images/battle_settings.xpm"

MainJoinBattleTab::MainJoinBattleTab( wxWindow* parent, Ui& ui ) : wxPanel( parent, -1 ),m_battle_tab(NULL),m_ui(ui)
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

BattleListTab& MainJoinBattleTab::GetBattleListTab()
{
  assert( m_list_tab != NULL );
  return *m_list_tab;
}

void MainJoinBattleTab::JoinBattle( Battle& battle )
{
  //if ( m_battle_tab != NULL ) delete m_battle_tab;
  LeaveCurrentBattle();
  m_battle_tab = new BattleRoomTab( m_tabs, m_ui, battle );
  m_tabs->InsertPage( 1, m_battle_tab, _("Battleroom"), true, 1 );
}


void MainJoinBattleTab::LeaveCurrentBattle()
{
  if ( m_battle_tab != NULL ) {
    //delete m_battle_tab;
    m_tabs->DeletePage( 1 );
    m_battle_tab = NULL;
  }
}


void MainJoinBattleTab::BattleUserUpdated( User& user )
{
  assert( m_battle_tab != NULL );
  m_battle_tab->UpdateUser( user );
}

