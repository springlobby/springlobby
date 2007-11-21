/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: BattleRoomTab
//

#include <wx/splitter.h>
#include <wx/intl.h>
#include <wx/combobox.h>
#include <wx/stattext.h>
#include <wx/statline.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/msgdlg.h>
#include <wx/settings.h>
#include <wx/colordlg.h>
#include <wx/colour.h>
#include <stdexcept>

#include "battleroomtab.h"
#include "ui.h"
#include "iunitsync.h"
#include "user.h"
#include "battle.h"
#include "utils.h"
#include "battleroomlistctrl.h"
#include "chatpanel.h"
#include "mapctrl.h"
#include "uiutils.h"
#include "addbotdialog.h"
#include "server.h"
#include "iconimagelist.h"


#define Opt_Pos_Size 0
#define Opt_Pos_Windspeed 1
#define Opt_Pos_Tidal 2

#define Opt_Pos_Startpos 4
#define Opt_Pos_Gameend 5
#define Opt_Pos_LimitDgun 6
#define Opt_Pos_Startmetal 7
#define Opt_Pos_Startenergy 8
#define Opt_Pos_Maxunits 9
#define Opt_Pos_Restrictions 10

BEGIN_EVENT_TABLE(BattleRoomTab, wxPanel)

  EVT_BUTTON ( BROOM_START, BattleRoomTab::OnStart )
  EVT_BUTTON ( BROOM_LEAVE, BattleRoomTab::OnLeave )
  EVT_BUTTON ( BROOM_ADDBOT, BattleRoomTab::OnAddBot )

  EVT_CHECKBOX( BROOM_IMREADY, BattleRoomTab::OnImReady )
  EVT_CHECKBOX( BROOM_LOCK, BattleRoomTab::OnLock )
  EVT_CHECKBOX( BROOM_SPEC, BattleRoomTab::OnImSpec )
  EVT_COMBOBOX( BROOM_TEAMSEL, BattleRoomTab::OnTeamSel )
  EVT_COMBOBOX( BROOM_ALLYSEL, BattleRoomTab::OnAllySel )
  EVT_BUTTON( BROOM_COLOURSEL, BattleRoomTab::OnColourSel )
  EVT_COMBOBOX( BROOM_SIDESEL, BattleRoomTab::OnSideSel )

END_EVENT_TABLE()

const wxString team_choices[] = { _T("1"), _T("2"), _T("3"), _T("4"), _T("5"), _T("6"), _T("7"), _T("8"), _T("9"), _T("10"), _T("11"), _T("12"), _T("13"), _T("14"), _T("15"), _T("16") };


