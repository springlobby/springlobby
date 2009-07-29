///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 17 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "simplefrontbase.h"

///////////////////////////////////////////////////////////////////////////

SimpleFrontBase::SimpleFrontBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	this->SetExtraStyle( wxFRAME_EX_METAL );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	
	bSizer1->Add( 0, 0, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	m_sp = new wxButton( this, wxID_ANY, _("Singleplayer"), wxDefaultPosition, wxDefaultSize, 0 );
	m_sp->SetDefault(); 
	bSizer3->Add( m_sp, 0, wxALL, 5 );
	
	m_mp = new wxButton( this, wxID_ANY, _("Multiplayer"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_mp, 0, wxALL, 5 );
	
	m_settings = new wxButton( this, wxID_ANY, _("Settings"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_settings, 0, wxALL, 5 );
	
	m_exit = new wxButton( this, wxID_ANY, _("Exit"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_exit, 0, wxALL, 5 );
	
	bSizer1->Add( bSizer3, 0, wxALIGN_CENTER|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );
	
	
	bSizer1->Add( 0, 0, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_sp->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SimpleFrontBase::OnSingleplayer ), NULL, this );
	m_mp->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SimpleFrontBase::OnMultiplayer ), NULL, this );
	m_settings->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SimpleFrontBase::OnSettings ), NULL, this );
	m_exit->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SimpleFrontBase::OnExit ), NULL, this );
}

SimpleFrontBase::~SimpleFrontBase()
{
	// Disconnect Events
	m_sp->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SimpleFrontBase::OnSingleplayer ), NULL, this );
	m_mp->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SimpleFrontBase::OnMultiplayer ), NULL, this );
	m_settings->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SimpleFrontBase::OnSettings ), NULL, this );
	m_exit->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SimpleFrontBase::OnExit ), NULL, this );
}
