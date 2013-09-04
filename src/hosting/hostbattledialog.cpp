/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: HostBattleDialog
//

#include "hostbattledialog.h"

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
#include <wx/menu.h>
#include <wx/image.h>
#include <wx/bmpbuttn.h>
#include <wx/scrolwin.h>


#include "settings.h"
#include "user.h"
#include "uiutils.h"
#include "utils/controls.h"
#include "utils/customdialogs.h"
#include "ui.h"
#include "server.h"


#include "images/rank0.xpm"
#include "images/rank1.xpm"
#include "images/rank2.xpm"
#include "images/rank3.xpm"
#include "images/rank4.xpm"
#include "images/rank5.xpm"
#include "images/rank6.xpm"
#include "images/rank7.xpm"
#include "images/arrow_refresh.png.h"

BEGIN_EVENT_TABLE( HostBattleDialog, wxDialog )

	EVT_BUTTON              ( HOST_CANCEL, 		HostBattleDialog::OnCancel    		)
	EVT_BUTTON              ( HOST_OK,     		HostBattleDialog::OnOk        		)
	EVT_BUTTON              ( BTN_REFRESH, 		HostBattleDialog::OnReloadMods		)
	EVT_BUTTON              ( PICK_RELAYHOST,	HostBattleDialog::OnPickRelayHost   )
	EVT_MENU				( wxID_ANY, 		HostBattleDialog::OnRelayChoice		)
	EVT_RADIOBOX            ( CHOSE_NAT,   		HostBattleDialog::OnNatChange 		)
	EVT_CHECKBOX            ( CHK_USE_RELAY,    HostBattleDialog::OnUseRelay        )

END_EVENT_TABLE()

