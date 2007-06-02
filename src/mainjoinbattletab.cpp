//
// Class: MainJoinBattleTab
//

#include "mainjoinbattletab.h"


MainJoinBattleTab::MainJoinBattleTab( wxWindow* parent ) : wxPanel( parent, -1 )
{
  m_main_sizer = new wxBoxSizer( wxVERTICAL );
  m_tabs = new wxNotebook( this, BATTLE_TABS, wxDefaultPosition, wxDefaultSize, wxLB_TOP );
  
  m_list_tab = new BattleListTab( m_tabs );
  m_tabs->AddPage( m_list_tab, _T("Battle list"), true, -1 );

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




