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

BEGIN_EVENT_TABLE(BattleMapTab, wxPanel)

END_EVENT_TABLE()


BattleMapTab::BattleMapTab( wxWindow* parent, Ui& ui, Battle& battle ) : wxPanel( parent, -1 ),m_ui(ui), m_battle(battle)
{
	wxBoxSizer* m_main_sizer = new wxBoxSizer( wxHORIZONTAL );
	wxBoxSizer* m_map_sizer = new wxBoxSizer( wxVERTICAL );
	
	m_map_sizer->SetMinSize(wxSize( 352,-1 )); 
	m_minimap = new MapCtrl( this, 352, m_battle, false );
	m_minimap->SetMinSize( wxSize( 352,352 ) );
	
	m_map_sizer->Add( m_minimap, 0, wxALL, 2 );
	
	wxBoxSizer* m_selmap_sizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_map_combo = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize );
	m_selmap_sizer->Add( m_map_combo, 1, wxALL, 2 );
	
	m_browse_btn = new wxButton( this, wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, 0 );
	m_browse_btn->Enable( false );
	
	m_selmap_sizer->Add( m_browse_btn, 0, wxALL, 2 );
	
	m_map_sizer->Add( m_selmap_sizer, 0, wxEXPAND, 5 );
	
	m_main_sizer->Add( m_map_sizer, 0, wxEXPAND, 5 );
	
	wxBoxSizer* m_opts_sizer = new wxBoxSizer( wxVERTICAL );
	
	//m_opts_sizer->SetMinSize(wxSize( 200,-1 )); 	
	m_map_opts_list = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxSize( 150,240 ), wxLC_NO_HEADER|wxLC_REPORT );
//m_map_opts_list->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );

  wxListItem col;

  col.SetText( _T("Option") );
  m_map_opts_list->InsertColumn( 0, col );
  col.SetText( _T("Value") );
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

  wxString m_start_radiosChoices[] = { wxT("Fixed"), wxT("Random"), wxT("Choose in game") };
	int m_start_radiosNChoices = sizeof( m_start_radiosChoices ) / sizeof( wxString );
	m_start_radios = new wxRadioBox( this, wxID_ANY, wxT("Startpositions"), wxDefaultPosition, wxSize( 150,-1 ), m_start_radiosNChoices, m_start_radiosChoices, 1, wxRA_SPECIFY_COLS );
	m_opts_sizer->Add( m_start_radios, 0, wxALL, 2 );
	
	m_main_sizer->Add( m_opts_sizer, 1, wxEXPAND, 5 );
	m_main_sizer->AddStretchSpacer();
	SetSizer( m_main_sizer );
	Layout();

  ReloadMaplist();
  UpdateMap();
}


BattleMapTab::~BattleMapTab()
{

}


void BattleMapTab::UpdateMap()
{
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

  m_start_radios->SetSelection( m_battle.opts().starttype );

  m_minimap->UpdateMinimap();
}

void BattleMapTab::ReloadMaplist()
{
  m_map_combo->Clear();
  for ( int i = 0; i < usync()->GetNumMaps(); i++ ) {
    m_map_combo->Insert( RefineMapname( WX_STRING(usync()->GetMap( i, false ).name) ), i );
  }
}

