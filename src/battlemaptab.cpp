/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: BattleMapTab
//

#include <wx/splitter.h>
#include <wx/intl.h>
#include <wx/combobox.h>
#include <wx/stattext.h>
#include <wx/statline.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/radiobox.h>
#include <wx/window.h>
#include <wx/listctrl.h>
#include <wx/settings.h>
#include <wx/arrstr.h>
#include <wx/choice.h>
#include <stdexcept>

#include "battlemaptab.h"
#include "ui.h"
#include "iunitsync.h"
#include "user.h"
#include "battle.h"
#include "utils.h"
#include "chatpanel.h"
#include "mapctrl.h"
#include "uiutils.h"
#include "server.h"
#include "settings.h"

BEGIN_EVENT_TABLE(BattleMapTab, wxPanel)

  EVT_CHOICE( BMAP_MAP_SEL, BattleMapTab::OnMapSelect )
  EVT_RADIOBOX( BMAP_START_TYPE, BattleMapTab::OnStartTypeSelect )

END_EVENT_TABLE()


BattleMapTab::BattleMapTab( wxWindow* parent, Ui& ui, Battle& battle ):
  wxPanel( parent, -1 ), m_ui(ui), m_battle(battle)
{
  wxBoxSizer* m_main_sizer = new wxBoxSizer( wxHORIZONTAL );
  wxBoxSizer* m_map_sizer = new wxBoxSizer( wxVERTICAL );

  m_map_sizer->SetMinSize(wxSize( 352,-1 ));
  m_minimap = new MapCtrl( this, 352, &m_battle, m_ui, !battle.IsFounderMe(), false, true, false );
  m_minimap->SetMinSize( wxSize( 352,352 ) );

  m_map_sizer->Add( m_minimap, 1, wxALL|wxEXPAND, 2 );

  wxBoxSizer* m_selmap_sizer = new wxBoxSizer( wxHORIZONTAL );

  m_map_combo = new wxChoice( this, BMAP_MAP_SEL, wxDefaultPosition, wxDefaultSize );
  m_selmap_sizer->Add( m_map_combo, 1, wxALL, 2 );

 // m_browse_btn = new wxButton( this, wxID_ANY, _("Select"), wxDefaultPosition, wxDefaultSize, 0 );

  //m_selmap_sizer->Add( m_browse_btn, 0, wxALL, 2 );

  m_map_sizer->Add( m_selmap_sizer, 0, wxEXPAND, 5 );

  m_main_sizer->Add( m_map_sizer, 1, wxEXPAND, 5 );

  wxBoxSizer* m_opts_sizer = new wxBoxSizer( wxVERTICAL );

  //m_opts_sizer->SetMinSize(wxSize( 200,-1 ));
  m_map_opts_list = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxSize( 150,160 ), wxLC_NO_HEADER|wxLC_REPORT );
  m_map_opts_list->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
  m_map_opts_list->SetFont( wxFont( 8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_LIGHT ) );

  wxListItem col;

  col.SetText( _("Option") );
  m_map_opts_list->InsertColumn( 0, col );
  col.SetText( _("Value") );
  m_map_opts_list->InsertColumn( 1, col );
  m_map_opts_list->SetColumnWidth( 0, 90 );
  m_map_opts_list->SetColumnWidth( 1, 50 );

  m_map_opts_list->InsertItem( 0, _("Size") );
  m_map_opts_list->InsertItem( 1, _("Windspeed") );
  m_map_opts_list->InsertItem( 2, _("Tidal strength") );
  m_map_opts_list->InsertItem( 3, _("Gravity") );
  m_map_opts_list->InsertItem( 4, _("Extractor radius") );
  m_map_opts_list->InsertItem( 5, _("Max metal") );

  m_opts_sizer->Add( m_map_opts_list, 0, wxALL, 2 );

  wxString m_start_radiosChoices[] = { _("Fixed"),_("Random"), _("Choose in game") };
  int m_start_radiosNChoices = sizeof( m_start_radiosChoices ) / sizeof( wxString );
  //TODO these need to be tooltipped, no idea how yet
  m_start_radios = new wxRadioBox( this, BMAP_START_TYPE, _("Startpositions"), wxDefaultPosition, wxSize( 150,-1 ), m_start_radiosNChoices, m_start_radiosChoices, 1, wxRA_SPECIFY_COLS );
  m_opts_sizer->Add( m_start_radios, 0, wxALL, 2 );

  m_main_sizer->Add( m_opts_sizer, 0, wxEXPAND, 5 );
  //m_main_sizer->AddStretchSpacer();
  SetSizer( m_main_sizer );
  Layout();

  if(battle.IsFounderMe()){
    sett().LoadBattleMapOptions(&m_battle);
    m_battle.SendHostInfo( HI_StartRects );
  }
  ReloadMaplist();
  Update();

  //m_map_combo->Enable( m_battle.IsFounderMe() );
  m_start_radios->Enable( m_battle.IsFounderMe() );

}


