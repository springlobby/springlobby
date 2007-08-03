//
// Class: HostBattleDialog
//

#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/choice.h>
#include <wx/slider.h>
#include <wx/statline.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/statline.h>
#include <wx/statbox.h>
#include <wx/radiobox.h>
#include <wx/textctrl.h>

#include "hostbattledialog.h"


HostBattleDialog::HostBattleDialog( wxWindow* parent, int id, wxString title, wxPoint pos, wxSize size, int style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* m_main_sizer;
	m_main_sizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* m_desc_sizer;
	m_desc_sizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_desc_lbl = new wxStaticText( this, wxID_ANY, _("Description"), wxDefaultPosition, wxDefaultSize, 0 );
	m_desc_sizer->Add( m_desc_lbl, 1, wxALL, 5 );
	
	m_desc_text = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_desc_text->SetToolTip( _("A short description of the game, this will show up in the battle list.") );
	
	m_desc_sizer->Add( m_desc_text, 2, wxALL, 5 );
	
	m_main_sizer->Add( m_desc_sizer, 0, wxEXPAND, 5 );
	
	wxBoxSizer* m_mod_sizer;
	m_mod_sizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_mod_lbl = new wxStaticText( this, wxID_ANY, _("Mod"), wxDefaultPosition, wxDefaultSize, 0 );
	m_mod_sizer->Add( m_mod_lbl, 1, wxALL, 5 );
	
	m_mod_pic = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize );
	m_mod_pic->SetToolTip( _("Select the mod to play with.") );
	
	m_mod_sizer->Add( m_mod_pic, 2, wxALL, 5 );
	
	m_main_sizer->Add( m_mod_sizer, 0, wxEXPAND, 5 );
	
	wxBoxSizer* m_pwd_sizer;
	m_pwd_sizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_pwd_lbl = new wxStaticText( this, wxID_ANY, _("Password"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pwd_sizer->Add( m_pwd_lbl, 1, wxALL, 5 );
	
	m_pwd_text = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD );
	m_pwd_text->SetToolTip( _("Password needed to join game. Keep empty for no password") );
	
	m_pwd_sizer->Add( m_pwd_text, 1, wxALL, 5 );
	
	m_pwd_sizer->Add( 0, 0, 1, wxEXPAND, 0 );
	
	m_main_sizer->Add( m_pwd_sizer, 0, wxEXPAND, 5 );
	
	wxBoxSizer* m_port_sizer;
	m_port_sizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_port_lbl = new wxStaticText( this, wxID_ANY, _("Port"), wxDefaultPosition, wxDefaultSize, 0 );
	m_port_sizer->Add( m_port_lbl, 1, wxALL, 5 );
	
	m_port_text = new wxTextCtrl( this, wxID_ANY, _("8452"), wxDefaultPosition, wxDefaultSize, 0 );
	m_port_text->SetToolTip( _("UDP port to host game on. Default is 8452.") );
	
	m_port_sizer->Add( m_port_text, 1, wxALL, 5 );
	
	m_port_sizer->Add( 0, 0, 1, wxEXPAND, 0 );
	
	m_main_sizer->Add( m_port_sizer, 0, wxEXPAND, 5 );
	
	wxBoxSizer* m_pl_nat_sizer;
	m_pl_nat_sizer = new wxBoxSizer( wxHORIZONTAL );
	
	wxStaticBoxSizer* m_players_box;
	m_players_box = new wxStaticBoxSizer( new wxStaticBox( this, -1, _("Number of players") ), wxVERTICAL );
	
	m_players_slide = new wxSlider( this, wxID_ANY, 4, 2, 16, wxDefaultPosition, wxDefaultSize, wxSL_AUTOTICKS|wxSL_BOTH|wxSL_HORIZONTAL|wxSL_LABELS );
	m_players_slide->SetToolTip( _("The maximun number of players to allow in the battle.") );
	
	m_players_box->Add( m_players_slide, 0, wxALL|wxEXPAND, 5 );
	
	m_pl_nat_sizer->Add( m_players_box, 1, wxALL|wxEXPAND, 5 );
	
	wxString m_nat_radiosChoices[] = { _("None"), _("Hole punching"), _("Fixed source ports") };
	int m_nat_radiosNChoices = sizeof( m_nat_radiosChoices ) / sizeof( wxString );
	m_nat_radios = new wxRadioBox( this, wxID_ANY, _("NAT traversal"), wxDefaultPosition, wxDefaultSize, m_nat_radiosNChoices, m_nat_radiosChoices, 1, wxRA_SPECIFY_COLS );
	m_nat_radios->Enable( false );
	m_nat_radios->SetToolTip( _("NAT traversal to use, currently this feature is not supported by SpringLobby.") );
	
	m_pl_nat_sizer->Add( m_nat_radios, 1, wxALL|wxEXPAND, 5 );
	
	m_main_sizer->Add( m_pl_nat_sizer, 0, wxEXPAND, 5 );
	
	m_main_sizer->Add( 0, 0, 1, wxEXPAND, 0 );
	
	m_buttons_sep = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	m_main_sizer->Add( m_buttons_sep, 0, wxALL|wxEXPAND, 2 );
	
	wxBoxSizer* m_buttons_sizer;
	m_buttons_sizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_cancel_btn = new wxButton( this, HOST_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttons_sizer->Add( m_cancel_btn, 0, wxALL, 5 );
	
	m_buttons_sizer->Add( 0, 0, 1, wxEXPAND, 0 );
	
	m_host_btn = new wxButton( this, HOST_OK, _("Host"), wxDefaultPosition, wxDefaultSize, 0 );
	m_host_btn->SetToolTip( _("Start hosting the battle.") );
	
	m_buttons_sizer->Add( m_host_btn, 0, wxALL, 5 );
	
	m_main_sizer->Add( m_buttons_sizer, 0, wxEXPAND, 5 );
	
	this->SetSizer( m_main_sizer );
	this->Layout();
}
