/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */


#include <wx/intl.h>
#include <wx/sizer.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/statline.h>

#include "singleplayertab.h"
#include "mapctrl.h"


SinglePlayerTab::SinglePlayerTab(wxWindow* parent, Ui& ui):
  wxPanel( parent, -1 ),
  m_ui( ui ),
  m_battle( ui )
{
	wxBoxSizer* m_main_sizer = new wxBoxSizer( wxVERTICAL );

	m_minimap = new MapCtrl( this, 100, &m_battle, ui );
	m_main_sizer->Add( m_minimap, 1, wxALL|wxEXPAND, 5 );

	wxBoxSizer* m_ctrl_sizer = new wxBoxSizer( wxHORIZONTAL );

	wxString m_map_pickChoices[] = {  };
	int m_map_pickNChoices = sizeof( m_map_pickChoices ) / sizeof( wxString );
	m_map_pick = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_map_pickNChoices, m_map_pickChoices, 0 );
	m_ctrl_sizer->Add( m_map_pick, 8, wxALL, 5 );

	m_select_btn = new wxButton( this, wxID_ANY, _("..."), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	m_ctrl_sizer->Add( m_select_btn, 0, wxBOTTOM|wxRIGHT|wxTOP, 5 );

	m_ctrl_sizer->Add( 0, 0, 1, wxEXPAND, 0 );

	m_addbot_btn = new wxButton( this, wxID_ANY, _("Add bot..."), wxDefaultPosition, wxDefaultSize, 0 );
	m_ctrl_sizer->Add( m_addbot_btn, 0, wxALL, 5 );

	m_main_sizer->Add( m_ctrl_sizer, 0, wxEXPAND, 5 );

	m_buttons_sep = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	m_main_sizer->Add( m_buttons_sep, 0, wxEXPAND, 5 );

	wxBoxSizer* m_buttons_sizer = new wxBoxSizer( wxHORIZONTAL );

	m_reset_btn = new wxButton( this, wxID_ANY, _("Reset"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttons_sizer->Add( m_reset_btn, 0, wxALL, 5 );

	m_buttons_sizer->Add( 0, 0, 1, wxEXPAND, 0 );

	m_start_btn = new wxButton( this, wxID_ANY, _("Start"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttons_sizer->Add( m_start_btn, 0, wxALL, 5 );

	m_main_sizer->Add( m_buttons_sizer, 0, wxEXPAND, 5 );

	this->SetSizer( m_main_sizer );
	this->Layout();
}


SinglePlayerTab::~SinglePlayerTab()
{

}