HostBattleDialog::HostBattleDialog( wxWindow* parent )
	: wxDialog( parent, -1, _( "Host new battle" ), wxDefaultPosition, wxSize( 410, 441 ), wxRESIZE_BORDER | wxDEFAULT_DIALOG_STYLE ),
	WindowHintsPickle ( _T("hostbattledialog"), this, wxSize( 410, 441 ) ),
    m_last_relayhost( sett().GetLastRelayedHost() )
{
//	this->SetSizeHints( GetSize(), wxDefaultSize );

	m_panel = new wxScrolledWindow( this );
//		m_panel ->SetSizeHints( GetSize(), wxDefaultSize );
	wxBoxSizer* all_sizer = new wxBoxSizer( wxVERTICAL );

	wxFlexGridSizer* topsizer = new wxFlexGridSizer( 2,  0, 10);
//	topsizer->AddGrowableCol( 1, 1 );

//	SetSizeHints( wxDefaultSize, wxDefaultSize );
	wxBoxSizer* m_main_sizer;
	m_main_sizer = new wxBoxSizer( wxVERTICAL );

	m_desc_lbl = new wxStaticText( m_panel, wxID_ANY, _( "Description" ), wxDefaultPosition, wxDefaultSize, 0 );
	m_desc_lbl->Wrap( -1 );
	topsizer->Add( m_desc_lbl, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

//    wxBoxSizer* desc_sizer = new wxBoxSizer( wxVERTICAL );
	m_desc_text = new wxTextCtrl( m_panel, wxID_ANY, sett().GetLastHostDescription(), wxDefaultPosition, wxDefaultSize, 0 );
	m_desc_text->SetToolTip( TE( _( "A short description of the game, this will show up in the battle list." ) ) );
	topsizer ->Add( m_desc_text, 1, wxALL | wxEXPAND, 5 );

	m_desc_check = new wxCheckBox( m_panel, wxID_ANY, _( "Autopaste description" ) );
	m_desc_check->SetValue( sett().GetBattleLastAutoAnnounceDescription() );
	m_desc_check->SetToolTip( TE( _( "Automatically write the battle description when a user joins." ) ) );

    topsizer->AddStretchSpacer();
	topsizer->Add( m_desc_check, 0, wxLEFT, 5 );
//	topsizer->Add( desc_sizer , 0, wxEXPAND | wxALL, 0 );

	m_mod_lbl = new wxStaticText( m_panel, wxID_ANY, _( "Game" ), wxDefaultPosition, wxDefaultSize, 0 );
	m_mod_lbl->Wrap( -1 );
	topsizer->Add( m_mod_lbl, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5 );

	wxArrayString m_mod_picChoices;
	wxBoxSizer* mod_choice_button_sizer = new wxBoxSizer( wxHORIZONTAL );
	m_mod_pic = new wxChoice( m_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_mod_picChoices, 0 );
	m_mod_pic->SetToolTip( TE( _( "Select the game to play." ) ) );
	mod_choice_button_sizer->Add( m_mod_pic, 0, wxALL , 5 );

    wxBitmap mp = charArr2wxBitmap( arrow_refresh_png, sizeof( arrow_refresh_png ) );
	m_refresh_btn = new wxBitmapButton( m_panel, BTN_REFRESH, mp );
	mod_choice_button_sizer->Add( m_refresh_btn, 0, wxEXPAND|wxTOP|wxBOTTOM, 5 );

	topsizer->Add( mod_choice_button_sizer, 0,  wxEXPAND|wxALL ,1 );

	m_pwd_lbl = new wxStaticText( m_panel, wxID_ANY, _( "Password\n(no spaces)" ), wxDefaultPosition, wxDefaultSize, 0 );
	m_pwd_lbl->Wrap( -1 );
	topsizer->Add( m_pwd_lbl, 1, wxALL| wxALIGN_CENTER_VERTICAL, 5 );

	m_pwd_text = new wxTextCtrl( m_panel, wxID_ANY, sett().GetLastHostPassword(), wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD );
	m_pwd_text->SetToolTip( TE( _( "Password needed to join game. Keep empty for no password" ) ) );
	topsizer->Add( m_pwd_text, 1, wxALL| wxEXPAND, 5 );

	m_port_lbl = new wxStaticText( m_panel, wxID_ANY, _( "Port" ), wxDefaultPosition, wxDefaultSize, 0 );
	m_port_lbl->Wrap( -1 );
	topsizer->Add( m_port_lbl, 1, wxALL| wxALIGN_CENTER_VERTICAL, 5 );

	m_port_text = new wxTextCtrl( m_panel, wxID_ANY, wxFormat( _T( "%d" ) ) % sett().GetLastHostPort(), wxDefaultPosition, wxDefaultSize, 0 );
	m_port_text->SetToolTip( TE( _( "UDP port to host game on. Default is 8452." ) ) );
	topsizer->Add( m_port_text, 1, wxALL| wxEXPAND, 5 );

//	m_port_test_check = new wxCheckBox( m_panel, wxID_ANY, _("Test firewall"), wxDefaultPosition, wxDefaultSize, 0 );
//	m_port_test_check->SetValue( sett().GetTestHostPort() );
//	m_port_sizer->Add( m_port_test_check, 1, wxALL|wxEXPAND, 5 );



	m_relayed_host_check = new wxCheckBox( m_panel, CHK_USE_RELAY, _( "Use relayhost" ), wxDefaultPosition, wxDefaultSize, 0 );
	m_relayed_host_check->SetToolTip( TE( _( "host and control game on remote server, helps if you have trouble hosting" ) ) );
	m_relayed_host_pick = new wxButton( m_panel, PICK_RELAYHOST, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
	m_relayed_host_pick->SetLabel( m_last_relayhost.IsEmpty() ? _T("automatic") : m_last_relayhost );

	m_relayhost_list = new wxMenu();
	wxMenuItem* automatic_pick = new wxMenuItem( m_relayhost_list, AUTO_PICK_HOST, _("Chose automatically"), _("Randomly picks an available one"), wxITEM_RADIO );
	m_relayhost_list->Append( automatic_pick );
	automatic_pick->Check( m_last_relayhost.IsEmpty() );
	m_relayhost_list->AppendSeparator();
	wxMenuItem* manual_pick_relay = new wxMenuItem( m_relayhost_list, MANUAL_PICK_HOST, _("Manually enter the manager name"), _("You'll get prompted for the exact manager name"), wxITEM_RADIO );
	m_relayhost_list->Append( manual_pick_relay );
	m_relayhost_list->AppendSeparator();
	m_relayhost_array_list = serverSelector().GetServer().GetRelayHostList();
	for ( unsigned int i = 0; i < m_relayhost_array_list.GetCount(); i++ )
	{
		wxMenuItem* newitem = new wxMenuItem( m_relayhost_list, MANUAL_PICK_HOST + 1 + i, m_relayhost_array_list[i], _T("") , wxITEM_RADIO );
		m_relayhost_list->Append( newitem );
		newitem->Check( m_last_relayhost == m_relayhost_array_list[i] );
	}

	m_relayed_host_check->SetValue( sett().GetLastHostRelayedMode() );
	m_relayed_host_pick->Show( m_relayed_host_check->IsChecked() );

	topsizer->Add(  m_relayed_host_check, 1, wxALL | wxEXPAND, 5 );
	topsizer->Add(  m_relayed_host_pick, 0, wxALL | wxEXPAND, 5);

	m_main_sizer->Add( topsizer, 0, wxEXPAND, 0 );

	wxStaticBoxSizer* m_players_box;
	m_players_box = new wxStaticBoxSizer( new wxStaticBox( m_panel, -1, _( "Number of players" ) ), wxVERTICAL );

	m_players_box->SetMinSize( wxSize( -1, 60 ) );
	m_players_slide = new wxSlider( m_panel, wxID_ANY, sett().GetLastHostPlayerNum(), 2, SPRING_MAX_USERS, wxDefaultPosition, wxDefaultSize, wxSL_AUTOTICKS | wxSL_BOTH | wxSL_HORIZONTAL | wxSL_LABELS );
	m_players_slide->SetToolTip( TE( _( "The maximum number of players to allow in the battle." ) ) );
	m_players_box->Add( m_players_slide, 0, wxALL | wxEXPAND, 5 );

	m_main_sizer->Add( m_players_box, 0, wxALL | wxEXPAND, 5 );

	wxBoxSizer* m_pl_nat_sizer;
	m_pl_nat_sizer = new wxBoxSizer( wxHORIZONTAL );

	wxString m_nat_radiosChoices[] = { _( "None" ), _( "Hole punching" )/*, _("Fixed source ports")*/ };
	int m_nat_radiosNChoices = sizeof( m_nat_radiosChoices ) / sizeof( wxString );
	m_nat_radios = new wxRadioBox( m_panel, CHOSE_NAT, _( "NAT traversal" ), wxDefaultPosition, wxDefaultSize, m_nat_radiosNChoices, m_nat_radiosChoices, 1, wxRA_SPECIFY_COLS );
	m_nat_radios->SetSelection( sett().GetLastHostNATSetting() );

	//m_nat_radios->Enable( false );
	m_nat_radios->Enable( true );

	m_nat_radios->SetToolTip( TE( _( "NAT traversal to use." ) ) );

	m_pl_nat_sizer->Add( m_nat_radios, 1, wxALL | wxEXPAND, 5 );

	wxStaticBoxSizer* m_rank_box;
	m_rank_box = new wxStaticBoxSizer( new wxStaticBox( m_panel, -1, _( "Rank requirement" ) ), wxVERTICAL );
	wxArrayString rankFilterChoices;
	rankFilterChoices.Add(_("At least"));
	rankFilterChoices.Add(_("No greater than"));
	m_rank_direction = new wxChoice( m_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, rankFilterChoices, 0 );
	m_rank_direction->SetToolTip( TE( _( "Select the type of rank enforcement." ) ) );
	m_rank_box->Add(m_rank_direction);

	wxFlexGridSizer* m_rank_sizer;
	m_rank_sizer = new wxFlexGridSizer( 2, 6, 0, 0 );
	m_rank_sizer->SetFlexibleDirection( wxBOTH );

	m_rank0_radio = new wxRadioButton( m_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	m_rank_sizer->Add( m_rank0_radio, 0, wxALL, 5 );

	m_rank0_img = new wxStaticBitmap( m_panel, wxID_ANY, wxBitmap( rank0_xpm ), wxDefaultPosition, wxSize( 16, 16 ), 0 );
	m_rank_sizer->Add( m_rank0_img, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL, 5 );

	m_rank1_radio = new wxRadioButton( m_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_rank_sizer->Add( m_rank1_radio, 0, wxALL, 5 );

	m_rank1_img = new wxStaticBitmap( m_panel, wxID_ANY, wxBitmap( rank1_xpm ), wxDefaultPosition, wxSize( 16, 16 ), 0 );
	m_rank_sizer->Add( m_rank1_img, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL | wxALL, 2 );

	m_rank2_radio = new wxRadioButton( m_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_rank_sizer->Add( m_rank2_radio, 0, wxALL, 5 );

	m_rank2_img = new wxStaticBitmap( m_panel, wxID_ANY, wxBitmap( rank2_xpm ), wxDefaultPosition, wxSize( 16, 16 ), 0 );
	m_rank_sizer->Add( m_rank2_img, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL, 2 );

	m_rank3_radio = new wxRadioButton( m_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_rank_sizer->Add( m_rank3_radio, 0, wxALL, 5 );

	m_rank3_img = new wxStaticBitmap( m_panel, wxID_ANY, wxBitmap( rank3_xpm ), wxDefaultPosition, wxSize( 16, 16 ), 0 );
	m_rank_sizer->Add( m_rank3_img, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL, 2 );

	m_rank4_radio = new wxRadioButton( m_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_rank_sizer->Add( m_rank4_radio, 0, wxALL, 5 );

	m_rank4_img = new wxStaticBitmap( m_panel, wxID_ANY, wxBitmap( rank4_xpm ), wxDefaultPosition, wxSize( 16, 16 ), 0 );
	m_rank_sizer->Add( m_rank4_img, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL, 2 );

	m_rank5_radio = new wxRadioButton( m_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_rank_sizer->Add( m_rank5_radio, 0, wxALL, 5 );

	m_rank5_img = new wxStaticBitmap( m_panel, wxID_ANY, wxBitmap( rank5_xpm ), wxDefaultPosition, wxSize( 16, 16 ), 0 );
	m_rank_sizer->Add( m_rank5_img, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL, 5 );

	m_rank6_radio = new wxRadioButton( m_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_rank_sizer->Add( m_rank6_radio, 0, wxALL, 5 );

	m_rank6_img = new wxStaticBitmap( m_panel, wxID_ANY, wxBitmap( rank6_xpm ), wxDefaultPosition, wxSize( 16, 16 ), 0 );
	m_rank_sizer->Add( m_rank6_img, 0, wxALL, 5 );

	m_rank7_radio = new wxRadioButton( m_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_rank_sizer->Add( m_rank7_radio, 0, wxALL, 5 );

	m_rank7_img = new wxStaticBitmap( m_panel, wxID_ANY, wxBitmap( rank7_xpm ), wxDefaultPosition, wxSize( 16, 16 ), 0 );
	m_rank_sizer->Add( m_rank7_img, 0, wxALL, 5 );

	m_rank_box->Add( m_rank_sizer, 1, wxEXPAND, 5 );

	m_pl_nat_sizer->Add( m_rank_box, 1, wxALL | wxEXPAND, 5 );

	m_main_sizer->Add( m_pl_nat_sizer, 0, wxEXPAND, 5 );
	m_main_sizer->Add( 0, 0, 1, wxEXPAND, 0 );

	m_buttons_sep = new wxStaticLine( m_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	m_main_sizer->Add( m_buttons_sep, 0, wxALL | wxEXPAND, 2 );

	wxBoxSizer* m_buttons_sizer;
	m_buttons_sizer = new wxBoxSizer( wxHORIZONTAL );

	m_cancel_btn = new wxButton( m_panel, HOST_CANCEL, _( "Cancel" ), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttons_sizer->Add( 0, 0, 1, wxEXPAND, 0 );
	m_buttons_sizer->Add( m_cancel_btn, 0, wxALL, 5 );

	m_host_btn = new wxButton( m_panel, HOST_OK, _( "Host" ), wxDefaultPosition, wxDefaultSize, 0 );
	m_host_btn->SetToolTip( TE( _( "Start hosting the battle." ) ) );

	m_buttons_sizer->Add( m_host_btn, 0, wxALL, 5 );

	m_main_sizer->Add( m_buttons_sizer, 0, wxEXPAND, 5 );

	m_panel->SetSizer( m_main_sizer );
	all_sizer->Add( m_panel, 1, wxEXPAND, 0 );
	this->SetSizer( all_sizer );
	m_panel->SetScrollRate( SCROLL_RATE, SCROLL_RATE );
	this->Layout();
	m_host_btn->SetFocus();

	ReloadModList();
}


void HostBattleDialog::ReloadModList()
{
	m_mod_pic->Clear();

    wxArrayString modlist = LSL::Util::vectorToArrayString(
                LSL::usync().GetModList());

	size_t nummods = modlist.Count();
	for ( size_t i = 0; i < nummods; i++ ) m_mod_pic->Insert( modlist[i], i );

	wxString last = sett().GetLastHostMod();
	if ( last != wxEmptyString )
        m_mod_pic->SetSelection( m_mod_pic->FindString( last ) );

	if ( m_mod_pic->GetSelection() == wxNOT_FOUND )
        m_mod_pic->SetSelection( 0 );
}


void HostBattleDialog::OnOk( wxCommandEvent& /*unused*/ )
{
	if ( m_mod_pic->GetSelection() == wxNOT_FOUND ) {
		wxLogWarning( _T( "no mod selected" ) );
		customMessageBox( SL_MAIN_ICON, _( "You have to select a game first." ), _( "No game selected." ), wxOK );
		return;
	}
	if ( m_desc_text->GetValue().IsEmpty() ) m_desc_text->SetValue( _T( "(none)" ) );
	sett().SetLastHostDescription( m_desc_text->GetValue() );
	sett().SetLastHostMod( m_mod_pic->GetString( m_mod_pic->GetSelection() ) );
	wxString password = m_pwd_text->GetValue();
	password.Replace(_T(" "), _T(""));
	sett().SetLastHostPassword( password );
	long tmp = DEFSETT_SPRING_PORT;
	m_port_text->GetValue().ToLong( &tmp );
	sett().SetLastHostPort( tmp );
//  sett().SetTestHostPort( m_port_test_check->GetValue() );
	sett().SetTestHostPort( false );
	sett().SetLastHostPlayerNum( m_players_slide->GetValue() );
	sett().SetLastHostNATSetting( m_nat_radios->GetSelection() );
	sett().SetLastRankLimit( GetSelectedRank() );
	sett().SetLastHostRelayedMode( m_relayed_host_check->GetValue() );
	sett().SetBattleLastAutoAnnounceDescription( m_desc_check->GetValue() );
	sett().SetLastRelayedHost( m_last_relayhost );
	sett().SaveSettings();
	EndModal( wxID_OK );
}


void HostBattleDialog::OnCancel( wxCommandEvent& /*unused*/ )
{
	EndModal( wxID_CANCEL );
}


int HostBattleDialog::GetSelectedRank()
{
	int ret = 0;
	if ( m_rank0_radio->GetValue() ) ret = 0;
	if ( m_rank1_radio->GetValue() ) ret = 1;
	if ( m_rank2_radio->GetValue() ) ret = 2;
	if ( m_rank3_radio->GetValue() ) ret = 3;
	if ( m_rank4_radio->GetValue() ) ret = 4;
	if ( m_rank5_radio->GetValue() ) ret = 5;
	if ( m_rank6_radio->GetValue() ) ret = 6;
	if ( m_rank7_radio->GetValue() ) ret = 7;
	if ( m_rank_direction->GetSelection() > 0 ) ret = -ret -1;
	return ret;
}

void HostBattleDialog::OnNatChange( wxCommandEvent& /*unused*/  )
{
//  m_port_test_check->Enable( m_nat_radios->GetSelection() == 0 );
	m_port_text->Enable( m_nat_radios->GetSelection() == 0 );
}

void HostBattleDialog::OnReloadMods( wxCommandEvent&  )
{
    LSL::usync().ReloadUnitSyncLib();
    ReloadModList();
}

void HostBattleDialog::OnPickRelayHost( wxCommandEvent&  )
{
		PopupMenu( m_relayhost_list );
}

void HostBattleDialog::OnRelayChoice( wxCommandEvent& event )
{
		int index = event.GetId();
		if ( index == AUTO_PICK_HOST )
            m_last_relayhost = _T("");
		else if ( index == MANUAL_PICK_HOST ) {
			ui().AskText( _("Manually chose a manager"), _("Please type the nick of the manager you want to use ( case sensitive )"), m_last_relayhost );
		}
		else if ( !(index - MANUAL_PICK_HOST - 1 > int(m_relayhost_array_list.GetCount()) ) ){
			index = index - MANUAL_PICK_HOST - 1;
			m_last_relayhost = m_relayhost_array_list[index];
		}
		m_relayed_host_pick->SetLabel( m_last_relayhost.IsEmpty() ? _T("automatic") : m_last_relayhost );
}

void HostBattleDialog::OnUseRelay( wxCommandEvent&  )
{
    m_relayed_host_pick->Show( m_relayed_host_check->IsChecked() );
	m_port_text->Enable( !m_relayed_host_check->IsChecked() && m_nat_radios->GetSelection() == 0 );
	m_nat_radios->Enable( !m_relayed_host_check->IsChecked() );
    Layout();
}

namespace SL{
void RunHostBattleDialog( wxWindow* parent )
{
	HostBattleDialog dlg( parent );
	if ( dlg.ShowModal() == wxID_OK )
	{
		BattleOptions bo;
		bo.description = sett().GetLastHostDescription();
		bo.port = sett().GetLastHostPort();
		bo.nattype = NatType( sett().GetLastHostNATSetting() );

		if ( bo.nattype == NAT_None && sett().GetTestHostPort() )
		{
			switch ( ui().TestHostPort( bo.port ) )
			{
				case Server::porttest_pass :
					break; // success
				case Server::porttest_pass_WX26 :
					wxLogWarning( _T( "hosting port %d: test aborted (wx26)" ), bo.port  );
					customMessageBoxNoModal( SL_MAIN_ICON, wxFormat( _( "Your using wxWidgets prior to version 2.8,\n "
											 "port testing is not supported.\n Hosting may or may not work." ) ) % bo.port );
					sett().SetTestHostPort( false ); // no need to have it checked anymore
					break;

				case Server::porttest_unreachable :
					wxLogWarning( _T( "hosting port %d: test undetermined" ), bo.port  );
					customMessageBoxNoModal( SL_MAIN_ICON, wxFormat( _( "The server used for testing your port %d "
											 "is unreachable. \nHosting may or may not work with this setting." ) ) % bo.port );
					break; //inconclusive test shouldn't hinder hosting imo (koshi)

				case Server::porttest_timeout :
				case Server::porttest_socketNotOk :
				case Server::porttest_socketError :
					wxLogWarning( _T( "hosting port %d: test unsuccessful, closing battle" ), bo.port  );
					customMessageBoxNoModal( SL_MAIN_ICON, wxFormat( _( "Battle not started because the port you selected (%d) "
											 "is unable to-f /home/kosh/.springlobby/testing.conf recieve incoming packets\n checks your router & firewall configuration again or change port "
											 "in the dialog.\n\nIf everything else fails, enable the Hole Punching NAT Traversal\n "
											 "option in the hosting settings." ) ) % bo.port );
					return;
				default:
					wxLogWarning( _T( "unknonw port forward test result" ) );
					break;

			}
			if ( !ui().TestHostPort( bo.port ) )
			{
				wxLogWarning( _T( "hosting port %d: test unsuccessful, closing battle" ), bo.port  );
				customMessageBoxNoModal( SL_MAIN_ICON, wxFormat( _( "Battle not started because the port you selected (%d) "
										"is unable to recieve incoming packets\n checks your router & firewall configuration "
										"again or change port in the dialog.\n\nIf everything else fails, enable the Hole "
										"Punching NAT Traversal\n option in the hosting settings." ) ) % bo.port );
				return;
			}
		}

		// Get selected mod from unitsync.
		LSL::UnitsyncMod mod;
		try
		{
            mod = LSL::usync().GetMod(STD_STRING(sett().GetLastHostMod()));
            bo.modhash = TowxString(mod.hash);
            bo.modname = TowxString(mod.name);
		}
		catch ( ... )
		{
			wxLogWarning( _T( "can't host: game not found" ) );
			customMessageBoxNoModal( SL_MAIN_ICON, _( "Battle not started beacuse the game you selected could not be found. " ), _( "Error starting battle." ), wxOK );
			return;
		}

		LSL::UnitsyncMap map;
        const auto mname = STD_STRING(sett().GetLastHostMap());
		try {
			if ( LSL::usync().MapExists( mname ) )
				map = LSL::usync().GetMap( mname );
			else if ( LSL::usync().GetNumMaps() <= 0 )
			{
				wxLogWarning( _T( "no maps found" ) );
				customMessageBoxNoModal( SL_MAIN_ICON, _( "Couldn't find any maps in your spring installation. This could happen when you set the Spring settings incorrectly." ), _( "No maps found" ), wxOK );
				return;
			}
			else
			{
				map = LSL::usync().GetMap( 0 );
			}
		}
		catch ( ... )
		{
			wxLogWarning( _T( "no maps found" ) );
			customMessageBoxNoModal( SL_MAIN_ICON, _( "Couldn't find any maps in your spring installation. This could happen when you set the Spring settings incorrectly." ), _( "No maps found" ), wxOK );
			return;
		}
        bo.maphash = TowxString(map.hash);
        bo.mapname = TowxString(map.name);

		bo.rankneeded = sett().GetLastRankLimit();

		bo.maxplayers = sett().GetLastHostPlayerNum();
		bo.userelayhost = sett().GetLastHostRelayedMode();
		if ( bo.userelayhost ) bo.nattype = NAT_None;
		bo.relayhost = sett().GetLastRelayedHost();
		bo.engineName = _T("spring");
		bo.engineVersion = TowxString(LSL::usync().GetSpringVersion());

		serverSelector().GetServer().HostBattle( bo, sett().GetLastHostPassword() );
	}
}
} //end namespace SL
