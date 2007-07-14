//
// Class: BattleRoomTab
//

#include "battleroomtab.h"
#include "ui.h"

BEGIN_EVENT_TABLE(BattleRoomTab, wxPanel)

  EVT_BUTTON ( BROOM_LEAVE, BattleRoomTab::OnLeave )
  EVT_CHECKBOX( BROOM_IMREADY, BattleRoomTab::OnImReady )
  EVT_CHECKBOX( BROOM_SPEC, BattleRoomTab::OnImSpec )
  EVT_COMBOBOX( BROOM_TEAMSEL, BattleRoomTab::OnTeamSel )
  EVT_COMBOBOX( BROOM_ALLYSEL, BattleRoomTab::OnAllySel )
  EVT_COMBOBOX( BROOM_COLOURSEL, BattleRoomTab::OnColourSel )
  EVT_COMBOBOX( BROOM_SIDESEL, BattleRoomTab::OnSideSel )

END_EVENT_TABLE()


BattleRoomTab::BattleRoomTab( wxWindow* parent, Ui& ui, Battle& battle ) : wxPanel( parent, -1 ),m_ui(ui), m_battle(battle) 
{
  // Create all widgets
  m_splitter = new wxSplitterWindow( this, -1, wxDefaultPosition, wxSize(100, 60) );

  m_player_panel = new wxPanel( m_splitter , -1 );
  m_team_sel = new wxComboBox( m_player_panel, BROOM_TEAMSEL, _("1"), wxDefaultPosition, wxDefaultSize, 16, team_choices );
  m_ally_sel = new wxComboBox( m_player_panel, BROOM_ALLYSEL, _("1"), wxDefaultPosition, wxDefaultSize, 16, team_choices );
  m_color_sel = new wxComboBox( m_player_panel, BROOM_COLOURSEL, _("black"), wxDefaultPosition, wxDefaultSize, 16, colour_choices );
  m_side_sel = new wxComboBox( m_player_panel, BROOM_SIDESEL );

  for ( int i = 0; i < usync().GetSideCount( battle.opts().modname ); i++ ) {
    m_side_sel->Append( WX_STRING(usync().GetSideName( battle.opts().modname, i )) );
  }

  m_team_lbl = new wxStaticText( m_player_panel, -1, _("Team") );
  m_ally_lbl = new wxStaticText( m_player_panel, -1, _("Ally") );
  m_color_lbl = new wxStaticText( m_player_panel, -1, _("Color") );
  m_side_lbl = new wxStaticText( m_player_panel, -1, _("Side") );
  m_map_lbl = new wxStaticText( this, -1, _T("$Mapname") );

  m_players = new BattleroomListCtrl( m_player_panel );
  m_chat = new ChatPanel( m_splitter, m_ui, battle );

  m_command_line = new wxStaticLine( this, -1, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );

  m_leave_btn = new wxButton( this, BROOM_LEAVE, _("Leave"), wxDefaultPosition, wxSize(80,28) );
  m_start_btn = new wxButton( this, -1, _("Start"), wxDefaultPosition, wxSize(80,28) );

  m_ready_chk = new wxCheckBox( this, BROOM_IMREADY, _("I'm ready"), wxDefaultPosition, wxSize(80,28) );
  m_spec_chk = new wxCheckBox( m_player_panel, BROOM_SPEC, _("Spectator"), wxDefaultPosition, wxSize(80,28) );
  
  // Create Sizers
  m_players_sizer = new wxBoxSizer( wxVERTICAL );
  m_player_sett_sizer = new wxBoxSizer( wxHORIZONTAL );
  m_info_sizer = new wxBoxSizer( wxVERTICAL );
  m_top_sizer = new wxBoxSizer( wxHORIZONTAL );
  m_buttons_sizer = new wxBoxSizer( wxHORIZONTAL );
  m_main_sizer = new wxBoxSizer( wxVERTICAL );

  // Put widgets in place
  
  m_player_sett_sizer->Add( m_team_lbl, 0, wxEXPAND | wxALL, 2 );
  m_player_sett_sizer->Add( m_team_sel, 0, wxEXPAND | wxALL, 2 );
  m_player_sett_sizer->Add( m_ally_lbl, 0, wxEXPAND | wxALL, 2 );
  m_player_sett_sizer->Add( m_ally_sel, 0, wxEXPAND | wxALL, 2 );
  m_player_sett_sizer->Add( m_color_lbl, 0, wxEXPAND | wxALL, 2 );
  m_player_sett_sizer->Add( m_color_sel, 0, wxEXPAND | wxALL, 2 );
  m_player_sett_sizer->Add( m_side_lbl, 0, wxEXPAND | wxALL, 2 );
  m_player_sett_sizer->Add( m_side_sel, 0, wxEXPAND | wxALL, 2 );
  m_player_sett_sizer->Add( m_spec_chk, 0, wxEXPAND | wxALL, 2 );
  
  m_players_sizer->Add( m_players, 1, wxEXPAND );
  m_players_sizer->Add( m_player_sett_sizer, 0, wxEXPAND );
  
  m_player_panel->SetSizer( m_players_sizer );

  m_splitter->SplitHorizontally( m_player_panel, m_chat, 50 );
    
  m_info_sizer->Add( m_map_lbl, 0, wxEXPAND | wxALL, 2 );
  
  m_top_sizer->Add( m_splitter, 1, wxEXPAND | wxALL, 2 );
  m_top_sizer->Add( m_info_sizer, 0, wxEXPAND | wxALL, 2 );
  
  m_buttons_sizer->Add( m_leave_btn, 0, wxEXPAND | wxALL, 2 );
  m_buttons_sizer->AddStretchSpacer();
  m_buttons_sizer->Add( m_ready_chk, 0, wxEXPAND | wxALL, 2 );
  m_buttons_sizer->Add( m_start_btn, 0, wxEXPAND | wxALL, 2 );

  m_main_sizer->Add( m_top_sizer, 1, wxEXPAND );
  m_main_sizer->Add( m_command_line, 0, wxEXPAND );
  m_main_sizer->Add( m_buttons_sizer, 0, wxEXPAND );
  
  SetSizer( m_main_sizer );
  Layout();
  
  m_splitter->SetMinimumPaneSize( 100 );
  m_splitter->SetSashPosition( 200 );
  
  for ( int i = 0; i < battle.GetNumUsers(); i++ ) {
    m_players->AddUser( battle.GetUser( i ) );
  }
  
}