BattleRoomTab::BattleRoomTab( wxWindow* parent, Ui& ui, Battle& battle ) : wxPanel( parent, -1 ),m_ui(ui), m_battle(battle)
{
  // Create all widgets
  m_splitter = new wxSplitterWindow( this, -1, wxDefaultPosition, wxSize(100, 60) );

  UserBattleStatus& myself = m_battle.GetMe().BattleStatus();

  m_player_panel = new wxPanel( m_splitter , -1 );
  m_team_sel = new wxComboBox( m_player_panel, BROOM_TEAMSEL, _T("1"), wxDefaultPosition, wxSize(50,CONTROL_HEIGHT), 16, team_choices );
  m_ally_sel = new wxComboBox( m_player_panel, BROOM_ALLYSEL, _T("1"), wxDefaultPosition, wxSize(50,CONTROL_HEIGHT), 16, team_choices );
  m_color_sel = new wxBitmapButton( m_player_panel, BROOM_COLOURSEL, icons().GetBitmap( icons().GetColourIcon( myself.team ) ) , wxDefaultPosition, wxSize(-1,CONTROL_HEIGHT) );
  m_side_sel = new wxComboBox( m_player_panel, BROOM_SIDESEL, _T(""), wxDefaultPosition, wxSize(80,CONTROL_HEIGHT) );

  usync()->SetCurrentMod( STD_STRING(m_battle.GetModName()) );
  for ( int i = 0; i < usync()->GetSideCount(); i++ ) {
    m_side_sel->Append( WX_STRING(usync()->GetSideName( i )) );
  }

  m_team_lbl = new wxStaticText( m_player_panel, -1, _("Team") );
  m_ally_lbl = new wxStaticText( m_player_panel, -1, _("Ally") );
  m_color_lbl = new wxStaticText( m_player_panel, -1, _("Color") );
  m_side_lbl = new wxStaticText( m_player_panel, -1, _("Side") );

  m_map_lbl = new wxStaticText( this, -1, RefineMapname( battle.GetMapName() ) );
  m_size_lbl = new wxStaticText( this, -1, _("") );
  m_wind_lbl = new wxStaticText( this, -1, _("") );
  m_tidal_lbl = new wxStaticText( this, -1, _("") );

  m_minimap = new MapCtrl( this, 162, &m_battle, m_ui, true, true, true, false );

  m_players = new BattleroomListCtrl( m_player_panel, battle );
  m_chat = new ChatPanel( m_splitter, m_ui, battle );

  m_command_line = new wxStaticLine( this, -1, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );

  m_leave_btn = new wxButton( this, BROOM_LEAVE, _("Leave"), wxDefaultPosition, wxSize(-1,CONTROL_HEIGHT) );
  m_start_btn = new wxButton( this, BROOM_START, _("Start"), wxDefaultPosition, wxSize(-1,CONTROL_HEIGHT) );
  m_addbot_btn = new wxButton( this, BROOM_ADDBOT, _("Add Bot..."), wxDefaultPosition, wxSize(-1,CONTROL_HEIGHT) );

  m_ready_chk = new wxCheckBox( this, BROOM_IMREADY, _("I'm ready"), wxDefaultPosition, wxSize(-1,CONTROL_HEIGHT) );
  m_lock_chk = new wxCheckBox( this, BROOM_LOCK, _("Locked"), wxDefaultPosition, wxSize(-1,CONTROL_HEIGHT) );
  m_spec_chk = new wxCheckBox( m_player_panel, BROOM_SPEC, _("Spectator"), wxDefaultPosition, wxSize(-1,CONTROL_HEIGHT) );

  m_opts_list = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_NO_HEADER|wxLC_REPORT );
  m_opts_list->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
  m_opts_list->SetFont( wxFont( 8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_LIGHT ) );
  wxListItem col;

  col.SetText( _("Option") );
  m_opts_list->InsertColumn( 0, col );
  col.SetText( _("Value") );
  m_opts_list->InsertColumn( 1, col );
  m_opts_list->SetColumnWidth( 0, 85 );
  m_opts_list->SetColumnWidth( 1, 60 );

  m_opts_list->InsertItem( Opt_Pos_Size, _("Size") );
  m_opts_list->InsertItem( Opt_Pos_Windspeed, _("Windspeed") );
  m_opts_list->InsertItem( Opt_Pos_Tidal, _("Tidal strength") );

  m_opts_list->InsertItem( 3, wxEmptyString );

  m_opts_list->InsertItem( Opt_Pos_Startpos, _("Startpos") );
  m_opts_list->InsertItem( Opt_Pos_Gameend, _("Game end") );
  m_opts_list->InsertItem( Opt_Pos_LimitDgun, _("Limit D-gun") );
  m_opts_list->InsertItem( Opt_Pos_Startmetal, _("Start metal") );
  m_opts_list->InsertItem( Opt_Pos_Startenergy, _("Start energy") );
  m_opts_list->InsertItem( Opt_Pos_Maxunits, _("Max units") );
  m_opts_list->InsertItem( Opt_Pos_Restrictions, _("Restrictions") );

  // Create Sizers
  m_players_sizer = new wxBoxSizer( wxVERTICAL );
  m_player_sett_sizer = new wxBoxSizer( wxHORIZONTAL );
  m_info_sizer = new wxBoxSizer( wxVERTICAL );
  m_top_sizer = new wxBoxSizer( wxHORIZONTAL );
  m_buttons_sizer = new wxBoxSizer( wxHORIZONTAL );
  //m_info1_sizer = new wxBoxSizer( wxHORIZONTAL );
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

  //m_info1_sizer->Add( m_wind_lbl, 1, wxEXPAND );
  //m_info1_sizer->Add( m_size_lbl, 1, wxEXPAND );

  m_info_sizer->Add( m_minimap, 0, wxEXPAND );
  m_info_sizer->Add( m_map_lbl, 0, wxEXPAND );
  //m_info_sizer->Add( m_info1_sizer, 0, wxEXPAND );
  //m_info_sizer->Add( m_tidal_lbl, 0, wxEXPAND );
  m_info_sizer->Add( m_opts_list, 1, wxEXPAND | wxTOP, 4 );


  m_top_sizer->Add( m_splitter, 1, wxEXPAND | wxALL, 2 );
  m_top_sizer->Add( m_info_sizer, 0, wxEXPAND | wxALL, 2 );

  m_buttons_sizer->Add( m_leave_btn, 0, wxEXPAND | wxALL, 2 );
  m_buttons_sizer->AddStretchSpacer();
  m_buttons_sizer->Add( m_addbot_btn, 0, wxEXPAND | wxALL, 2 );
  m_buttons_sizer->Add( m_lock_chk, 0, wxEXPAND | wxALL, 2 );
  m_buttons_sizer->Add( m_ready_chk, 0, wxEXPAND | wxALL, 2 );
  m_buttons_sizer->Add( m_start_btn, 0, wxEXPAND | wxALL, 2 );

  m_main_sizer->Add( m_top_sizer, 1, wxEXPAND );
  m_main_sizer->Add( m_command_line, 0, wxEXPAND );
  m_main_sizer->Add( m_buttons_sizer, 0, wxEXPAND );

  SetSizer( m_main_sizer );
  Layout();

  UpdateBattleInfo();

  m_splitter->SetMinimumPaneSize( 100 );
  m_splitter->SetSashPosition( 200 );

  for ( user_map_t::size_type i = 0; i < battle.GetNumUsers(); i++ ) {
    m_players->AddUser( battle.GetUser( i ) );
  }

  if ( !IsHosted() ) {
    m_start_btn->Disable();
    m_lock_chk->Disable();
  } else {
    m_battle.SetImReady ( true );
    m_ready_chk->Disable();
  }

}


