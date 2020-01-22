///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.9.0 Jan 11 2020)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "CrashReporterDialogBase.h"

///////////////////////////////////////////////////////////////////////////

CrashReporterDialogBase::CrashReporterDialogBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 400,600 ), wxDefaultSize );

	m_main_sizer = new wxBoxSizer( wxVERTICAL );

	m_report_text = new wxTextCtrl( this, wxID_ANY, _("<report>"), wxDefaultPosition, wxDefaultSize, wxTE_AUTO_URL|wxTE_BESTWRAP|wxTE_MULTILINE|wxTE_READONLY|wxTE_RICH );
	m_report_text->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_MENU ) );

	m_main_sizer->Add( m_report_text, 1, wxEXPAND|wxALL, 5 );

	m_report_file_name_label = new wxStaticText( this, wxID_ANY, _("Contents of ~/.spring/infolog.txt:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_report_file_name_label->Wrap( -1 );
	m_main_sizer->Add( m_report_file_name_label, 0, wxALL, 5 );

	m_report_file_text = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxTE_MULTILINE|wxTE_RICH );
	m_main_sizer->Add( m_report_file_text, 1, wxEXPAND|wxALL, 5 );

	wxBoxSizer* m_buttons_sizer;
	m_buttons_sizer = new wxBoxSizer( wxHORIZONTAL );

	m_bug_button = new wxButton( this, wxID_ANY, _("Report bug..."), wxDefaultPosition, wxDefaultSize, 0 );
	m_bug_button->SetToolTip( _("https://springrts.com/wiki/Bugs") );

	m_buttons_sizer->Add( m_bug_button, 0, wxALL, 5 );


	m_buttons_sizer->Add( 0, 0, 1, wxEXPAND, 5 );

	m_cancel_button = new wxButton( this, wxID_ANY, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttons_sizer->Add( m_cancel_button, 0, wxALL, 5 );

	m_safe_button = new wxButton( this, wxID_ANY, _("Re-run in safe mode"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttons_sizer->Add( m_safe_button, 0, wxALL, 5 );

	m_norm_button = new wxButton( this, wxID_ANY, _("Re-run normally"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttons_sizer->Add( m_norm_button, 0, wxALL, 5 );


	m_main_sizer->Add( m_buttons_sizer, 0, wxEXPAND, 5 );


	this->SetSizer( m_main_sizer );
	this->Layout();
	m_main_sizer->Fit( this );

	this->Centre( wxBOTH );

	// Connect Events
	m_bug_button->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CrashReporterDialogBase::OnClickBugReport ), NULL, this );
	m_cancel_button->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CrashReporterDialogBase::OnCancel ), NULL, this );
	m_safe_button->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CrashReporterDialogBase::OnSafe ), NULL, this );
	m_norm_button->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CrashReporterDialogBase::OnNorm ), NULL, this );
}

CrashReporterDialogBase::~CrashReporterDialogBase()
{
	// Disconnect Events
	m_bug_button->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CrashReporterDialogBase::OnClickBugReport ), NULL, this );
	m_cancel_button->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CrashReporterDialogBase::OnCancel ), NULL, this );
	m_safe_button->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CrashReporterDialogBase::OnSafe ), NULL, this );
	m_norm_button->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CrashReporterDialogBase::OnNorm ), NULL, this );

}