BattleRoomTab::~BattleRoomTab()
{
  
}


void BattleRoomTab::UpdateUser( User& user )
{
  m_players->UpdateUser( user );
  if ( &user != &m_battle.GetMe() ) return;

  UserBattleStatus bs = user.GetBattleStatus();
  m_team_sel->SetSelection( bs.team );
  m_ally_sel->SetSelection( bs.ally );
  m_side_sel->SetSelection( bs.side );
  m_spec_chk->SetValue( bs.spectator );
}


void BattleRoomTab::OnLeave( wxCommandEvent& event )
{
  m_battle.Leave();
}


void BattleRoomTab::OnImReady( wxCommandEvent& event )
{
  m_battle.SetImReady( m_ready_chk->GetValue() );
}


void BattleRoomTab::OnImSpec( wxCommandEvent& event )
{
  UserBattleStatus bs = m_battle.GetMe().GetBattleStatus();
  bs.spectator = m_spec_chk->GetValue();
  m_battle.GetMe().SetBattleStatus( bs );
  m_battle.SendMyBattleStatus();
}


void BattleRoomTab::OnTeamSel( wxCommandEvent& event )
{
  User& u = m_battle.GetMe();
  UserBattleStatus bs = u.GetBattleStatus();
  m_team_sel->GetValue().ToULong( (unsigned long*)&bs.team );
  bs.team--;
  u.SetBattleStatus( bs );
  m_battle.SendMyBattleStatus();
}


void BattleRoomTab::OnAllySel( wxCommandEvent& event )
{
  User& u = m_battle.GetMe();
  UserBattleStatus bs = u.GetBattleStatus();
  m_ally_sel->GetValue().ToULong( (unsigned long*)&bs.ally );
  bs.ally--;
  u.SetBattleStatus( bs );
  m_battle.SendMyBattleStatus();
}


void BattleRoomTab::OnColourSel( wxCommandEvent& event )
{
  User& u = m_battle.GetMe();
  UserBattleStatus bs = u.GetBattleStatus();
  int i = m_color_sel->GetSelection();
  bs.color_r = colour_values[i][0];
  bs.color_g = colour_values[i][1];
  bs.color_b = colour_values[i][2];
  u.SetBattleStatus( bs );
  m_battle.SendMyBattleStatus();
}


void BattleRoomTab::OnSideSel( wxCommandEvent& event )
{
  User& u = m_battle.GetMe();
  UserBattleStatus bs = u.GetBattleStatus();
  bs.side = m_side_sel->GetSelection();
  u.SetBattleStatus( bs );
  m_battle.SendMyBattleStatus();
}


void BattleRoomTab::OnUserJoined( User& user )
{
  m_players->AddUser( user );
}


void BattleRoomTab::OnUserLeft( User& user )
{
  m_players->RemoveUser( user );
}


