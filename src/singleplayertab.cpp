/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */


#include <wx/intl.h>
#include <wx/sizer.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/statline.h>

#include "singleplayertab.h"
#include "mapctrl.h"
#include "utils.h"
#include "uiutils.h"
#include "iunitsync.h"


BEGIN_EVENT_TABLE(SinglePlayerTab, wxPanel)

  EVT_CHOICE( SP_MAP_PICK, SinglePlayerTab::OnMapSelect )

END_EVENT_TABLE()


SinglePlayerTab::SinglePlayerTab(wxWindow* parent, Ui& ui):
  wxPanel( parent, -1 ),
  m_ui( ui ),
  m_battle( ui )
{
	wxBoxSizer* m_main_sizer = new wxBoxSizer( wxVERTICAL );

	m_minimap = new MapCtrl( this, 100, &m_battle, ui, false, false, true );
	m_main_sizer->Add( m_minimap, 1, wxALL|wxEXPAND, 5 );

	wxBoxSizer* m_ctrl_sizer = new wxBoxSizer( wxHORIZONTAL );

	wxString m_map_pickChoices[] = {  };
	int m_map_pickNChoices = sizeof( m_map_pickChoices ) / sizeof( wxString );
	m_map_pick = new wxChoice( this, SP_MAP_PICK, wxDefaultPosition, wxDefaultSize, m_map_pickNChoices, m_map_pickChoices, 0 );
	m_ctrl_sizer->Add( m_map_pick, 8, wxALL, 5 );

	m_select_btn = new wxButton( this, SP_BROWSE_MAP, _("..."), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	m_ctrl_sizer->Add( m_select_btn, 0, wxBOTTOM|wxRIGHT|wxTOP, 5 );

	m_ctrl_sizer->Add( 0, 0, 1, wxEXPAND, 0 );

	m_addbot_btn = new wxButton( this, SP_ADD_BOT, _("Add bot..."), wxDefaultPosition, wxSize(80, CONTROL_HEIGHT), 0 );
	m_ctrl_sizer->Add( m_addbot_btn, 0, wxALL, 5 );

	m_main_sizer->Add( m_ctrl_sizer, 0, wxEXPAND, 5 );

	m_buttons_sep = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	m_main_sizer->Add( m_buttons_sep, 0, wxLEFT|wxRIGHT|wxEXPAND, 5 );

	wxBoxSizer* m_buttons_sizer = new wxBoxSizer( wxHORIZONTAL );

	m_reset_btn = new wxButton( this, SP_RESET, _("Reset"), wxDefaultPosition, wxSize(80, CONTROL_HEIGHT), 0 );
	m_buttons_sizer->Add( m_reset_btn, 0, wxALL, 5 );

	m_buttons_sizer->Add( 0, 0, 1, wxEXPAND, 0 );

	m_start_btn = new wxButton( this, SP_START, _("Start"), wxDefaultPosition, wxSize(80, CONTROL_HEIGHT), 0 );
	m_buttons_sizer->Add( m_start_btn, 0, wxALL, 5 );

	m_main_sizer->Add( m_buttons_sizer, 0, wxEXPAND, 5 );

	this->SetSizer( m_main_sizer );
	this->Layout();

	ReloadMaplist();

	if ( m_map_pick->GetCount() > 0 ) {
	  m_map_pick->SetSelection(0);
    UnitSyncMap map = usync()->GetMap( 0, true );
    m_battle.SetMap( map );
    m_minimap->UpdateMinimap();
	}

}


SinglePlayerTab::~SinglePlayerTab()
{

}


void SinglePlayerTab::ReloadMaplist()
{
  m_map_pick->Clear();
  for ( int i = 0; i < usync()->GetNumMaps(); i++ ) {
    m_map_pick->Insert( RefineMapname( WX_STRING(usync()->GetMap( i, false ).name) ), i );
  }
}



void SinglePlayerTab::OnMapSelect( wxCommandEvent& event )
{
  int index = m_map_pick->GetCurrentSelection();

  UnitSyncMap map = usync()->GetMap( index, true );
  m_battle.SetMap( map );

  m_minimap->UpdateMinimap();
}
