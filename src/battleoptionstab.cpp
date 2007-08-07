/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
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
#include <wx/listctrl.h>
#include <wx/settings.h>
#include <wx/arrstr.h>
#include <wx/choice.h>
#include <wx/slider.h>
#include <wx/checklst.h>

#include "battleoptionstab.h"
#include "ui.h"
#include "iunitsync.h"
#include "user.h"
#include "battle.h"
#include "utils.h"
#include "chatpanel.h"
#include "mapctrl.h"
#include "uiutils.h"
#include "server.h"

BEGIN_EVENT_TABLE(BattleOptionsTab, wxPanel)

//  EVT_CHOICE( BMAP_MAP_SEL, BattleOptionsTab::OnMapSelect )
//  EVT_RADIOBOX( BMAP_START_TYPE, BattleOptionsTab::OnStartTypeSelect )

END_EVENT_TABLE()


BattleOptionsTab::BattleOptionsTab( wxWindow* parent, Ui& ui, Battle& battle ): 
  wxPanel( parent, -1 ), m_ui(ui), m_battle(battle)
{
	wxBoxSizer* m_main_sizer;
	m_main_sizer = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* m_main_options_sizer;
	m_main_options_sizer = new wxBoxSizer( wxVERTICAL );
	
	wxString m_end_radiosChoices[] = { _("Continue if commander dies"), _("End if commander dies"), _("Linage mode") };
	int m_end_radiosNChoices = sizeof( m_end_radiosChoices ) / sizeof( wxString );
	m_end_radios = new wxRadioBox( this, wxID_ANY, _("End condition"), wxDefaultPosition, wxDefaultSize, m_end_radiosNChoices, m_end_radiosChoices, 1, wxRA_SPECIFY_COLS );
	m_main_options_sizer->Add( m_end_radios, 0, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* m_resources_box;
	m_resources_box = new wxStaticBoxSizer( new wxStaticBox( this, -1, _("Resources") ), wxHORIZONTAL );
	
	wxBoxSizer* m_metal_sizer;
	m_metal_sizer = new wxBoxSizer( wxVERTICAL );
	
	m_metal_lbl = new wxStaticText( this, wxID_ANY, _("Start Metal"), wxDefaultPosition, wxDefaultSize, 0 );
	m_metal_sizer->Add( m_metal_lbl, 0, wxALL, 5 );
	
	m_metal_slider = new wxSlider( this, wxID_ANY, 1000, 0, 10000, wxDefaultPosition, wxDefaultSize, wxSL_BOTH|wxSL_VERTICAL|wxSL_LABELS );
	m_metal_slider->SetToolTip( _("The amount of metal each player starts with.") );
	
	m_metal_sizer->Add( m_metal_slider, 1, wxALL, 5 );
	
	m_resources_box->Add( m_metal_sizer, 0, wxEXPAND, 5 );
	
	m_staticline2 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
	m_resources_box->Add( m_staticline2, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* m_energy_sizer;
	m_energy_sizer = new wxBoxSizer( wxVERTICAL );
	
	m_energy_lbl = new wxStaticText( this, wxID_ANY, _("Start Energy"), wxDefaultPosition, wxDefaultSize, 0 );
	m_energy_sizer->Add( m_energy_lbl, 0, wxALL, 5 );
	
	m_energy_slider = new wxSlider( this, wxID_ANY, 1000, 0, 10000, wxDefaultPosition, wxDefaultSize, wxSL_BOTH|wxSL_VERTICAL|wxSL_LABELS );
	m_energy_slider->SetToolTip( _("The amount of energy each player starts with.") );
	
	m_energy_sizer->Add( m_energy_slider, 1, wxALL, 5 );
	
	m_resources_box->Add( m_energy_sizer, 1, wxEXPAND, 5 );
	
	m_staticline21 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
	m_resources_box->Add( m_staticline21, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* m_units_sizer;
	m_units_sizer = new wxBoxSizer( wxVERTICAL );
	
	m_units_lbl = new wxStaticText( this, wxID_ANY, _("Max units"), wxDefaultPosition, wxDefaultSize, 0 );
	m_units_sizer->Add( m_units_lbl, 0, wxALL, 5 );
	
  m_units_slider = new wxSlider( this, wxID_ANY, 500, 10, 5000, wxDefaultPosition, wxDefaultSize, wxSL_BOTH|wxSL_VERTICAL|wxSL_LABELS );
	m_units_slider->SetToolTip( _("The maximun number of units allowed per player.") );
	
	m_units_sizer->Add( m_units_slider, 1, wxALL, 5 );
	
	m_resources_box->Add( m_units_sizer, 1, wxEXPAND, 5 );
	
	m_main_options_sizer->Add( m_resources_box, 1, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* m_options_box;
	m_options_box = new wxStaticBoxSizer( new wxStaticBox( this, -1, _("Options") ), wxVERTICAL );
	
	wxString m_options_checksChoices[] = { _("Limit d-gun"), _("Ghosted buildning"), _("Diminishing metal makers"), _("Lock game speed") };
	int m_options_checksNChoices = sizeof( m_options_checksChoices ) / sizeof( wxString );
	m_options_checks = new wxCheckListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_options_checksNChoices, m_options_checksChoices, 0 );
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
	
	m_allowed_list = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT );
	m_allowed_list->SetToolTip( _("Units in this list will be available in the game.") );
	
	m_allowed_sizer->Add( m_allowed_list, 1, wxALL|wxEXPAND, 5 );
	
	m_top_restr_sizer->Add( m_allowed_sizer, 1, wxEXPAND, 5 );
	
	wxBoxSizer* m_mid_btn_sizer;
	m_mid_btn_sizer = new wxBoxSizer( wxVERTICAL );
	
	m_mid_btn_sizer->Add( 0, 50, 0, wxEXPAND, 0 );
	
	m_restrict_btn = new wxButton( this, wxID_ANY, _(">"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	m_mid_btn_sizer->Add( m_restrict_btn, 0, wxALL, 5 );
	
	m_allow_btn = new wxButton( this, wxID_ANY, _("<"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	m_mid_btn_sizer->Add( m_allow_btn, 0, wxALL, 5 );
	
	m_top_restr_sizer->Add( m_mid_btn_sizer, 0, wxEXPAND, 5 );
	
	wxBoxSizer* m_restricted_sizer;
	m_restricted_sizer = new wxBoxSizer( wxVERTICAL );
	
	m_restricted_lbl = new wxStaticText( this, wxID_ANY, _("Restricted units"), wxDefaultPosition, wxDefaultSize, 0 );
	m_restricted_sizer->Add( m_restricted_lbl, 0, wxALL, 5 );
	
	m_restrict_list = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT );
	m_restrict_list->SetToolTip( _("Units in this list will not be available in the game.") );
	
	m_restricted_sizer->Add( m_restrict_list, 1, wxALL|wxEXPAND, 5 );
	
	m_top_restr_sizer->Add( m_restricted_sizer, 1, wxEXPAND, 5 );
	
	m_restr_box->Add( m_top_restr_sizer, 1, wxEXPAND, 5 );
	
	wxBoxSizer* m_restricted_btn_sizer;
	m_restricted_btn_sizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_load_btn = new wxButton( this, wxID_ANY, _("Load..."), wxDefaultPosition, wxDefaultSize, 0 );
	m_load_btn->SetToolTip( _("Load a saved set of restrictions.") );
	
	m_restricted_btn_sizer->Add( m_load_btn, 0, wxALL, 5 );
	
	m_save_btn = new wxButton( this, wxID_ANY, _("Save..."), wxDefaultPosition, wxDefaultSize, 0 );
	m_save_btn->SetToolTip( _("Save a set of restrictions.") );
	
	m_restricted_btn_sizer->Add( m_save_btn, 0, wxALL, 5 );
	
	m_restricted_btn_sizer->Add( 0, 0, 1, wxEXPAND, 0 );
	
	m_clear_btn = new wxButton( this, wxID_ANY, _("Clear"), wxDefaultPosition, wxDefaultSize, 0 );
	m_clear_btn->SetToolTip( _("Enable all units.") );
	
	m_restricted_btn_sizer->Add( m_clear_btn, 0, wxALL, 5 );
	
	m_restr_box->Add( m_restricted_btn_sizer, 0, wxEXPAND, 5 );
	
	m_main_sizer->Add( m_restr_box, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( m_main_sizer );
	this->Layout();

}


BattleOptionsTab::~BattleOptionsTab()
{

}