BattleRoomTab::~BattleRoomTab()
{

}


bool BattleRoomTab::IsHosted()
{
  return m_battle.IsFounderMe();
}

wxString _GetStartPosStr( StartType t )
{
  switch ( t ) {
    case ST_Fixed: return _T("Fixed");
    case ST_Random: return _T("Random");
    case ST_Choose: return _T("Boxes");
    case ST_Pick: return _T("Pick");
    default: return _T("?");
  };
}


wxString _GetGameTypeStr( GameType t )
{
  switch ( t ) {
    case GT_ComContinue: return _T("Continue");
    case GT_ComEnds: return _T("End");
    case GT_Lineage: return _T("Lineage");
    default: return _T("?");
  };
}


void BattleRoomTab::UpdateBattleInfo()
{
  try {
    ASSERT_RUNTIME( m_battle.MapExists(), "Map does not exist." );
    UnitSyncMap map = m_battle.Map();
    m_map_lbl->SetLabel( RefineMapname( WX_STRING(map.name) ) );
    m_opts_list->SetItem( Opt_Pos_Size, 1, wxString::Format( _T("%.0fx%.0f"), map.info.width/512.0, map.info.height/512.0 ) );
    m_opts_list->SetItem( Opt_Pos_Windspeed, 1, wxString::Format( _T("%d-%d"), map.info.minWind, map.info.maxWind) );
    m_opts_list->SetItem( Opt_Pos_Tidal, 1, wxString::Format( _T("%d"), map.info.tidalStrength) );
    //    m_opts_list->SetItem( 0, 1,  );
  } catch (...) {
    m_map_lbl->SetLabel( m_battle.GetMapName() );
    m_opts_list->SetItem( Opt_Pos_Size, 1, _T("?x?") );
    m_opts_list->SetItem( Opt_Pos_Windspeed, 1, _T("?-?") );
    m_opts_list->SetItem( Opt_Pos_Tidal, 1, _T("?") );
  }

  m_opts_list->SetItem( Opt_Pos_Startpos, 1, _GetStartPosStr( m_battle.GetStartType() ) );
  m_opts_list->SetItem( Opt_Pos_Gameend, 1, _GetGameTypeStr( m_battle.GetGameType() ) );
  m_opts_list->SetItem( Opt_Pos_LimitDgun, 1, bool2yn( m_battle.LimitDGun() ) );
  m_opts_list->SetItem( Opt_Pos_Startmetal, 1, wxString::Format( _T("%d"), m_battle.GetStartMetal() ) );
  m_opts_list->SetItem( Opt_Pos_Startenergy, 1, wxString::Format( _T("%d"), m_battle.GetStartEnergy() ) );
  m_opts_list->SetItem( Opt_Pos_Maxunits, 1, wxString::Format( _T("%d"), m_battle.GetMaxUnits() ) );
  m_opts_list->SetItem( Opt_Pos_Restrictions, 1, bool2yn( m_battle.GetNumDisabledUnits() > 0 ) );

  m_lock_chk->SetValue( m_battle.IsLocked() );
  m_minimap->UpdateMinimap();
}


