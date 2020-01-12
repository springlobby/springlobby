///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.9.0 Jan 11 2020)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "ConnectWindowBase.h"

///////////////////////////////////////////////////////////////////////////

ConnectWindowBase::ConnectWindowBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 200,386 ), wxDefaultSize );

	wxBoxSizer* m_main_sizer;
	m_main_sizer = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* m_mode_switch_sizer;
	m_mode_switch_sizer = new wxBoxSizer( wxHORIZONTAL );


	m_mode_switch_sizer->Add( 0, 0, 1, wxEXPAND, 5 );

	m_loginreg_button = new wxButton( this, wxID_ANY, _("see code"), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE|wxBU_EXACTFIT );
	m_loginreg_button->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ) );

	m_mode_switch_sizer->Add( m_loginreg_button, 0, wxALL|wxEXPAND, 0 );


	m_main_sizer->Add( m_mode_switch_sizer, 1, wxEXPAND, 0 );

	m_server_label = new wxStaticText( this, wxID_ANY, _("Lobby server:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_server_label->Wrap( -1 );
	m_main_sizer->Add( m_server_label, 0, wxALL, 5 );

	m_server_combo = new wxComboBox( this, wxID_ANY, _("Official Server"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	m_server_combo->SetToolTip( _("Server to connect to. You can connect to any server you like by typing in hostaddress:port format.") );

	m_main_sizer->Add( m_server_combo, 0, wxEXPAND|wxLEFT|wxRIGHT, 10 );

	m_email_label = new wxStaticText( this, wxID_ANY, _("E-mail address:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_email_label->Wrap( -1 );
	m_email_label->SetToolTip( _("A verification code will be sent to your email address.") );

	m_main_sizer->Add( m_email_label, 0, wxALL, 5 );

	m_email_text = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_email_text->SetToolTip( _("A verification code will be sent to your email address.") );

	m_main_sizer->Add( m_email_text, 0, wxRIGHT|wxLEFT|wxEXPAND, 10 );

	m_nickname_label = new wxStaticText( this, wxID_ANY, _("Nickname:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_nickname_label->Wrap( -1 );
	m_main_sizer->Add( m_nickname_label, 0, wxALL, 5 );

	m_nickname_text = new wxTextCtrl( this, wxID_ANY, _("NewPlayer012356"), wxDefaultPosition, wxDefaultSize, 0 );
	#ifdef __WXGTK__
	if ( !m_nickname_text->HasFlag( wxTE_MULTILINE ) )
	{
	m_nickname_text->SetMaxLength( 16 );
	}
	#else
	m_nickname_text->SetMaxLength( 16 );
	#endif
	m_nickname_text->SetToolTip( _("Can be changed later") );

	m_main_sizer->Add( m_nickname_text, 0, wxEXPAND|wxLEFT|wxRIGHT, 10 );

	m_password1_label = new wxStaticText( this, wxID_ANY, _("Password:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_password1_label->Wrap( -1 );
	m_main_sizer->Add( m_password1_label, 0, wxALL, 5 );

	m_password1_hidden_text = new wxTextCtrl( this, wxID_ANY, _("1111222233334444"), wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD );
	m_password1_hidden_text->SetToolTip( _("Can be changed later") );

	m_main_sizer->Add( m_password1_hidden_text, 0, wxEXPAND|wxLEFT|wxRIGHT, 10 );

	m_password1_plain_text = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_password1_plain_text->Hide();

	m_main_sizer->Add( m_password1_plain_text, 0, wxEXPAND|wxRIGHT|wxLEFT, 10 );

	m_password2_label = new wxStaticText( this, wxID_ANY, _("Verify password:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_password2_label->Wrap( -1 );
	m_password2_label->Hide();

	m_main_sizer->Add( m_password2_label, 0, wxALL, 5 );

	m_password2_hidden_text = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD );
	m_password2_hidden_text->Hide();

	m_main_sizer->Add( m_password2_hidden_text, 0, wxEXPAND|wxRIGHT|wxLEFT, 10 );

	m_password2_plain_text = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_password2_plain_text->Hide();

	m_main_sizer->Add( m_password2_plain_text, 0, wxEXPAND|wxRIGHT|wxLEFT, 10 );

	m_password_show_check = new wxCheckBox( this, wxID_ANY, _("Show password"), wxDefaultPosition, wxDefaultSize, 0 );
	m_password_show_check->Hide();

	m_main_sizer->Add( m_password_show_check, 0, wxLEFT|wxRIGHT, 10 );

	m_remember_password_check = new wxCheckBox( this, wxID_ANY, _("Remember password"), wxDefaultPosition, wxDefaultSize, 0 );
	m_remember_password_check->SetValue(true);
	m_main_sizer->Add( m_remember_password_check, 0, wxLEFT|wxRIGHT, 10 );

	m_autoconnect_check = new wxCheckBox( this, wxID_ANY, _("Autoconnect next time"), wxDefaultPosition, wxDefaultSize, 0 );
	m_autoconnect_check->SetValue(true);
	m_autoconnect_check->SetToolTip( _("remember connection details and automatically connect to server on next lobby startup") );

	m_main_sizer->Add( m_autoconnect_check, 0, wxLEFT|wxRIGHT, 10 );

	wxBoxSizer* m_buttons_sizer;
	m_buttons_sizer = new wxBoxSizer( wxHORIZONTAL );

	m_cancel_button = new wxButton( this, wxID_ANY, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttons_sizer->Add( m_cancel_button, 1, wxEXPAND|wxALL, 5 );

	m_ok_button = new wxButton( this, wxID_ANY, _("Login"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttons_sizer->Add( m_ok_button, 1, wxALL|wxEXPAND, 5 );


	m_main_sizer->Add( m_buttons_sizer, 0, wxEXPAND|wxRIGHT|wxLEFT, 5 );


	this->SetSizer( m_main_sizer );
	this->Layout();
	m_main_sizer->Fit( this );

	this->Centre( wxBOTH );

	// Connect Events
	m_loginreg_button->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ConnectWindowBase::OnChangeMode ), NULL, this );
	m_cancel_button->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ConnectWindowBase::OnCancel ), NULL, this );
	m_ok_button->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ConnectWindowBase::OnOk ), NULL, this );
}

ConnectWindowBase::~ConnectWindowBase()
{
	// Disconnect Events
	m_loginreg_button->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ConnectWindowBase::OnChangeMode ), NULL, this );
	m_cancel_button->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ConnectWindowBase::OnCancel ), NULL, this );
	m_ok_button->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ConnectWindowBase::OnOk ), NULL, this );

}
