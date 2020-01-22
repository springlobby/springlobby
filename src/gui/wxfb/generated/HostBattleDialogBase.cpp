///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.9.0 Jan 16 2020)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "HostBattleDialogBase.h"

///////////////////////////////////////////////////////////////////////////

HostBattleDialogBase::HostBattleDialogBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	m_main_sizer = new wxBoxSizer( wxVERTICAL );

	m_top_sizer = new wxFlexGridSizer( 0, 2, 0, 0 );
	m_top_sizer->AddGrowableCol( 1 );
	m_top_sizer->SetFlexibleDirection( wxBOTH );
	m_top_sizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_top_sizer->SetMinSize( wxSize( 350,-1 ) );
	m_battle_name_label = new wxStaticText( this, wxID_ANY, _("Battle name:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_battle_name_label->Wrap( -1 );
	m_battle_name_label->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );

	m_top_sizer->Add( m_battle_name_label, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxALIGN_RIGHT, 5 );

	m_battle_name_text = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_battle_name_text->SetToolTip( _("A short description of the game, this will show up in the battle list.") );

	m_top_sizer->Add( m_battle_name_text, 1, wxEXPAND|wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_game_label = new wxStaticText( this, wxID_ANY, _("Game:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_game_label->Wrap( -1 );
	m_game_label->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );

	m_top_sizer->Add( m_game_label, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );

	wxArrayString m_game_choiceChoices;
	m_game_choice = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_game_choiceChoices, 0 );
	m_game_choice->SetSelection( 0 );
	m_game_choice->SetToolTip( _("Select the game to play.") );

	m_top_sizer->Add( m_game_choice, 1, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );

	m_engine_label = new wxStaticText( this, wxID_ANY, _("Engine:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_engine_label->Wrap( -1 );
	m_engine_label->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );

	m_top_sizer->Add( m_engine_label, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );

	wxArrayString m_engine_choiceChoices;
	m_engine_choice = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_engine_choiceChoices, 0 );
	m_engine_choice->SetSelection( 0 );
	m_engine_choice->SetToolTip( _("Select the engine version to play.") );

	m_top_sizer->Add( m_engine_choice, 1, wxEXPAND|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_password_label = new wxStaticText( this, wxID_ANY, _("Password:\n(no spaces)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_password_label->Wrap( -1 );
	m_password_label->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );

	m_top_sizer->Add( m_password_label, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );

	m_password_text = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_password_text->SetToolTip( _("Password needed to join game. Keep empty for no password") );

	m_top_sizer->Add( m_password_text, 1, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );

	m_port_label = new wxStaticText( this, wxID_ANY, _("Port:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_port_label->Wrap( -1 );
	m_port_label->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );

	m_top_sizer->Add( m_port_label, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );

	m_port_spinctrl = new wxSpinCtrl( this, wxID_ANY, wxT("8452"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 65535, 8452 );
	m_port_spinctrl->SetToolTip( _("UDP port to host game on. Default is 8452. Make sure to open your firewall and re-configure your router.") );

	m_top_sizer->Add( m_port_spinctrl, 1, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );

	m_use_relayhost_check = new wxCheckBox( this, wxID_ANY, _("Use relayhost"), wxDefaultPosition, wxDefaultSize, 0 );
	m_use_relayhost_check->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );
	m_use_relayhost_check->SetToolTip( _("host and control game on remote server, helps if you have trouble hosting") );

	m_top_sizer->Add( m_use_relayhost_check, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );

	m_relayhost_pick = new wxButton( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	m_top_sizer->Add( m_relayhost_pick, 1, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );


	m_main_sizer->Add( m_top_sizer, 0, wxEXPAND, 5 );

	wxString m_nat_traversal_radiosChoices[] = { _("None"), _("Hole punching") };
	int m_nat_traversal_radiosNChoices = sizeof( m_nat_traversal_radiosChoices ) / sizeof( wxString );
	m_nat_traversal_radios = new wxRadioBox( this, wxID_ANY, _("NAT traversal:"), wxDefaultPosition, wxDefaultSize, m_nat_traversal_radiosNChoices, m_nat_traversal_radiosChoices, 1, wxRA_SPECIFY_ROWS );
	m_nat_traversal_radios->SetSelection( 0 );
	m_nat_traversal_radios->SetToolTip( _("NAT traversal method to use. Use none and reconfigure firewall/router if hole punching does not work.") );

	m_main_sizer->Add( m_nat_traversal_radios, 0, wxEXPAND|wxALL, 5 );

	m_rank_sizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Player rank limit:") ), wxHORIZONTAL );

	wxString m_rank_directionChoices[] = { _("Require at least"), _("Allow no greater than") };
	int m_rank_directionNChoices = sizeof( m_rank_directionChoices ) / sizeof( wxString );
	m_rank_direction = new wxChoice( m_rank_sizer->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, m_rank_directionNChoices, m_rank_directionChoices, 0 );
	m_rank_direction->SetSelection( 0 );
	m_rank_sizer->Add( m_rank_direction, 0, wxALL, 5 );

	m_rank_combo = new wxBitmapComboBox( m_rank_sizer->GetStaticBox(), wxID_ANY, _("Combo!"), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY );
	m_rank_sizer->Add( m_rank_combo, 1, wxALL, 5 );


	m_main_sizer->Add( m_rank_sizer, 0, wxEXPAND|wxALL, 5 );

	m_max_players_box = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Maximum number of players:") ), wxVERTICAL );

	m_max_players_slider = new wxSlider( m_max_players_box->GetStaticBox(), wxID_ANY, 8, 2, 16, wxDefaultPosition, wxDefaultSize, wxSL_AUTOTICKS|wxSL_HORIZONTAL|wxSL_VALUE_LABEL );
	m_max_players_slider->SetToolTip( _("The maximum number of players to allow in the battle. You may need special permission to host more than 8.") );

	m_max_players_box->Add( m_max_players_slider, 0, wxALL|wxEXPAND, 5 );


	m_main_sizer->Add( m_max_players_box, 0, wxEXPAND|wxALL, 5 );


	m_main_sizer->Add( 0, 0, 0, 0, 0 );

	m_buttons_sep = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	m_main_sizer->Add( m_buttons_sep, 0, wxEXPAND | wxALL, 2 );

	wxBoxSizer* m_buttons_sizer;
	m_buttons_sizer = new wxBoxSizer( wxHORIZONTAL );


	m_buttons_sizer->Add( 0, 0, 1, wxEXPAND, 5 );

	m_cancel_button = new wxButton( this, wxID_ANY, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttons_sizer->Add( m_cancel_button, 0, wxALL, 5 );

	m_host_button = new wxButton( this, wxID_ANY, _("Host!"), wxDefaultPosition, wxDefaultSize, 0 );
	m_host_button->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );
	m_host_button->SetToolTip( _("Start hosting the battle.") );

	m_buttons_sizer->Add( m_host_button, 0, wxALL, 5 );


	m_main_sizer->Add( m_buttons_sizer, 1, wxEXPAND, 5 );


	this->SetSizer( m_main_sizer );
	this->Layout();
	m_main_sizer->Fit( this );

	this->Centre( wxBOTH );

	// Connect Events
	m_engine_choice->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( HostBattleDialogBase::OnEngineSelect ), NULL, this );
	m_use_relayhost_check->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( HostBattleDialogBase::OnUseRelay ), NULL, this );
	m_relayhost_pick->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( HostBattleDialogBase::OnPickRelayHost ), NULL, this );
	m_nat_traversal_radios->Connect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( HostBattleDialogBase::OnNatChange ), NULL, this );
	m_cancel_button->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( HostBattleDialogBase::OnCancel ), NULL, this );
	m_host_button->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( HostBattleDialogBase::OnOk ), NULL, this );
}

HostBattleDialogBase::~HostBattleDialogBase()
{
	// Disconnect Events
	m_engine_choice->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( HostBattleDialogBase::OnEngineSelect ), NULL, this );
	m_use_relayhost_check->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( HostBattleDialogBase::OnUseRelay ), NULL, this );
	m_relayhost_pick->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( HostBattleDialogBase::OnPickRelayHost ), NULL, this );
	m_nat_traversal_radios->Disconnect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( HostBattleDialogBase::OnNatChange ), NULL, this );
	m_cancel_button->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( HostBattleDialogBase::OnCancel ), NULL, this );
	m_host_button->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( HostBattleDialogBase::OnOk ), NULL, this );

}
