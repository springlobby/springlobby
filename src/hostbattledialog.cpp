/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: HostBattleDialog
//

#include <wx/intl.h>
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
#include <wx/radiobut.h>
#include <wx/textctrl.h>
#include <wx/statbmp.h>
#include <wx/checkbox.h>
#include <wx/log.h>

#include "hostbattledialog.h"
#include "settings.h"
#include "utils.h"
#include "iunitsync.h"
#include "user.h"

#include "images/rank0.xpm"
#include "images/rank1.xpm"
#include "images/rank2.xpm"
#include "images/rank3.xpm"
#include "images/rank4.xpm"
#include "images/rank5.xpm"
#include "images/rank6.xpm"

#include "settings++/custom_dialogs.h"

BEGIN_EVENT_TABLE( HostBattleDialog, wxDialog )

  EVT_BUTTON              ( HOST_CANCEL, HostBattleDialog::OnCancel    )
  EVT_BUTTON              ( HOST_OK,     HostBattleDialog::OnOk        )
  EVT_RADIOBOX            ( CHOSE_NAT,   HostBattleDialog::OnNatChange )

END_EVENT_TABLE()

HostBattleDialog::HostBattleDialog( wxWindow* parent ): wxDialog( parent, -1, _("Host new battle"), wxDefaultPosition, wxSize( 385,441 ), wxDEFAULT_DIALOG_STYLE )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	SetSizeHints( wxDefaultSize, wxDefaultSize );
	wxBoxSizer* m_main_sizer;
	m_main_sizer = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* m_desc_sizer;
	m_desc_sizer = new wxBoxSizer( wxHORIZONTAL );

	m_desc_lbl = new wxStaticText( this, wxID_ANY, _("Description"), wxDefaultPosition, wxDefaultSize, 0 );
	m_desc_lbl->Wrap( -1 );
	m_desc_sizer->Add( m_desc_lbl, 1, wxALL, 5 );

	m_desc_text = new wxTextCtrl( this, wxID_ANY, sett().GetLastHostDescription(), wxDefaultPosition, wxDefaultSize, 0 );
	m_desc_text->SetToolTip( TE(_("A short description of the game, this will show up in the battle list.")) );

	m_desc_check = new wxCheckBox( this, wxID_ANY, _("Autopaste description") );
	m_desc_check->SetValue( sett().GetBattleLastAutoAnnounceDescription() );
	m_desc_check->SetToolTip( TE(_("Automatically write the battle description when a user joins.")) );

	m_desc_sizer->Add( m_desc_text, 2, wxALL, 5 );

	m_main_sizer->Add( m_desc_sizer, 0, wxEXPAND, 5 );
	m_main_sizer->Add(m_desc_check, 0);

	wxBoxSizer* m_mod_sizer;
	m_mod_sizer = new wxBoxSizer( wxHORIZONTAL );

	m_mod_lbl = new wxStaticText( this, wxID_ANY, _("Mod"), wxDefaultPosition, wxDefaultSize, 0 );
	m_mod_lbl->Wrap( -1 );
	m_mod_sizer->Add( m_mod_lbl, 1, wxALL, 5 );

	wxArrayString m_mod_picChoices;
	m_mod_pic = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_mod_picChoices, 0 );
	m_mod_pic->SetToolTip( TE(_("Select the mod to play with.")) );

	m_mod_sizer->Add( m_mod_pic, 2, wxALL, 5 );

	m_main_sizer->Add( m_mod_sizer, 0, wxEXPAND, 5 );

	wxBoxSizer* m_pwd_sizer;
	m_pwd_sizer = new wxBoxSizer( wxHORIZONTAL );

	m_pwd_lbl = new wxStaticText( this, wxID_ANY, _("Password"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pwd_lbl->Wrap( -1 );
	m_pwd_sizer->Add( m_pwd_lbl, 1, wxALL, 5 );

	m_pwd_text = new wxTextCtrl( this, wxID_ANY, sett().GetLastHostPassword(), wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD );
	m_pwd_text->SetToolTip( TE(_("Password needed to join game. Keep empty for no password")) );

	m_pwd_sizer->Add( m_pwd_text, 1, wxALL, 5 );
	m_pwd_sizer->Add( 0, 0, 1, wxEXPAND, 0 );

	m_main_sizer->Add( m_pwd_sizer, 0, wxEXPAND, 5 );

	wxBoxSizer* m_port_sizer;
	m_port_sizer = new wxBoxSizer( wxHORIZONTAL );

	m_port_lbl = new wxStaticText( this, wxID_ANY, _("Port"), wxDefaultPosition, wxDefaultSize, 0 );
	m_port_lbl->Wrap( -1 );
	m_port_sizer->Add( m_port_lbl, 1, wxALL, 5 );

	m_port_text = new wxTextCtrl( this, wxID_ANY, wxString::Format( _T("%d"), sett().GetLastHostPort()), wxDefaultPosition, wxDefaultSize, 0 );
	m_port_text->SetToolTip( TE(_("UDP port to host game on. Default is 8452.")) );

	m_port_sizer->Add( m_port_text, 1, wxALL, 5 );

	m_port_test_check = new wxCheckBox( this, wxID_ANY, _("Test firewall"), wxDefaultPosition, wxDefaultSize, 0 );
	m_port_test_check->SetValue( sett().GetTestHostPort() );

	m_port_sizer->Add( m_port_test_check, 1, wxALL|wxEXPAND, 5 );

	m_main_sizer->Add( m_port_sizer, 0, wxEXPAND, 5 );

	wxBoxSizer* m_relayed_sizer;
	m_relayed_sizer = new wxBoxSizer( wxHORIZONTAL );
	m_relayed_host_check = new wxCheckBox( this, wxID_ANY, _("Relay battle to an Autohost"), wxDefaultPosition, wxDefaultSize, 0 );
	m_relayed_host_check->SetToolTip( TE(_("host and control game on remote server, helps if you have trouble hosting")) );
  m_relayed_host_check->SetValue( sett().GetLastHostRelayedMode() );
  m_relayed_sizer->Add(  m_relayed_host_check, 1, wxALL|wxEXPAND, 5 );
  m_main_sizer->Add( m_relayed_sizer, 0, wxEXPAND, 5 );

	wxStaticBoxSizer* m_players_box;
	m_players_box = new wxStaticBoxSizer( new wxStaticBox( this, -1, _("Number of players") ), wxVERTICAL );

	m_players_box->SetMinSize( wxSize( -1,60 ) );
	m_players_slide = new wxSlider( this, wxID_ANY, sett().GetLastHostPlayerNum(), 2, SPRING_MAX_USERS, wxDefaultPosition, wxDefaultSize, wxSL_AUTOTICKS|wxSL_BOTH|wxSL_HORIZONTAL|wxSL_LABELS );
	m_players_slide->SetToolTip( TE(_("The maximum number of players to allow in the battle.")) );
	m_players_box->Add( m_players_slide, 0, wxALL|wxEXPAND, 5 );

	m_main_sizer->Add( m_players_box, 0, wxALL|wxEXPAND, 5 );

	wxBoxSizer* m_pl_nat_sizer;
	m_pl_nat_sizer = new wxBoxSizer( wxHORIZONTAL );

	wxString m_nat_radiosChoices[] = { _("None"), _("Hole punching")/*, _("Fixed source ports")*/ };
	int m_nat_radiosNChoices = sizeof( m_nat_radiosChoices ) / sizeof( wxString );
	m_nat_radios = new wxRadioBox( this, CHOSE_NAT, _("NAT traversal"), wxDefaultPosition, wxDefaultSize, m_nat_radiosNChoices, m_nat_radiosChoices, 1, wxRA_SPECIFY_COLS );
	m_nat_radios->SetSelection(sett().GetLastHostNATSetting());

	//m_nat_radios->Enable( false );
  m_nat_radios->Enable( true );

	m_nat_radios->SetToolTip( TE(_("NAT traversal to use.")) );

	m_pl_nat_sizer->Add( m_nat_radios, 1, wxALL|wxEXPAND, 5 );

	wxStaticBoxSizer* m_rank_box;
	m_rank_box = new wxStaticBoxSizer( new wxStaticBox( this, -1, _("Minimum Rank needed") ), wxVERTICAL );

	wxFlexGridSizer* m_rank_sizer;
	m_rank_sizer = new wxFlexGridSizer( 2, 6, 0, 0 );
	m_rank_sizer->SetFlexibleDirection( wxBOTH );

	m_rank0_radio = new wxRadioButton( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	m_rank_sizer->Add( m_rank0_radio, 0, wxALL, 5 );

	m_rank0_img = new wxStaticBitmap( this, wxID_ANY, wxBitmap( rank0_xpm ), wxDefaultPosition, wxSize( 16,16 ), 0 );
	m_rank_sizer->Add( m_rank0_img, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_rank1_radio = new wxRadioButton( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_rank_sizer->Add( m_rank1_radio, 0, wxALL, 5 );

	m_rank1_img = new wxStaticBitmap( this, wxID_ANY, wxBitmap( rank1_xpm ), wxDefaultPosition, wxSize( 16,16 ), 0 );
	m_rank_sizer->Add( m_rank1_img, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL|wxALL, 2 );

	m_rank2_radio = new wxRadioButton( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_rank_sizer->Add( m_rank2_radio, 0, wxALL, 5 );

	m_rank2_img = new wxStaticBitmap( this, wxID_ANY, wxBitmap( rank2_xpm ), wxDefaultPosition, wxSize( 16,16 ), 0 );
	m_rank_sizer->Add( m_rank2_img, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 2 );

	m_rank3_radio = new wxRadioButton( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_rank_sizer->Add( m_rank3_radio, 0, wxALL, 5 );

	m_rank3_img = new wxStaticBitmap( this, wxID_ANY, wxBitmap( rank3_xpm ), wxDefaultPosition, wxSize( 16,16 ), 0 );
	m_rank_sizer->Add( m_rank3_img, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 2 );

	m_rank4_radio = new wxRadioButton( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_rank_sizer->Add( m_rank4_radio, 0, wxALL, 5 );

	m_rank4_img = new wxStaticBitmap( this, wxID_ANY, wxBitmap( rank4_xpm ), wxDefaultPosition, wxSize( 16,16 ), 0 );
	m_rank_sizer->Add( m_rank4_img, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 2 );

	m_rank5_radio = new wxRadioButton( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_rank_sizer->Add( m_rank5_radio, 0, wxALL, 5 );

	m_rank5_img = new wxStaticBitmap( this, wxID_ANY, wxBitmap( rank5_xpm ), wxDefaultPosition, wxSize( 16,16 ), 0 );
	m_rank_sizer->Add( m_rank5_img, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 5 );

	m_rank6_radio = new wxRadioButton( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_rank_sizer->Add( m_rank6_radio, 0, wxALL, 5 );

	m_rank6_img = new wxStaticBitmap( this, wxID_ANY, wxBitmap( rank6_xpm ), wxDefaultPosition, wxSize( 16,16 ), 0 );
	m_rank_sizer->Add( m_rank6_img, 0, wxALL, 5 );

	m_rank_box->Add( m_rank_sizer, 1, wxEXPAND, 5 );

	m_pl_nat_sizer->Add( m_rank_box, 1, wxALL|wxEXPAND, 5 );

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
	m_host_btn->SetToolTip( TE(_("Start hosting the battle.")) );

	m_buttons_sizer->Add( m_host_btn, 0, wxALL, 5 );

	m_main_sizer->Add( m_buttons_sizer, 0, wxEXPAND, 5 );

	m_port_test_check->Disable();

	this->SetSizer( m_main_sizer );
	this->Layout();

    m_host_btn->SetFocus();

	ReloadModList();
}


void HostBattleDialog::ReloadModList()
{
  m_mod_pic->Clear();

  wxArrayString modlist= usync().GetModList();
  //modlist.Sort(CompareStringIgnoreCase);

  size_t nummods = modlist.Count();
  for ( size_t i = 0; i < nummods; i++ ) m_mod_pic->Insert( modlist[i], i );

  wxString last = sett().GetLastHostMod();
  if ( last != wxEmptyString ) m_mod_pic->SetSelection( m_mod_pic->FindString( last ) );
}


void HostBattleDialog::OnOk( wxCommandEvent& event )
{
  if ( m_mod_pic->GetSelection() == wxNOT_FOUND ) {
    wxLogWarning( _T("no mod selected") );
    customMessageBox(SL_MAIN_ICON, _("You have to select a mod first."), _("No mod selected."), wxOK );
    return;
  }
  if ( m_desc_text->GetValue().IsEmpty() ) m_desc_text->SetValue(_T("(none)"));
  sett().SetLastHostDescription( m_desc_text->GetValue() );
  sett().SetLastHostMod( m_mod_pic->GetString(m_mod_pic->GetSelection()) );
  sett().SetLastHostPassword( m_pwd_text->GetValue() );
  long tmp = DEFSETT_SPRING_PORT;
  m_port_text->GetValue().ToLong( &tmp );
  sett().SetLastHostPort( tmp );
  sett().SetTestHostPort( m_port_test_check->GetValue() );
  sett().SetLastHostPlayerNum( m_players_slide->GetValue() );
  sett().SetLastHostNATSetting( m_nat_radios->GetSelection() );
  sett().SetLastRankLimit( GetSelectedRank() );
  sett().SetLastHostRelayedMode( m_relayed_host_check->GetValue() );
  sett().SetBattleLastAutoAnnounceDescription( m_desc_check->GetValue() );
  sett().SaveSettings();
  EndModal( wxID_OK );
}


void HostBattleDialog::OnCancel( wxCommandEvent& event )
{
  EndModal( wxID_CANCEL );
}


int HostBattleDialog::GetSelectedRank()
{
  if ( m_rank0_radio->GetValue() ) return 0;
  if ( m_rank1_radio->GetValue() ) return 1;
  if ( m_rank2_radio->GetValue() ) return 2;
  if ( m_rank3_radio->GetValue() ) return 3;
  if ( m_rank4_radio->GetValue() ) return 4;
  if ( m_rank5_radio->GetValue() ) return 5;
  if ( m_rank6_radio->GetValue() ) return 6;
  return 000;
}

void HostBattleDialog::OnNatChange( wxCommandEvent& event  )
{
  m_port_test_check->Enable( m_nat_radios->GetSelection() == 0 );
  m_port_text->Enable( m_nat_radios->GetSelection() == 0 );
}
