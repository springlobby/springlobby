/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: ConnectWindow
//

#include <wx/notebook.h>
#include <wx/panel.h>
#include <wx/tokenzr.h>
#include <wx/stattext.h>
#include <wx/combobox.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/intl.h>
#include <wx/settings.h>
#include <wx/icon.h>

#include "connectwindow.h"
#include "settings.h"
#include "ui.h"
#include "images/connect.xpm"
#include "utils.h"

#include "settings++/custom_dialogs.h"

// Define events.
BEGIN_EVENT_TABLE(ConnectWindow, wxDialog)

  EVT_BUTTON ( wxID_OK,     ConnectWindow::OnOk )
  EVT_BUTTON ( wxID_CANCEL, ConnectWindow::OnCancel )
  EVT_COMBOBOX ( CON_SERV_SEL , ConnectWindow::OnServerChange )

END_EVENT_TABLE()

//! @brief Constructor
//!
//! @param parent Parent window
ConnectWindow::ConnectWindow( wxWindow* parent, Ui& ui )
: wxDialog( parent, -1, _("Connect to lobby server"), wxDefaultPosition, wxSize(300, 300),
           wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN ), m_ui(ui)
{
  wxString server;
  wxString username;
  wxString password;
  bool savepass;

  SetIcon( wxIcon(connect_xpm) );

  server = WX_STRING( sett().GetDefaultServer() );
  username = WX_STRING( sett().GetServerAccountNick( sett().GetDefaultServer() ) );
  password = WX_STRING( sett().GetServerAccountPass( sett().GetDefaultServer() ) );
  savepass = sett().GetServerAccountSavePass( sett().GetDefaultServer() );
  // Create all UI elements.
  m_tabs =         new wxNotebook( this  , -1 );
  m_login_tab =    new wxPanel   ( m_tabs, -1 );
  m_register_tab = new wxPanel   ( m_tabs, -1 );

  m_server_lbl =   new wxStaticText( m_login_tab, -1, _("Server") );
  m_server_combo = new wxComboBox  ( m_login_tab, CON_SERV_SEL, server );
  m_server_combo->SetToolTip( _("Server to connect to. You can connect to any server you like by typing in hostaddress:port format.") );

  m_ser_acc_line = new wxStaticLine( m_login_tab );

  m_nick_lbl =    new wxStaticText( m_login_tab, -1, _("Nickname") );
  m_nick_text =   new wxTextCtrl  ( m_login_tab, -1, username );
  m_pass_lbl =    new wxStaticText( m_login_tab, -1, _("Password") );
  m_pass_text =   new wxTextCtrl  ( m_login_tab, -1, password, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD );
  m_rpass_check = new wxCheckBox  ( m_login_tab, -1, _T("Remember password") );

  m_rpass_check->SetValue( savepass );

  m_acc_note_line = new wxStaticLine( m_login_tab );

  m_note_lbl = new wxStaticText( m_login_tab, -1, _("Note: If you do not have an account, you\n can register one for free under the\n\"Register\" tab.") );

  m_ok_btn =     new wxButton( this, wxID_OK,     _("Ok") );
  m_cancel_btn = new wxButton( this, wxID_CANCEL, _("Cancel") );

  // Add tabs to tab control.
  m_tabs->AddPage( m_login_tab, _("Login"), true );
  m_tabs->AddPage( m_register_tab, _("Register"), false );

  // Create sizers.
  m_main_sizer = new wxBoxSizer( wxVERTICAL );
  m_login_main_sizer = new wxBoxSizer( wxVERTICAL );
  m_server_sizer = new wxBoxSizer( wxHORIZONTAL );
  m_nick_sizer = new wxBoxSizer( wxHORIZONTAL );
  m_pass_sizer = new wxBoxSizer( wxHORIZONTAL );
  m_rpass_sizer = new wxBoxSizer( wxHORIZONTAL );
  m_buttons_sizer = new wxBoxSizer( wxHORIZONTAL );

  // Add UI elements to sizers.
  m_buttons_sizer->Add( m_cancel_btn, 0, wxALL, 4 );
  m_buttons_sizer->AddStretchSpacer();
  m_buttons_sizer->Add( m_ok_btn, 0, wxALL, 4 );

  m_rpass_sizer->AddStretchSpacer();
  m_rpass_sizer->Add( m_rpass_check, 2, wxEXPAND | wxALL, 4 );

  m_pass_sizer->Add( m_pass_lbl, 1, wxEXPAND | wxALL, 4 );
  m_pass_sizer->Add( m_pass_text, 2, wxEXPAND | wxALL, 4 );

  m_nick_sizer->Add( m_nick_lbl, 1, wxEXPAND | wxALL, 4 );
  m_nick_sizer->Add( m_nick_text, 2, wxEXPAND | wxALL, 4 );

  m_server_sizer->Add( m_server_lbl, 1, wxEXPAND | wxALL, 4 );
  m_server_sizer->Add( m_server_combo, 2, wxEXPAND | wxALL, 4 );

  m_login_main_sizer->Add( m_server_sizer, 0, wxEXPAND );
  m_login_main_sizer->Add( m_ser_acc_line, 0, wxEXPAND | wxALL, 4 );
  m_login_main_sizer->Add( m_nick_sizer, 0, wxEXPAND );
  m_login_main_sizer->Add( m_pass_sizer, 0, wxEXPAND );
  m_login_main_sizer->Add( m_rpass_sizer, 0, wxEXPAND );
  m_login_main_sizer->Add( m_acc_note_line, 0, wxEXPAND | wxALL, 4 );
  m_login_main_sizer->Add( m_note_lbl, 0, wxEXPAND | wxALL, 4 );
  m_login_main_sizer->AddStretchSpacer();

  m_login_tab->SetSizer( m_login_main_sizer );

  m_main_sizer->Add( m_tabs, 1, wxEXPAND );
  m_main_sizer->Add( m_buttons_sizer, 0, wxEXPAND );


  // Register page
  wxBoxSizer* m_register_sizer = new wxBoxSizer( wxVERTICAL );

  wxBoxSizer* m_regnick_sizer = new wxBoxSizer( wxHORIZONTAL );

  m_regnick_lbl = new wxStaticText( m_register_tab, wxID_ANY, _("Nick"), wxDefaultPosition, wxDefaultSize, 0 );
  m_regnick_sizer->Add( m_regnick_lbl, 1, wxALL, 4 );

  m_regnick_text = new wxTextCtrl( m_register_tab, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
  m_regnick_sizer->Add( m_regnick_text, 1, wxALL, 4 );

  m_register_sizer->Add( m_regnick_sizer, 0, wxEXPAND, 4 );

  m_regpass_sep = new wxStaticLine( m_register_tab, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
  m_register_sizer->Add( m_regpass_sep, 0, wxALL|wxEXPAND, 4 );

  wxBoxSizer* m_regpass1_sizer = new wxBoxSizer( wxHORIZONTAL );

  m_regpass1_lbl = new wxStaticText( m_register_tab, wxID_ANY, _("Password"), wxDefaultPosition, wxDefaultSize, 0 );
  m_regpass1_sizer->Add( m_regpass1_lbl, 1, wxALL, 4 );

  m_regpass1_text = new wxTextCtrl( m_register_tab, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD );
  m_regpass1_sizer->Add( m_regpass1_text, 1, wxALL, 4 );

  m_register_sizer->Add( m_regpass1_sizer, 0, wxEXPAND, 4 );

  wxBoxSizer* m_regpass1_sizer1 = new wxBoxSizer( wxHORIZONTAL );

  m_regpass2_lbl = new wxStaticText( m_register_tab, wxID_ANY, wxT("Retype password"), wxDefaultPosition, wxDefaultSize, 0 );
  m_regpass1_sizer1->Add( m_regpass2_lbl, 1, wxALL, 4 );

  m_regpass2_text = new wxTextCtrl( m_register_tab, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD );
  m_regpass1_sizer1->Add( m_regpass2_text, 1, wxALL, 4 );

  m_register_sizer->Add( m_regpass1_sizer1, 0, wxEXPAND, 4 );

  m_register_tab->SetSizer( m_register_sizer );
  m_register_tab->Layout();
  m_register_sizer->Fit( m_register_tab );


  // Set sizer.
  SetSizer( m_main_sizer );

  if ( !username.empty() ) {
    m_pass_text->SetFocus();
  } else {
    m_nick_text->SetFocus();
  }

  m_ok_btn->SetDefault();
  Layout();
  m_main_sizer->SetSizeHints( this );
#ifdef __WXMSW__
  SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE  ) );
#endif

  ReloadServerList();
}


