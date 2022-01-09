///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.10.1-0-g8feb16b3)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "AgreementDialogBase.h"

///////////////////////////////////////////////////////////////////////////

AgreementDialogBase::AgreementDialogBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* m_main_sizer;
	m_main_sizer = new wxBoxSizer( wxVERTICAL );

	m_main_sizer->SetMinSize( wxSize( 450,375 ) );
	m_text = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_RICH|wxTE_WORDWRAP );
	m_main_sizer->Add( m_text, 1, wxALL|wxEXPAND, 5 );

	m_verif_sep = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	m_main_sizer->Add( m_verif_sep, 0, wxEXPAND | wxALL, 5 );

	wxBoxSizer* m_verif_sizer;
	m_verif_sizer = new wxBoxSizer( wxHORIZONTAL );

	m_verif_lbl = new wxStaticText( this, wxID_ANY, _("Verification code - sent by email"), wxDefaultPosition, wxDefaultSize, 0 );
	m_verif_lbl->Wrap( -1 );
	m_verif_sizer->Add( m_verif_lbl, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_verif_text = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_BESTWRAP );
	m_verif_sizer->Add( m_verif_text, 1, wxALL, 5 );


	m_main_sizer->Add( m_verif_sizer, 0, wxEXPAND, 5 );

	m_staticline2 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	m_main_sizer->Add( m_staticline2, 0, wxEXPAND | wxALL, 5 );

	m_accept_lbl = new wxStaticText( this, wxID_ANY, _("Do you accept the terms of this agreement?"), wxDefaultPosition, wxDefaultSize, 0 );
	m_accept_lbl->Wrap( -1 );
	m_main_sizer->Add( m_accept_lbl, 0, wxALL, 5 );

	wxBoxSizer* m_button_sizer;
	m_button_sizer = new wxBoxSizer( wxHORIZONTAL );

	m_no_btn = new wxButton( this, wxID_ANY, _("No"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button_sizer->Add( m_no_btn, 0, wxALL, 5 );


	m_button_sizer->Add( 0, 0, 1, wxEXPAND, 0 );

	m_yes_btn = new wxButton( this, wxID_ANY, _("Yes"), wxDefaultPosition, wxDefaultSize, 0 );

	m_yes_btn->SetDefault();
	m_button_sizer->Add( m_yes_btn, 0, wxALL, 5 );


	m_main_sizer->Add( m_button_sizer, 0, wxEXPAND, 5 );


	this->SetSizer( m_main_sizer );
	this->Layout();
	m_main_sizer->Fit( this );

	this->Centre( wxBOTH );

	// Connect Events
	m_no_btn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AgreementDialogBase::OnNo ), NULL, this );
	m_yes_btn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AgreementDialogBase::OnYes ), NULL, this );
}

AgreementDialogBase::~AgreementDialogBase()
{
	// Disconnect Events
	m_no_btn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AgreementDialogBase::OnNo ), NULL, this );
	m_yes_btn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AgreementDialogBase::OnYes ), NULL, this );

}
