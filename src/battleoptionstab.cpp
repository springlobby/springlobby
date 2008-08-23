/* Copyright (C) 2007, 2008 The SpringLobby Team. All rights reserved. */
//
// Class: BattleOptionsTab
//

#include <wx/splitter.h>
#include <wx/intl.h>
#include <wx/combobox.h>
#include <wx/stattext.h>
#include <wx/statline.h>
#include <wx/statbox.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/radiobox.h>
#include <wx/window.h>
#include <wx/listbox.h>
#include <wx/settings.h>
#include <wx/arrstr.h>
#include <wx/choice.h>
#include <wx/tokenzr.h>
#include <wx/slider.h>
#include <wx/checklst.h>
#include <stdexcept>

#include "battleoptionstab.h"
#include "ui.h"
#include "iunitsync.h"
#include "user.h"
#include "ibattle.h"
#include "utils.h"
#include "chatpanel.h"
#include "mapctrl.h"
#include "uiutils.h"
#include "server.h"
#include "mmoptionswrapper.h"

#ifndef HAVE_WX26
#include "auimanager.h"
#endif

#define LIMIT_DGUN_INDEX 0
#define GHOUSTED_INDEX  1
#define DIM_MMS_INDEX 2
#define LOCK_SPEED_INDEX 3
#define RANDOM_START_INDEX 3

#define SLI_METAL_ID 	9000
#define SLI_ENERGY_ID	9001
#define SLI_UNITS_ID	9002


BEGIN_EVENT_TABLE(BattleOptionsTab, wxPanel)

  EVT_RADIOBOX( BOPTS_END, BattleOptionsTab::OnEndSelect )
  EVT_CHECKLISTBOX( BOPTS_OPTS, BattleOptionsTab::OnOptsCheck )
  EVT_COMMAND_SCROLL_THUMBRELEASE( wxID_ANY, BattleOptionsTab::OnSlideChanged )
  EVT_COMMAND_SCROLL_CHANGED( wxID_ANY, BattleOptionsTab::OnSlideChanged )

  EVT_BUTTON( BOPTS_RESTRICT, BattleOptionsTab::OnRestrict )
  EVT_BUTTON( BOPTS_ALLOW, BattleOptionsTab::OnAllow )
  EVT_BUTTON( BOPTS_LOADRES, BattleOptionsTab::OnLoadRestrictions )
  EVT_BUTTON( BOPTS_SAVERES, BattleOptionsTab::OnSaveRestrictions )
  EVT_BUTTON( BOPTS_CLEARRES, BattleOptionsTab::OnClearRestrictions )

END_EVENT_TABLE()


