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
#include "settings++/custom_dialogs.h"
#include "autobalancedialog.h"
#include "settings.h"

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

  EVT_BUTTON ( BROOM_BALANCE, BattleRoomTab::OnBalance )
  EVT_BUTTON ( BROOM_FIXCOLOURS, BattleRoomTab::OnFixColours )

END_EVENT_TABLE()

//TODO make this more flexible
const wxString team_choices[] = { _T("1"), _T("2"), _T("3"), _T("4"), _T("5"), _T("6"), _T("7"), _T("8"), _T("9"), _T("10"), _T("11"), _T("12"), _T("13"), _T("14"), _T("15"), _T("16") };


BattleRoomTab::BattleRoomTab( wxWindow* parent, Ui& ui, Battle& battle ) : wxPanel( parent, -1 ),m_ui(ui), m_battle(battle)
{
  // Create all widgets
  m_splitter = new wxSplitterWindow( this, -1, wxDefaultPosition, wxSize(100, 60) );

  UserBattleStatus& myself = m_battle.GetMe().BattleStatus();

  m_player_panel = new wxPanel( m_splitter , -1 );
  m_team_sel = new wxComboBox( m_player_panel, BROOM_TEAMSEL, _T("1"), wxDefaultPosition, wxSize(50,CONTROL_HEIGHT), 16, team_choices );
  m_team_sel->SetToolTip(_T("Players with the same teamnumber share control of their units"));
  m_ally_sel = new wxComboBox( m_player_panel, BROOM_ALLYSEL, _T("1"), wxDefaultPosition, wxSize(50,CONTROL_HEIGHT), 16, team_choices );
  m_ally_sel->SetToolTip(_T("Players with same allynumber can achieve victory together"));
  m_color_sel = new wxBitmapButton( m_player_panel, BROOM_COLOURSEL, icons().GetBitmap( icons().GetColourIcon( myself.team ) ) , wxDefaultPosition, wxSize(-1,CONTROL_HEIGHT) );
  m_color_sel->SetToolTip(_T("You might want to select a color dissimilar to others'\n so you can tell you units apart ingame."));
  m_side_sel = new wxComboBox( m_player_panel, BROOM_SIDESEL, _T(""), wxDefaultPosition, wxSize(80,CONTROL_HEIGHT) );
  m_side_sel->SetToolTip(_T("Select your faction"));

  try {
    for ( int i = 0; i < usync()->GetSideCount( m_battle.GetHostModName() ); i++ ) {
      m_side_sel->Append( usync()->GetSideName( m_battle.GetHostModName(), i ) );
    }
  } catch (...) {}

  m_team_lbl = new wxStaticText( m_player_panel, -1, _("Team") );
  m_ally_lbl = new wxStaticText( m_player_panel, -1, _("Ally") );
  m_color_lbl = new wxStaticText( m_player_panel, -1, _("Color") );
  m_side_lbl = new wxStaticText( m_player_panel, -1, _("Side") );

  m_map_lbl = new wxStaticText( this, -1, RefineMapname( battle.GetHostMapName() ) );
  m_size_lbl = new wxStaticText( this, -1, _T("") );
  m_wind_lbl = new wxStaticText( this, -1, _T("") );
  m_tidal_lbl = new wxStaticText( this, -1, _T("") );

  m_minimap = new MapCtrl( this, 162, &m_battle, m_ui, true, true, true, false );
  m_minimap->SetToolTip(_T("A small version of the selected map.\n "
		  					"You can see the starting positions,\n"
		  					"and also (if set) starting boxes"));

  m_players = new BattleroomListCtrl( m_player_panel, battle, m_ui );
  m_chat = new ChatPanel( m_splitter, m_ui, battle );
  m_chat->SetToolTip(_T("This chat is exlusivly for participants of this battle"));

  m_command_line = new wxStaticLine( this, -1, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );

  m_leave_btn = new wxButton( this, BROOM_LEAVE, _("Leave"), wxDefaultPosition, wxSize(-1,CONTROL_HEIGHT) );
  m_leave_btn->SetToolTip(_T("Don't want to play? Click here"));
  m_start_btn = new wxButton( this, BROOM_START, _("Start"), wxDefaultPosition, wxSize(-1,CONTROL_HEIGHT) );
  m_start_btn->SetToolTip(_T("Only the host can do this if all players are ready."));
  m_addbot_btn = new wxButton( this, BROOM_ADDBOT, _("Add Bot..."), wxDefaultPosition, wxSize(-1,CONTROL_HEIGHT) );
  m_addbot_btn->SetToolTip(_T("Gives you a selection of available bots you can add"));


  m_fix_colours_btn = new wxButton( this, BROOM_FIXCOLOURS, _("Fix colours"), wxDefaultPosition, wxSize(-1,CONTROL_HEIGHT) );
  m_fix_colours_btn->SetToolTip(_T("Automatically makes player colors be different"));

  m_balance_btn = new wxButton( this, BROOM_BALANCE, _("Balance"), wxDefaultPosition, wxSize(-1,CONTROL_HEIGHT) );
  m_balance_btn->SetToolTip(_T("Automatically banalce players into two or more alliances."));

  m_ready_chk = new wxCheckBox( this, BROOM_IMREADY, _("I'm ready"), wxDefaultPosition, wxSize(-1,CONTROL_HEIGHT) );
  m_ready_chk->SetToolTip(_T("Click this if you are content with the battle settings"));
  m_lock_chk = new wxCheckBox( this, BROOM_LOCK, _("Locked"), wxDefaultPosition, wxSize(-1,CONTROL_HEIGHT) );
  m_lock_chk->SetToolTip(_T("When checked no more players may join this battle"));
  m_spec_chk = new wxCheckBox( m_player_panel, BROOM_SPEC, _("Spectator"), wxDefaultPosition, wxSize(-1,CONTROL_HEIGHT) );
  m_spec_chk->SetToolTip(_T("Check this if you just want to watch this battle"));

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

  long pos = 0;

  m_opts_list->InsertItem( pos, _("Size") );
  m_opt_list_map[ _("Size") ] = pos++;
  m_opts_list->InsertItem( pos , _("Windspeed") );
  m_opt_list_map[ _("Windspeed") ] = pos++;
  m_opts_list->InsertItem( pos, _("Tidal strength") );
  m_opt_list_map[ _("Tidal strength") ] = pos++;

  // add engine/map/mod options to the list
  m_battle.CustomBattleOptions()->loadOptions( ModOption, m_battle.GetHostModName() );
  m_battle.CustomBattleOptions()->loadOptions( MapOption, m_battle.GetHostMapName() );

  m_opts_list->InsertItem( pos++, wxEmptyString );
  pos = AddMMOptionsToList( pos++, EngineOption );
  m_opts_list->InsertItem( pos++, wxEmptyString );
  pos = AddMMOptionsToList( pos, ModOption );
  m_opts_list->InsertItem( pos++, wxEmptyString );
  m_map_opts_index = pos;
  pos = AddMMOptionsToList( pos, MapOption );


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

  m_splitter->SplitHorizontally( m_player_panel, m_chat );

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
  m_buttons_sizer->Add( m_fix_colours_btn, 0, wxEXPAND | wxALL, 2 );
  m_buttons_sizer->Add( m_balance_btn, 0, wxEXPAND | wxALL, 2 );
  m_buttons_sizer->Add( m_start_btn, 0, wxEXPAND | wxALL, 2 );

  m_main_sizer->Add( m_top_sizer, 1, wxEXPAND );
  m_main_sizer->Add( m_command_line, 0, wxEXPAND );
  m_main_sizer->Add( m_buttons_sizer, 0, wxEXPAND );

  m_splitter->SetMinimumPaneSize( 240 );

  for ( user_map_t::size_type i = 0; i < battle.GetNumUsers(); i++ ) {
    m_players->AddUser( battle.GetUser( i ) );
  }

  if ( !IsHosted() ) {
    m_start_btn->Disable();

    m_balance_btn->Disable();
    m_fix_colours_btn->Disable();

    m_lock_chk->Disable();
  } else {
    m_battle.SetImReady ( true );
    m_ready_chk->Disable();
  }

  UpdateBattleInfo( true );

  SetSizer( m_main_sizer );
  Layout();

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


void BattleRoomTab::UpdateBattleInfo( bool MapChanged, bool reloadMapOptions )
{
  if ( MapChanged ) /// the map has been changed
  {
    try { /// updates map info summary
      ASSERT_RUNTIME( m_battle.MapExists(), _T("Map does not exist.") );
      UnitSyncMap map = m_battle.LoadMap();
      m_map_lbl->SetLabel( RefineMapname( map.name ) );
      m_opts_list->SetItem( m_opt_list_map[ _("Size") ] , 1, wxString::Format( _T("%.0fx%.0f"), map.info.width/512.0, map.info.height/512.0 ) );
      m_opts_list->SetItem( m_opt_list_map[ _("Windspeed") ], 1, wxString::Format( _T("%d-%d"), map.info.minWind, map.info.maxWind) );
      m_opts_list->SetItem( m_opt_list_map[ _("Tidal strength") ], 1, wxString::Format( _T("%d"), map.info.tidalStrength) );
      //    m_opts_list->SetItem( 0, 1,  );
    } catch (...) {
      m_map_lbl->SetLabel( RefineMapname( m_battle.GetHostMapName() ) );
      m_opts_list->SetItem( m_opt_list_map[ _("Size") ], 1, _T("?x?") );
      m_opts_list->SetItem( m_opt_list_map[ _("Windspeed") ], 1, _T("?-?") );
      m_opts_list->SetItem( m_opt_list_map[ _("Tidal strength") ], 1, _T("?") );
    }

    if ( reloadMapOptions )
    {
      ///delete any eventual map option from the list and add options of the new map
      for ( long i = m_map_opts_index; i < m_opts_list->GetItemCount(); i++ ) m_opts_list->DeleteItem( i );
      m_battle.CustomBattleOptions()->loadOptions( ModOption, m_battle.GetHostModName() );
      AddMMOptionsToList( m_map_opts_index, MapOption );
    }
  }

  m_lock_chk->SetValue( m_battle.IsLocked() );
  m_minimap->UpdateMinimap();
}


void BattleRoomTab::UpdateBattleInfo( const wxString& Tag )
{
  long index = m_opt_list_map[ Tag ];
  long type;
  Tag.BeforeFirst( '_' ).ToLong( &type );
  wxString key = Tag.AfterFirst( '_' );
  wxString value;
  if ( type == EngineOption && key == _T("restrictions") )
    m_opts_list->SetItem( index, 1, bool2yn( m_battle.DisabledUnits().GetCount() > 0 ) );
  else if ( type == MapOption || type == ModOption || EngineOption )
  {
    OptionType DataType = m_battle.CustomBattleOptions()->GetSingleOptionType( key );
    if ( DataType == opt_bool )
    {
      long boolval;
      m_battle.CustomBattleOptions()->getSingleValue( key, (GameOption)type ).ToLong( &boolval );
      m_opts_list->SetItem( index, 1, bool2yn( boolval ) );
    }
    else
    {
      m_opts_list->SetItem( index, 1, m_battle.CustomBattleOptions()->getSingleValue( key, (GameOption)type ) );
    }
  }
}

BattleroomListCtrl& BattleRoomTab::GetPlayersListCtrl()
{
  ASSERT_LOGIC( m_players != 0, _T("m_players = 0") );
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
  icons().SetColourIcon( bs.team, user.BattleStatus().colour );
  m_color_sel->SetBitmapLabel( icons().GetBitmap( icons().GetColourIcon( bs.team ) ) );

  m_minimap->UpdateMinimap();
}


Battle& BattleRoomTab::GetBattle()
{
  return m_battle;
}


ChatPanel& BattleRoomTab::GetChatPanel()
{
  ASSERT_LOGIC( m_chat != 0, _T("m_chat = 0") );
  return *m_chat;
}


void BattleRoomTab::OnStart( wxCommandEvent& event )
{
  if ( m_battle.HaveMultipleBotsInSameTeam() ) {
    wxMessageDialog dlg( this, _("You have one or more bots sharing team, this is not possible."), _("Bot team sharing."), wxOK );
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



void BattleRoomTab::OnBalance( wxCommandEvent& event ){
  wxLogMessage(_T(""));
  if(!IsHosted()){/// if not hosted, say !cbalance . Works with autohosts, and human hosts knows what it mean.
    m_battle.Say(_T("!cbalance"));
    return;
  }
  AutoBalanceDialog dlg( this );
  if ( dlg.ShowModal() == wxID_OK ) {
    m_battle.Autobalance(sett().GetBalanceMethod(),sett().GetBalanceClans(),sett().GetBalanceStrongClans());
  }
  /// balance players.
}

void BattleRoomTab::OnFixColours( wxCommandEvent& event ){
  wxLogMessage(_T(""));
  if(!IsHosted()){/// Works with autohosts, and human hosts knows what it mean.
    m_battle.Say(_T("!fixcolors"));
    return;
  }
  //m_battle.Say(_T("fixing colours"));
  m_battle.FixColours();
}


void BattleRoomTab::OnAddBot( wxCommandEvent& event )
{
    //customMessageBox(SL_MAIN_ICON,_T("Max players reached"),_T("Cannot add bot, maximum number of players already reached.") );
  if ( m_battle.GetNumBots() + m_battle.GetNumUsers() - m_battle.GetSpectators()  < m_battle.GetMaxPlayers() )
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
        bs.colour = m_battle.GetFreeColour( NULL );
        m_ui.GetServer().AddBot( m_battle.GetBattleId(), dlg.GetNick(), m_battle.GetMe().GetNick(), bs, dlg.GetAI() );
      }
  }
  else
    customMessageBox(SL_MAIN_ICON,_T("Cannot add bot, maximum number of players already reached."),_T("Max players reached") );
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
  wxColour CurrentColour = bs.colour;
  CurrentColour = wxGetColourFromUser(this, CurrentColour);
  if ( !CurrentColour.IsColourOk() ) return;
  bs.colour = CurrentColour;
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

long BattleRoomTab::AddMMOptionsToList( long pos, GameOption optFlag )
{
  wxStringTripleVec optlist;
  m_battle.CustomBattleOptions()->getOptions( &optlist, optFlag );
  for (wxStringTripleVec::iterator it = optlist.begin(); it != optlist.end(); ++it)
  {
    m_opts_list->InsertItem( pos, it->second.first );
    m_opt_list_map[ wxString::Format(_T("%d_"), optFlag ) + it->first ] = pos;
    OptionType DataType = m_battle.CustomBattleOptions()->GetSingleOptionType( it->first );
    wxString value;
    if ( DataType == opt_bool )
    {
      long boolval;
      it->second.second.ToLong( &boolval );
      value = bool2yn( boolval );
    }
    else
      value = it->second.second;
    m_opts_list->SetItem( pos, 1, value );
    pos++;
  }
  return pos;
}