BattleroomListCtrl& BattleRoomTab::GetPlayersListCtrl()
{
  ASSERT_LOGIC( m_players != 0, "m_players = 0" );
  return *m_players;
}


void BattleRoomTab::UpdateUser( User& user )
{
  m_players->UpdateUser( user );
  if ( &user != &m_battle.GetMe() ) return;

  UserBattleStatus& bs = user.BattleStatus();
  m_team_sel->SetSelection( bs.team );
  m_ally_sel->SetSelection( bs.ally );
  m_side_sel->SetSelection( bs.side );
  m_spec_chk->SetValue( bs.spectator );
  if ( bs.spectator ) {
    m_ready_chk->SetValue ( true );
    m_ready_chk->Disable();
  } else {
    if ( !IsHosted() ) m_ready_chk->Enable();
    m_ready_chk->SetValue( bs.ready );
  }

  m_color_sel->SetBitmapLabel( icons().GetBitmap( icons().GetColourIcon( bs.team ) ) );

  m_minimap->UpdateMinimap();
}


Battle& BattleRoomTab::GetBattle()
{
  return m_battle;
}


ChatPanel& BattleRoomTab::GetChatPanel()
{
  ASSERT_LOGIC( m_chat != 0, "m_chat = 0" );
  return *m_chat;
}


void BattleRoomTab::OnStart( wxCommandEvent& event )
{
  if ( m_battle.HaveMultipleBotsInSameTeam() ) {
    wxMessageDialog dlg( this, _("You have one or more bots shring team, this is not possible."), _("Bot team sharing."), wxOK );
    dlg.ShowModal();
    return;
  }
  m_battle.GetMe().BattleStatus().ready = true;

  if ( !m_battle.IsEveryoneReady() ) {
    wxMessageDialog dlg1( this, _("Some players are not ready yet.\nRing these players?"), _("Not ready"), wxYES_NO );
    if ( dlg1.ShowModal() == wxID_YES ) {
      m_battle.RingNotReadyPlayers();
      return;
    }
    wxMessageDialog dlg2( this, _("Force start?"), _("Not ready"), wxYES_NO );
    if ( dlg2.ShowModal() == wxID_NO ) return;
  }
  m_ui.StartHostedBattle();
}


void BattleRoomTab::OnLeave( wxCommandEvent& event )
{
  m_battle.Leave();
}


