//
// Class: BattleListTab
//

#include "battlelisttab.h"


BattleListTab::BattleListTab( wxWindow* parent ) : wxPanel( parent, -1 )
{
  m_battle_list = new BattleListCtrl( this );
  m_filter_text = new wxStaticText( this, -1, _T("Filter ") );
  m_filter_combo = new wxComboBox( this, -1, _T("Show all") );
  m_join_button = new wxButton( this, -1, _T("Join"), wxDefaultPosition, wxSize(80,28) );
  
  m_main_sizer = new wxBoxSizer( wxVERTICAL );
  m_tools_sizer = new wxBoxSizer( wxHORIZONTAL );
  
  m_tools_sizer->Add( m_filter_text, 0, wxEXPAND | wxTOP, 6 );
  m_tools_sizer->Add( m_filter_combo, 0, wxEXPAND );
  m_tools_sizer->AddStretchSpacer();
  m_tools_sizer->Add( m_join_button, 0, wxEXPAND );
  
  m_main_sizer->Add( m_battle_list, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 2 );
  m_main_sizer->Add( m_tools_sizer, 0, wxEXPAND | wxALL, 2 );
  
  SetSizer( m_main_sizer );
  Layout();
}


BattleListTab::~BattleListTab()
{
  
}


void BattleListTab::AddBattle( Battle& battle )
{
  m_battle_list->AddBattle( battle );
}


void BattleListTab::RemoveBattle( Battle& battle )
{
  m_battle_list->RemoveBattle( battle );
}


void BattleListTab::UpdateBattle( Battle& battle )
{
  m_battle_list->UpdateBattle( battle );
}