BattleMapTab::~BattleMapTab()
{
  if(m_battle.IsFounderMe()){
    sett().SaveBattleMapOptions(&m_battle);
  }
}


void BattleMapTab::Update()
{
  wxString value = m_battle.CustomBattleOptions()->getSingleValue( _T("startpostype"), EngineOption);
  long longval;
  value.ToLong( &longval );
  m_start_radios->SetSelection( longval );

  m_minimap->UpdateMinimap();

  if ( !m_battle.MapExists() ) return;

  UnitSyncMap map = m_battle.LocalMap();

  m_map_opts_list->SetItem( 0, 1, wxString::Format( _T("%dx%d"), map.info.width/512, map.info.height/512 ) );
  m_map_opts_list->SetItem( 1, 1, wxString::Format( _T("%d-%d"), map.info.minWind, map.info.maxWind ) );
  m_map_opts_list->SetItem( 2, 1, wxString::Format( _T("%d"), map.info.tidalStrength ) );
  m_map_opts_list->SetItem( 3, 1, wxString::Format( _T("%d"), map.info.gravity ) );
  m_map_opts_list->SetItem( 4, 1, wxString::Format( _T("%d"), map.info.extractorRadius ) );
  m_map_opts_list->SetItem( 5, 1, wxString::Format( _T("%.3f"), map.info.maxMetal ) );

  int index = m_map_combo->FindString( RefineMapname( map.name ) );
  m_map_combo->SetSelection( index );
}


void BattleMapTab::Update( const wxString& Tag )
{
  long type;
  Tag.BeforeFirst( '_' ).ToLong( &type );
  wxString key = Tag.AfterFirst( '_' );
  wxString value = m_battle.CustomBattleOptions()->getSingleValue( key, (GameOption)type);
  long longval;
  value.ToLong( &longval );
  if ( type == EngineOption )
  {
    if ( key == _T("startpostype") )
    {
     m_start_radios->SetSelection( longval );
     Update();
    }
  }
}


void BattleMapTab::ReloadMaplist()
{
  m_map_combo->Clear();
  try {
    for ( int i = 0; i < usync()->GetNumMaps(); i++ ) {
      m_map_combo->Insert( RefineMapname( usync()->GetMap( i ).name ), i );
    }
  } catch(...){}
}


void BattleMapTab::UpdateUser( User& user )
{
  if ( &m_battle.GetMe() == &user ) {
    try {
      m_minimap->UpdateMinimap();
    } catch (...) { }
  }
}


void BattleMapTab::OnMapSelect( wxCommandEvent& event )
{

  if ( !m_battle.IsFounderMe() ) {
    m_map_combo->SetSelection( m_map_combo->FindString( RefineMapname( m_battle.GetHostMapName() ) ) );
    return;
  }

  sett().SaveBattleMapOptions(&m_battle);

  int index = m_map_combo->GetCurrentSelection();
  //wxString name = m_map_combo->GetString( index );
  try {
    UnitSyncMap map = usync()->GetMapEx( index );
    m_battle.SetLocalMap( map );

    sett().LoadBattleMapOptions(&m_battle);
    m_battle.SendHostInfo( HI_StartRects );

  } catch (...) {}
  m_ui.OnBattleMapChanged(m_battle);
  m_battle.SendHostInfo( HI_Map );
}


void BattleMapTab::OnStartTypeSelect( wxCommandEvent& event )
{
  wxString pos = wxString::Format( _T("%d"), m_start_radios->GetSelection());
  m_battle.CustomBattleOptions()->setSingleOption( _T("startpostype"), pos, EngineOption );
  m_battle.SendHostInfo( wxString::Format(_T("%d_startpostype"), EngineOption ) );
}


void BattleMapTab::OnUnitSyncReloaded()
{
  m_minimap->UpdateMinimap();
  ReloadMaplist();
}