void BattleRoomTab::OnAddBot( wxCommandEvent& event )
{
  AddBotDialog dlg( this, m_battle );
  if ( dlg.ShowModal() == wxID_OK ) {
    UserBattleStatus bs;
    bs.team = m_battle.GetFreeTeamNum( false );
    bs.ally = bs.team;
    bs.sync = SYNC_SYNCED;
    bs.spectator = false;
    bs.side = 0;
    bs.ready = true;
    bs.order = 0;
    bs.handicap = 0;
    m_battle.GetFreeColour( bs.color_r, bs.color_g, bs.color_b, false );
    m_ui.GetServer().AddBot( m_battle.GetBattleId(), STD_STRING(dlg.GetNick()), m_battle.GetMe().GetNick(), bs, STD_STRING(dlg.GetAI()) );
  }
}


void BattleRoomTab::OnImReady( wxCommandEvent& event )
{
  m_battle.SetImReady( m_ready_chk->GetValue() );
}


void BattleRoomTab::OnLock( wxCommandEvent& event )
{
  m_battle.SetIsLocked( m_lock_chk->GetValue() );
  m_battle.SendHostInfo( HI_Locked );
}


void BattleRoomTab::OnImSpec( wxCommandEvent& event )
{
  UserBattleStatus& bs = m_battle.GetMe().BattleStatus();
  bs.spectator = m_spec_chk->GetValue();
  //m_battle.GetMe().SetBattleStatus( bs );
  m_battle.SendMyBattleStatus();
}


void BattleRoomTab::OnTeamSel( wxCommandEvent& event )
{
  User& u = m_battle.GetMe();
  UserBattleStatus& bs = u.BattleStatus();
  m_team_sel->GetValue().ToULong( (unsigned long*)&bs.team );
  bs.team--;
  //u.SetBattleStatus( bs );
  m_battle.SendMyBattleStatus();
}


void BattleRoomTab::OnAllySel( wxCommandEvent& event )
{
  User& u = m_battle.GetMe();
  UserBattleStatus& bs = u.BattleStatus();
  m_ally_sel->GetValue().ToULong( (unsigned long*)&bs.ally );
  bs.ally--;
  //u.SetBattleStatus( bs );
  m_battle.SendMyBattleStatus();
}


void BattleRoomTab::OnColourSel( wxCommandEvent& event )
{
  User& u = m_battle.GetMe();
  UserBattleStatus& bs = u.BattleStatus();
  wxColour CurrentColour;
  CurrentColour.Set( bs.color_r, bs.color_g, bs.color_b );
  CurrentColour = wxGetColourFromUser(this, CurrentColour);
  if ( !CurrentColour.IsColourOk() ) return;
  bs.color_r = CurrentColour.Red();
  bs.color_g = CurrentColour.Green();
  bs.color_b = CurrentColour.Blue();
  //u.SetBattleStatus( bs );
  m_battle.SendMyBattleStatus();
}


void BattleRoomTab::OnSideSel( wxCommandEvent& event )
{
  User& u = m_battle.GetMe();
  UserBattleStatus& bs = u.BattleStatus();
  bs.side = m_side_sel->GetSelection();
  //u.SetBattleStatus( bs );
  m_battle.SendMyBattleStatus();
}


void BattleRoomTab::OnUserJoined( User& user )
{
  m_chat->Joined( user );
  m_players->AddUser( user );
  if ( &user == &m_battle.GetMe() ) {
    m_players->SetItemState( m_players->GetUserIndex( user ), wxLIST_MASK_STATE, wxLIST_STATE_SELECTED );
  }
}


void BattleRoomTab::OnUserLeft( User& user )
{
  m_chat->Parted( user, wxEmptyString );
  m_players->RemoveUser( user );
}


void BattleRoomTab::OnBotAdded( BattleBot& bot )
{
  m_players->AddBot( bot );
}


void BattleRoomTab::OnBotRemoved( BattleBot& bot )
{
  m_players->RemoveBot( bot );
}


void BattleRoomTab::OnBotUpdated( BattleBot& bot )
{
  m_players->UpdateBot( bot );
}


void BattleRoomTab::OnUnitSyncReloaded()
{
  m_minimap->UpdateMinimap();
  UpdateBattleInfo();
  m_battle.SendMyBattleStatus(); // This should reset sync status.
}