//! @brief Destructor
ConnectWindow::~ConnectWindow()
{

}


void ConnectWindow::ReloadServerList()
{
  m_server_combo->Clear();
  for ( int i = 0; i < sett().GetNumServers(); i++ ) {
    m_server_combo->AppendString( WX_STRING(sett().GetServerName( i )) );
  }
  m_server_combo->SetValue( WX_STRING(sett().GetDefaultServer()) );
}

void ConnectWindow::OnServerChange( wxCommandEvent& event )
{
  wxString HostAddress = m_server_combo->GetValue();
  if (!HostAddress.Contains(_T(":")) && HostAddress != wxString(DEFSETT_DEFAULT_SERVER, wxConvUTF8) ) HostAddress+= _T(":") + wxString::Format(_T("%d"), DEFSETT_DEFAULT_SERVER_PORT );
  m_nick_text->SetValue( WX_STRING( sett().GetServerAccountNick( STD_STRING( HostAddress ) ) ) );
  if ( sett().GetServerAccountSavePass( STD_STRING( HostAddress ) ) ) m_pass_text->SetValue ( WX_STRING ( sett().GetServerAccountPass ( STD_STRING ( HostAddress ) ) ) );
}


void ConnectWindow::OnOk(wxCommandEvent& event)
{
  Hide();
  wxString HostAddress = m_server_combo->GetValue();
  if (!HostAddress.Contains(_T(":")) && HostAddress != wxString(DEFSETT_DEFAULT_SERVER, wxConvUTF8) ) HostAddress+= _T(":") + wxString::Format(_T("%d"), DEFSETT_DEFAULT_SERVER_PORT ) ;
  if ( m_tabs->GetSelection() <= 0 ) {
    sett().SetDefaultServer( STD_STRING( HostAddress ) );
    sett().SetServerAccountNick( STD_STRING( HostAddress ), STD_STRING(m_nick_text->GetValue()) );
    sett().SetServerAccountSavePass( STD_STRING( HostAddress ), m_rpass_check->GetValue() );

    // We assume that the server is given as : "host:port" so we split based on ":"
    wxArrayString serverString = wxStringTokenize( HostAddress ,_T(":") );

    if ( serverString.GetCount() == 0 ) {
      wxLogWarning( _T("Invalid port or servername.") );
      customMessageBox(SL_MAIN_ICON, _("Invalid host/port or servername."), _("Invalid host"), wxOK );
      return;
    }

    if ( serverString.GetCount() == 2 ) {
      long port;
      if( !serverString[1].ToLong( &port ) ) {
        wxLogWarning( _T("Invalid port.") );
        customMessageBox(SL_MAIN_ICON, _("Invalid port."), _("Invalid port"), wxOK );
        return;
      }
      if( port < 1 || port > 65535) {
        wxLogWarning( _T("port number out of range") );
        customMessageBox(SL_MAIN_ICON, _("Port number out of range.\n\nIt must be an integer between 1 and 65535"), _("Invalid port"), wxOK );
        return;
      }
      sett().AddServer( STD_STRING( HostAddress ) );
      sett().SetServerHost( STD_STRING( HostAddress ),STD_STRING(serverString[0]) );
      sett().SetServerPort( STD_STRING( HostAddress ), (int)port );
    }

    if ( serverString.GetCount() != 1 && serverString.GetCount() != 2 ) {
      wxLogWarning( _T("invalid host/port.") );
      customMessageBox(SL_MAIN_ICON, _("Invalid host/port."), _("Invalid host"), wxOK );
      return;
    }

    sett().SaveSettings();
    ReloadServerList();

    m_ui.DoConnect( HostAddress, m_nick_text->GetValue(), m_pass_text->GetValue() );
  } else {
	  wxString* reason = new wxString();
	  if (m_regpass2_text->GetValue()!= m_regpass1_text->GetValue())
	  {
		  Show();
         wxLogWarning( _T("registration failed, reason: password/confirmation mismatch")  );
         customMessageBox(SL_MAIN_ICON,_("Registration failed, the reason was:\nPassword / confirmation mismatch") , _("Registration failed."), wxOK );
	  }
	  else if ( m_ui.DoRegister( HostAddress, m_regnick_text->GetValue(), m_regpass1_text->GetValue(),reason ) ) {
       m_tabs->SetSelection( 0 );
       m_nick_text->SetValue(m_regnick_text->GetValue());
       m_pass_text->SetValue(m_regpass1_text->GetValue());
       Show();
       customMessageBox(SL_MAIN_ICON, _("Registration successful,\nyou should now be able to login."), _("Registration successful"), wxOK );
    } else {
       Show();
       wxLogWarning( _T("registration failed, reason: %s"), (*reason).mb_str()  );
       customMessageBox(SL_MAIN_ICON,_("Registration failed, the reason was:\n")+(*reason) , _("Registration failed."), wxOK );
    }

  }
}

void ConnectWindow::OnCancel(wxCommandEvent& event)
{
  Hide();
}


