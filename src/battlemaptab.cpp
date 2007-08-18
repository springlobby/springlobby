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
  m_minimap = new MapCtrl( this, 352, &m_battle, m_ui, !battle.IsFounderMe() );
	m_minimap->SetMinSize( wxSize( 352,352 ) );
	
	m_map_sizer->Add( m_minimap, 1, wxALL|wxEXPAND, 2 );
	
	wxBoxSizer* m_selmap_sizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_map_combo = new wxChoice( this, BMAP_MAP_SEL, wxDefaultPosition, wxDefaultSize );
	m_selmap_sizer->Add( m_map_combo, 1, wxALL, 2 );
	
	m_browse_btn = new wxButton( this, wxID_ANY, _("Select"), wxDefaultPosition, wxDefaultSize, 0 );
	
	m_selmap_sizer->Add( m_browse_btn, 0, wxALL, 2 );
	
	m_map_sizer->Add( m_selmap_sizer, 0, wxEXPAND, 5 );
	
	m_main_sizer->Add( m_map_sizer, 1, wxEXPAND, 5 );
	
	wxBoxSizer* m_opts_sizer = new wxBoxSizer( wxVERTICAL );
	
	//m_opts_sizer->SetMinSize(wxSize( 200,-1 )); 	
	m_map_opts_list = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxSize( 150,240 ), wxLC_NO_HEADER|wxLC_REPORT );
//m_map_opts_list->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );

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
	m_start_radios = new wxRadioBox( this, BMAP_START_TYPE, _("Startpositions"), wxDefaultPosition, wxSize( 150,-1 ), m_start_radiosNChoices, m_start_radiosChoices, 1, wxRA_SPECIFY_COLS );
	m_opts_sizer->Add( m_start_radios, 0, wxALL, 2 );
	
	m_main_sizer->Add( m_opts_sizer, 0, wxEXPAND, 5 );
	//m_main_sizer->AddStretchSpacer();
	SetSizer( m_main_sizer );
	Layout();

  ReloadMaplist();
  UpdateMap();

  //m_map_combo->Enable( m_battle.IsFounderMe() );
  m_start_radios->Enable( m_battle.IsFounderMe() );

}


BattleMapTab::~BattleMapTab()
{

}


void BattleMapTab::UpdateMap()
{
  m_start_radios->SetSelection( m_battle.opts().starttype );

  m_minimap->UpdateMinimap();

  if ( !usync()->MapExists( m_battle.opts().mapname ) ) return;

  UnitSyncMap map = usync()->GetMap( m_battle.opts().mapname );
  if ( map.hash != m_map.hash ) map = m_map = usync()->GetMap( m_battle.opts().mapname, true );
  else map = m_map;

  m_map_opts_list->SetItem( 0, 1, wxString::Format( _T("%dx%d"), map.info.width/512, map.info.height/512 ) );
  m_map_opts_list->SetItem( 1, 1, wxString::Format( _T("%d-%d"), map.info.minWind, map.info.maxWind ) );
  m_map_opts_list->SetItem( 2, 1, wxString::Format( _T("%d"), map.info.tidalStrength ) );
  m_map_opts_list->SetItem( 3, 1, wxString::Format( _T("%d"), map.info.gravity ) );
  m_map_opts_list->SetItem( 4, 1, wxString::Format( _T("%d"), map.info.extractorRadius ) );
  m_map_opts_list->SetItem( 5, 1, wxString::Format( _T("%.3f"), map.info.maxMetal ) );

  int index = m_map_combo->FindString( RefineMapname( WX_STRING(map.name) ) );
  m_map_combo->SetSelection( index );
}

void BattleMapTab::ReloadMaplist()
{
  m_map_combo->Clear();
  for ( int i = 0; i < usync()->GetNumMaps(); i++ ) {
    m_map_combo->Insert( RefineMapname( WX_STRING(usync()->GetMap( i, false ).name) ), i );
  }
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
    m_map_combo->SetSelection( m_map_combo->FindString( RefineMapname( WX_STRING(m_battle.opts().mapname) ) ) );
    return;
  }
  int index = m_map_combo->GetCurrentSelection();
  //wxString name = m_map_combo->GetString( index );

  UnitSyncMap map = usync()->GetMap( index );
  m_battle.SetMapname( map.name );
  m_battle.SetMapHash( map.hash );

  m_ui.SendHostInfo( HI_Map );
}


void BattleMapTab::OnStartTypeSelect( wxCommandEvent& event )
{
  int pos = m_start_radios->GetSelection();
  switch ( pos ) {
    case 0: m_battle.SetStartType( ST_Fixed ); break;
    case 1: m_battle.SetStartType( ST_Random ); break;
    case 2: m_battle.SetStartType( ST_Choose ); break;
    default: assert(false);
  };
  m_ui.SendHostInfo( HI_StartType );
}

