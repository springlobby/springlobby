//
// Class: ConnectWindow
//

#include "connectwindow.h"
#include "settings.h"
#include "system.h"


// Define events.
BEGIN_EVENT_TABLE(ConnectWindow, wxFrame)

  EVT_BUTTON ( wxID_OK,     ConnectWindow::OnOk )
  EVT_BUTTON ( wxID_CANCEL, ConnectWindow::OnCancel )

END_EVENT_TABLE()

//! @brief Constructor
//!
//! @param parent Parent window
ConnectWindow::ConnectWindow( wxWindow* parent ) 
: wxFrame( parent, -1, _T("Connect to lobby server"), wxDefaultPosition, wxSize(300, 300), 
           wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN )
{
  wxString server;
  wxString username;
  wxString password;
  
  server = WX_STRING( sett().GetDefaultServer() );
  username = WX_STRING( sett().GetServerAccountNick( sett().GetDefaultServer() ) );
  
  // Create all UI elements.
  m_tabs =         new wxNotebook( this  , -1 );
  m_login_tab =    new wxPanel   ( m_tabs, -1 );
  m_register_tab = new wxPanel   ( m_tabs, -1 );
  
  m_server_lbl =   new wxStaticText( m_login_tab, -1, _T("Server") );
  m_server_combo = new wxComboBox  ( m_login_tab, -1, server );
  
  m_ser_acc_line = new wxStaticLine( m_login_tab );
  
  m_nick_lbl =    new wxStaticText( m_login_tab, -1, _T("Nickname") );
  m_nick_text =   new wxTextCtrl  ( m_login_tab, -1, username );
  m_pass_lbl =    new wxStaticText( m_login_tab, -1, _T("Password") );
  m_pass_text =   new wxTextCtrl  ( m_login_tab, -1, password, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD );
  m_rpass_check = new wxCheckBox  ( m_login_tab, -1, _("Remember\npassword") );
  
  m_acc_note_line = new wxStaticLine( m_login_tab );

  m_note_lbl = new wxStaticText( m_login_tab, -1, _T("Note: If you do not have an accound you\n can register one for free under the\n\"Register\" tab.") );

  m_ok_btn =     new wxButton( this, wxID_OK,     _T("Ok") );
  m_cancel_btn = new wxButton( this, wxID_CANCEL, _T("Cancel") );
  
  // Add tabs to tab control.
  m_tabs->AddPage( m_login_tab, _T("Login"), true );
  m_tabs->AddPage( m_register_tab, _T("Register"), false );

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
  m_rpass_sizer->Add( m_rpass_check, 1, wxEXPAND | wxALL, 4 );
  
  m_pass_sizer->Add( m_pass_lbl, 1, wxEXPAND | wxALL, 4 );
  m_pass_sizer->Add( m_pass_text, 1, wxEXPAND | wxALL, 4 );

  m_nick_sizer->Add( m_nick_lbl, 1, wxEXPAND | wxALL, 4 );
  m_nick_sizer->Add( m_nick_text, 1, wxEXPAND | wxALL, 4 );

  m_server_sizer->Add( m_server_lbl, 1, wxEXPAND | wxALL, 4 );
  m_server_sizer->Add( m_server_combo, 1, wxEXPAND | wxALL, 4 );
  
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
}


//! @brief Destructor
ConnectWindow::~ConnectWindow()
{
  
}

void ConnectWindow::OnOk(wxCommandEvent& event)
{
  Hide();
  sett().SetDefaultServer( STL_STRING(m_server_combo->GetValue()) );
  sett().SetServerAccountNick( STL_STRING(m_server_combo->GetValue()), STL_STRING(m_nick_text->GetValue()) );
  sys().Connect( STL_STRING(m_server_combo->GetValue()), STL_STRING(m_nick_text->GetValue()), STL_STRING(m_pass_text->GetValue()) );
}

void ConnectWindow::OnCancel(wxCommandEvent& event)
{
  Hide();
}