BattleOptionsTab::BattleOptionsTab( wxWindow* parent, Ui& ui, IBattle& battle, bool singleplayer ):
  wxScrolledWindow( parent, -1 ), m_ui(ui), m_battle(battle), m_sp(singleplayer)
{

  #ifndef HAVE_WX26
  GetAui().manager->AddPane( this, wxLEFT, _T("battleoptionstab") );
  #endif

  wxBoxSizer* m_main_sizer;
  m_main_sizer = new wxBoxSizer( wxHORIZONTAL );

  wxBoxSizer* m_main_options_sizer;
  m_main_options_sizer = new wxBoxSizer( wxVERTICAL );

  wxString m_end_radiosChoices[] =
    {
      _("Continue if commander dies"),
      _("End if commander dies"),
      _("Lineage mode")
    };
  int m_end_radiosNChoices = sizeof( m_end_radiosChoices ) / sizeof( wxString );
  //TODO these need to be tooltipped, no idea how yet
  m_end_radios = new wxRadioBox( this, BOPTS_END, _("End condition"), wxDefaultPosition, wxDefaultSize, m_end_radiosNChoices, m_end_radiosChoices, 1, wxRA_SPECIFY_COLS );
  m_main_options_sizer->Add( m_end_radios, 0, wxALL|wxEXPAND, 5 );

  wxStaticBoxSizer* m_resources_box;
  m_resources_box = new wxStaticBoxSizer( new wxStaticBox( this, -1, _("Resources") ), wxHORIZONTAL );

  wxBoxSizer* m_metal_sizer;
  m_metal_sizer = new wxBoxSizer( wxVERTICAL );

  m_metal_lbl = new wxStaticText( this, wxID_ANY, _("Start Metal"), wxDefaultPosition, wxDefaultSize, 0 );
  m_metal_sizer->Add( m_metal_lbl, 0, wxALL, 5 );

  m_metal_slider = new wxSlider( this, SLI_METAL_ID, 1000, 0, 10000, wxDefaultPosition, wxDefaultSize, wxSL_BOTH|wxSL_VERTICAL|wxSL_LABELS );
  m_metal_slider->SetToolTip( _("The amount of metal each player starts with.") );

  m_metal_sizer->Add( m_metal_slider, 1, wxALL|wxEXPAND, 5 );

  m_resources_box->Add( m_metal_sizer, 0, wxEXPAND, 5 );

  m_staticline2 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
  m_resources_box->Add( m_staticline2, 0, wxALL|wxEXPAND, 5 );

  wxBoxSizer* m_energy_sizer;
  m_energy_sizer = new wxBoxSizer( wxVERTICAL );

  m_energy_lbl = new wxStaticText( this, wxID_ANY, _("Start Energy"), wxDefaultPosition, wxDefaultSize, 0 );
  m_energy_sizer->Add( m_energy_lbl, 0, wxALL, 5 );

  m_energy_slider = new wxSlider( this,SLI_ENERGY_ID , 1000, 0, 10000, wxDefaultPosition, wxDefaultSize, wxSL_BOTH|wxSL_VERTICAL|wxSL_LABELS );
  m_energy_slider->SetToolTip( _("The amount of energy each player starts with.") );

  m_energy_sizer->Add( m_energy_slider, 1, wxALL|wxEXPAND, 5 );

  m_resources_box->Add( m_energy_sizer, 1, wxEXPAND, 5 );

  m_staticline21 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
  m_resources_box->Add( m_staticline21, 0, wxALL|wxEXPAND, 5 );

  wxBoxSizer* m_units_sizer;
  m_units_sizer = new wxBoxSizer( wxVERTICAL );

  m_units_lbl = new wxStaticText( this, wxID_ANY, _("Max units"), wxDefaultPosition, wxDefaultSize, 0 );
  m_units_sizer->Add( m_units_lbl, 0, wxALL, 5 );

  m_units_slider = new wxSlider( this, SLI_UNITS_ID, 500, 10, 5000, wxDefaultPosition, wxDefaultSize, wxSL_BOTH|wxSL_VERTICAL|wxSL_LABELS );
  m_units_slider->SetToolTip( _("The maximum number of units allowed per player.") );

  m_units_sizer->Add( m_units_slider, 1, wxALL|wxEXPAND, 5 );

  m_resources_box->Add( m_units_sizer, 1, wxEXPAND, 5 );

  m_main_options_sizer->Add( m_resources_box, 1, wxALL|wxEXPAND, 5 );

  wxStaticBoxSizer* m_options_box;
  m_options_box = new wxStaticBoxSizer( new wxStaticBox( this, -1, _("Options") ), wxVERTICAL );

  //TODO these need to be tooltipped, no idea how yet
  m_options_checks = new wxCheckListBox( this, BOPTS_OPTS );
  m_options_checks->Append( _("Limit d-gun") );
  m_options_checks->Append( _("Ghosted buildings") );
  m_options_checks->Append( _("Diminishing metal makers") );
  if ( m_sp ) m_options_checks->Append( _("Random start positions") );

  m_options_box->Add( m_options_checks, 0, wxALL|wxEXPAND, 5 );

  m_main_options_sizer->Add( m_options_box, 0, wxALL|wxEXPAND, 5 );

  m_main_sizer->Add( m_main_options_sizer, 0, wxEXPAND, 5 );

  wxStaticBoxSizer* m_restr_box;
  m_restr_box = new wxStaticBoxSizer( new wxStaticBox( this, -1, _("Unit restrictions") ), wxVERTICAL );

  wxBoxSizer* m_top_restr_sizer;
  m_top_restr_sizer = new wxBoxSizer( wxHORIZONTAL );

  wxBoxSizer* m_allowed_sizer;
  m_allowed_sizer = new wxBoxSizer( wxVERTICAL );

  m_aloowed_lbl = new wxStaticText( this, wxID_ANY, _("Allowed units"), wxDefaultPosition, wxDefaultSize, 0 );
  m_allowed_sizer->Add( m_aloowed_lbl, 0, wxALL, 5 );

  m_allowed_list = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_MULTIPLE|wxLB_NEEDED_SB|wxLB_SORT );
  m_allowed_list->SetToolTip( _("Units in this list will be available in the game.") );

  m_allowed_sizer->Add( m_allowed_list, 1, wxALL|wxEXPAND, 5 );

  m_top_restr_sizer->Add( m_allowed_sizer, 1, wxEXPAND, 5 );

  wxBoxSizer* m_mid_btn_sizer;
  m_mid_btn_sizer = new wxBoxSizer( wxVERTICAL );

  m_mid_btn_sizer->Add( 0, 50, 0, wxEXPAND, 0 );

  m_restrict_btn = new wxButton( this, BOPTS_RESTRICT, _T(">"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
  m_mid_btn_sizer->Add( m_restrict_btn, 0, wxALL, 5 );

  m_allow_btn = new wxButton( this, BOPTS_ALLOW, _T("<"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
  m_mid_btn_sizer->Add( m_allow_btn, 0, wxALL, 5 );

  m_top_restr_sizer->Add( m_mid_btn_sizer, 0, wxEXPAND, 5 );

  wxBoxSizer* m_restricted_sizer;
  m_restricted_sizer = new wxBoxSizer( wxVERTICAL );

  m_restricted_lbl = new wxStaticText( this, wxID_ANY, _("Restricted units"), wxDefaultPosition, wxDefaultSize, 0 );
  m_restricted_sizer->Add( m_restricted_lbl, 0, wxALL, 5 );

  m_restrict_list = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_MULTIPLE|wxLB_NEEDED_SB|wxLB_SORT );
  m_restrict_list->SetToolTip( _("Units in this list will not be available in the game.") );

  m_restricted_sizer->Add( m_restrict_list, 1, wxALL|wxEXPAND, 5 );

  m_top_restr_sizer->Add( m_restricted_sizer, 1, wxEXPAND, 5 );

  m_restr_box->Add( m_top_restr_sizer, 1, wxEXPAND, 5 );

  wxBoxSizer* m_restricted_btn_sizer;
  m_restricted_btn_sizer = new wxBoxSizer( wxHORIZONTAL );

  m_load_btn = new wxButton( this, BOPTS_LOADRES, _("Load..."), wxDefaultPosition, wxDefaultSize, 0 );
  m_load_btn->SetToolTip( _("Load a saved set of restrictions.") );
  m_load_btn->Disable();

  m_restricted_btn_sizer->Add( m_load_btn, 0, wxALL, 5 );

  m_save_btn = new wxButton( this, BOPTS_SAVERES, _("Save..."), wxDefaultPosition, wxDefaultSize, 0 );
  m_save_btn->SetToolTip( _("Save a set of restrictions.") );
  m_save_btn->Disable();

  m_restricted_btn_sizer->Add( m_save_btn, 0, wxALL, 5 );

  m_restricted_btn_sizer->Add( 0, 0, 1, wxEXPAND, 0 );

  m_clear_btn = new wxButton( this, BOPTS_CLEARRES, _("Clear"), wxDefaultPosition, wxDefaultSize, 0 );
  m_clear_btn->SetToolTip( _("Enable all units.") );

  m_restricted_btn_sizer->Add( m_clear_btn, 0, wxALL, 5 );

  m_restr_box->Add( m_restricted_btn_sizer, 0, wxEXPAND, 5 );

  m_main_sizer->Add( m_restr_box, 1, wxALL|wxEXPAND, 5 );

  SetScrollRate( 3, 3 );
  this->SetSizer( m_main_sizer );
  this->Layout();

  UpdateBattle(  wxString::Format(_T("%d_gamemode"), EngineOption ) );
  UpdateBattle(  wxString::Format(_T("%d_limitdgun"), EngineOption ) );
  UpdateBattle(  wxString::Format(_T("%d_startmetal"), EngineOption ) );
  UpdateBattle(  wxString::Format(_T("%d_startenergy"), EngineOption ) );
  UpdateBattle(  wxString::Format(_T("%d_maxunits"), EngineOption ) );
  UpdateBattle(  wxString::Format(_T("%d_ghostedbuildings"), EngineOption ) );
  UpdateBattle(  wxString::Format(_T("%d_diminishingmms"), EngineOption ) );

  ReloadRestrictions();

  if ( !m_battle.IsFounderMe() ) {
    m_end_radios->Disable();
    m_options_checks->Disable();
    m_metal_slider->Disable();
    m_energy_slider->Disable();
    m_units_slider->Disable();
    m_restrict_btn->Disable();
    m_allow_btn->Disable();
    m_clear_btn->Disable();
    m_restrict_btn->Disable();
    m_allow_btn->Disable();
  }
}


BattleOptionsTab::~BattleOptionsTab()
{
  #ifndef HAVE_WX26
  GetAui().manager->DetachPane( this );
  #endif
}


void BattleOptionsTab::UpdateBattle( const wxString& Tag )
{
  long type;
  Tag.BeforeFirst( '_' ).ToLong( &type );
  wxString key = Tag.AfterFirst( '_' );
  wxString value = m_battle.CustomBattleOptions()->getSingleValue( key, (GameOption)type);
  long longval;
  value.ToLong( &longval );
  if ( type == EngineOption )
  {

    if ( key == _T("gamemode") ) m_end_radios->SetSelection( longval );
    else if ( key == _T("limitdgun") ) m_options_checks->Check( LIMIT_DGUN_INDEX, longval );
    else if ( key == _T("startmetal") )
    {
      m_metal_slider->SetValue( longval );
      m_last_metal = longval;

    }
    else if ( key == _T("startenergy") )
    {
      m_energy_slider->SetValue( longval );
      m_last_energy = longval;
    }
    else if ( key == _T("maxunits") )
    {
      m_units_slider->SetValue( longval );
      m_last_units = longval;
    }
    else if ( key == _T("ghostedbuildings") ) m_options_checks->Check( GHOUSTED_INDEX, longval );
    else if ( key == _T("diminishingmms") ) m_options_checks->Check( DIM_MMS_INDEX, longval );
  }
}

void BattleOptionsTab::ReloadRestrictions()
{
  m_allowed_list->Clear();
  m_restrict_list->Clear();
  if ( m_battle.GetHostModName() == wxEmptyString ) return;

  try {
    m_allowed_list->InsertItems( usync()->GetUnitsList( m_battle.GetHostModName() ), 0 );
  } catch (...) {}
  wxArrayString units = m_battle.DisabledUnits();

  for ( unsigned int i = 0; i < units.GetCount(); i++) Restrict( units[i] );
}


int BattleOptionsTab::GetAllowedUnitIndex( const wxString& name )
{
  for ( unsigned int i = 0; i < m_allowed_list->GetCount(); i++ ) {
    wxString tmp = m_allowed_list->GetString( i );
    tmp = tmp.AfterLast( '(' );
    tmp = tmp.BeforeLast( ')' );
    if ( name == tmp ) return i;
  }
  return -1;
}


int BattleOptionsTab::GetRestrictedUnitIndex( const wxString& name )
{
  for ( unsigned int i = 0; i < m_restrict_list->GetCount(); i++ ) {
    wxString tmp = m_restrict_list->GetString( i );
    tmp = tmp.AfterLast( '(' );
    tmp = tmp.BeforeLast( ')' );
    if ( name == tmp ) return i;
  }
  return -1;
}


bool BattleOptionsTab::IsRestricted( const wxString& name )
{
  return ( GetRestrictedUnitIndex( name ) >= 0 );
}


void BattleOptionsTab::Restrict( const wxString& name )
{
  int i = GetAllowedUnitIndex( name );
  Restrict( i );
}


void BattleOptionsTab::Allow( const wxString& name )
{
  int i = GetRestrictedUnitIndex( name );
  Allow( i );
}


void BattleOptionsTab::Restrict( int index )
{
  if ( index >= 0 ) {
    wxString unit = m_allowed_list->GetString( index );
    m_restrict_list->Append( unit );
    m_allowed_list->Delete( index );
    unit = unit.AfterLast( '(' );
    unit = unit.BeforeLast( ')' );
    m_battle.DisableUnit( unit );
  }
}


void BattleOptionsTab::Allow( int index)
{
  if ( index >= 0 ) {
    wxString unit = m_restrict_list->GetString( index );
    m_allowed_list->Append( unit );
    m_restrict_list->Delete( index );
    unit = unit.AfterLast( '(' );
    unit = unit.BeforeLast( ')' );
    m_battle.EnableUnit( unit );
  }
}


  //////////////////////////////////////////////////////////////////////////
 //  EVENTS
//////////////////////////////////////////////////////////////////////////

void BattleOptionsTab::OnEndSelect( wxCommandEvent& event )
{
  m_battle.CustomBattleOptions()->setSingleOption( _T("gamemode"), i2s(m_end_radios->GetSelection()), EngineOption );
  m_battle.SendHostInfo( wxString::Format(_T("%d_gamemode"), EngineOption ) );
}


void BattleOptionsTab::OnOptsCheck( wxCommandEvent& event )
{
  m_battle.CustomBattleOptions()->setSingleOption( _T("limitdgun"), i2s(m_options_checks->IsChecked( LIMIT_DGUN_INDEX )), EngineOption );
  m_battle.SendHostInfo( wxString::Format(_T("%d_limitdgun"), EngineOption ) );

  m_battle.CustomBattleOptions()->setSingleOption( _T("ghostedbuildings"), i2s(m_options_checks->IsChecked( GHOUSTED_INDEX )), EngineOption );
  m_battle.SendHostInfo( wxString::Format(_T("%d_ghostedbuildings"), EngineOption ) );

  m_battle.CustomBattleOptions()->setSingleOption( _T("diminishingmms"), i2s(m_options_checks->IsChecked( DIM_MMS_INDEX )), EngineOption );
  m_battle.SendHostInfo( wxString::Format(_T("%d_diminishingmms"), EngineOption ) );

  if ( m_sp ) {
    if ( m_options_checks->IsChecked( RANDOM_START_INDEX ) )
      m_battle.CustomBattleOptions()->setSingleOption( _T("startpostype"), i2s(ST_Random), EngineOption );
    else
      m_battle.CustomBattleOptions()->setSingleOption( _T("startpostype"), i2s(ST_Pick), EngineOption );
    m_battle.SendHostInfo( HI_StartType );
  }

}


void BattleOptionsTab::OnSlideChanged( wxScrollEvent& event )
{
  /* for reference
  HostInfo changed = HI_None;
  if  		( m_last_metal != m_metal_slider->GetValue() ) {
    m_last_metal = m_metal_slider->GetValue();
    m_battle.SetStartMetal( m_last_metal );
    changed |= HI_StartResources;
  } else if ( m_last_energy != m_energy_slider->GetValue() ) {
    m_last_energy = m_energy_slider->GetValue();
    m_battle.SetStartEnergy( m_last_energy );
    changed |= HI_StartResources;
  } else if ( m_last_units != m_units_slider->GetValue() ) {
    m_last_units = m_units_slider->GetValue();
    m_battle.SetMaxUnits( m_last_units );
    changed |= HI_MaxUnits;
  }*/
  wxString val;

  switch (event.GetId())
  {
  case SLI_METAL_ID:
	  m_last_metal = m_metal_slider->GetValue();
    m_battle.CustomBattleOptions()->setSingleOption( _T("startmetal"), i2s(m_last_metal), EngineOption );
    m_battle.SendHostInfo( wxString::Format(_T("%d_startmetal"), EngineOption ) );
	  break;
  case SLI_ENERGY_ID:
	  m_last_energy = m_energy_slider->GetValue();
    m_battle.CustomBattleOptions()->setSingleOption( _T("startenergy"), i2s(m_last_energy), EngineOption );
    m_battle.SendHostInfo( wxString::Format(_T("%d_startenergy"), EngineOption ) );
	  break;
  case SLI_UNITS_ID:
	  m_last_units = m_units_slider->GetValue();
    m_battle.CustomBattleOptions()->setSingleOption( _T("maxunits"), i2s(m_last_units), EngineOption );
    m_battle.SendHostInfo( wxString::Format(_T("%d_maxunits"), EngineOption ) );
	  break;
  }
}


void BattleOptionsTab::OnRestrict( wxCommandEvent& event )
{
  wxArrayInt sel;
  wxArrayString names;

  m_allowed_list->GetSelections( sel );
  for ( unsigned int i = 0; i < sel.Count(); i++ ) {
    wxString name = m_allowed_list->GetString( sel.Item( i ) );
    name = name.AfterLast( '(' );
    name = name.BeforeLast( ')' );
    names.Add( name );
  }
  for ( unsigned int i = 0; i < names.Count(); i++ ) {
    Restrict( names.Item( i ) );
  }
  if ( names.Count() > 0 ) m_battle.SendHostInfo( HI_Restrictions );
}


void BattleOptionsTab::OnAllow( wxCommandEvent& event )
{
  wxArrayInt sel;
  wxArrayString names;

  m_restrict_list->GetSelections( sel );
  for ( unsigned int i = 0; i < sel.Count(); i++ ) {
    wxString name = m_restrict_list->GetString( sel.Item( i ) );
    name = name.AfterLast( '(' );
    name = name.BeforeLast( ')' );
    names.Add( name );
  }
  for ( unsigned int i = 0; i < names.Count(); i++ ) {
    Allow( names.Item( i ) );
  }
  if ( names.Count() > 0 ) m_battle.SendHostInfo( HI_Restrictions );

}


void BattleOptionsTab::OnLoadRestrictions( wxCommandEvent& event )
{
}


void BattleOptionsTab::OnSaveRestrictions( wxCommandEvent& event )
{
}


void BattleOptionsTab::OnClearRestrictions( wxCommandEvent& event )
{
  m_battle.EnableAllUnits();
  ReloadRestrictions();
}

