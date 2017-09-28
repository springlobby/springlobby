/* This file is part of the Springlobby (GPL v2 or later), see COPYING */
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
#include <wx/tooltip.h>
#include <wx/log.h>
#include <wx/textctrl.h>

#include "connectwindow.h"
#include "settings.h"
#include "utils/slconfig.h"
#include "ui.h"
#include "images/connect.xpm"
#include "gui/controls.h"
#include "utils/tasutil.h"
#include "utils/conversion.h"
#include "servermanager.h"

// Define events.
BEGIN_EVENT_TABLE(ConnectWindow, wxDialog)

EVT_BUTTON(wxID_OK, ConnectWindow::OnOk)
EVT_BUTTON(wxID_CANCEL, ConnectWindow::OnCancel)
EVT_COMBOBOX(CON_SERV_SEL, ConnectWindow::OnServerChange)

END_EVENT_TABLE()

//! @brief Constructor
//!
//! @param parent Parent window
ConnectWindow::ConnectWindow(wxWindow* parent, Ui& ui)
    : wxDialog(parent, -1, _("Connect to lobby server"), wxDefaultPosition, wxSize(300, 300),
	       wxDEFAULT_DIALOG_STYLE | wxCLIP_CHILDREN)
    , m_ui(ui)
{
	wxString server;
	wxString username;
	wxString password;
	bool savepass;
	bool autoconnect;
	bool tls;

	SetIcon(wxIcon(connect_xpm));

	server = sett().GetDefaultServer();
	username = sett().GetServerAccountNick(server);
	password = sett().GetServerAccountPass(server);
	savepass = sett().GetServerAccountSavePass(server);
	autoconnect = cfg().ReadBool(_T( "/Server/Autoconnect" ));
	tls = sett().IsServerTLS(STD_STRING(server));

	// Create all UI elements.
	m_tabs = new wxNotebook(this, -1);
	m_login_tab = new wxPanel(m_tabs, -1);
	m_register_tab = new wxPanel(m_tabs, -1);

	m_server_lbl = new wxStaticText(m_login_tab, -1, _("Server"));
	m_server_combo = new wxComboBox(m_login_tab, CON_SERV_SEL, server);
	m_server_combo->SetToolTip(_("Server to connect to. You can connect to any server you like by typing in hostaddress:port format."));

	m_ser_acc_line = new wxStaticLine(m_login_tab);

	m_nick_lbl = new wxStaticText(m_login_tab, -1, _("Nickname"));
	m_nick_text = new wxTextCtrl(m_login_tab, -1, username);
	m_pass_lbl = new wxStaticText(m_login_tab, -1, _("Password"));
	m_pass_text = new wxTextCtrl(m_login_tab, -1, password, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);
	m_rpass_check = new wxCheckBox(m_login_tab, -1, _("Remember password"));
	m_autoconnect_check = new wxCheckBox(m_login_tab, -1, _("Autoconnect next time"));
	m_autoconnect_check->SetToolTip(_("remember connection details and automatically connect to server on next lobby startup"));

	m_tls_check = new wxCheckBox(m_login_tab, -1, _("TLS"));
	m_tls_check->SetToolTip(_("Use encrypted connection to connect to the Lobby server"));

	m_rpass_check->SetValue(savepass);
	m_autoconnect_check->SetValue(autoconnect);
	m_tls_check->SetValue(tls);

	m_acc_note_line = new wxStaticLine(m_login_tab);

	m_note_lbl = new wxStaticText(m_login_tab, -1, _("Note: If you do not have an account, you can register one for free on the Register tab."));
	m_note_lbl->Wrap(400);

	m_ok_btn = new wxButton(this, wxID_OK, _("Ok"));
	m_cancel_btn = new wxButton(this, wxID_CANCEL, _("Cancel"));

	// Add tabs to tab control.
	m_tabs->AddPage(m_login_tab, _("Login"), true);
	m_tabs->AddPage(m_register_tab, _("Register"), false);

	// Create sizers.
	m_main_sizer = new wxBoxSizer(wxVERTICAL);
	m_login_main_sizer = new wxBoxSizer(wxVERTICAL);
	m_server_sizer = new wxBoxSizer(wxHORIZONTAL);
	m_nick_sizer = new wxBoxSizer(wxHORIZONTAL);
	m_pass_sizer = new wxBoxSizer(wxHORIZONTAL);
	m_rpass_sizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* m_check_sizer = new wxBoxSizer(wxHORIZONTAL);
	m_buttons_sizer = new wxStdDialogButtonSizer();

	// Add UI elements to sizers.
	m_buttons_sizer->Add(m_cancel_btn);
	m_buttons_sizer->AddStretchSpacer();
	m_buttons_sizer->Add(m_ok_btn);


	m_rpass_sizer->Add(m_rpass_check, 2, wxEXPAND | wxALL | wxALIGN_RIGHT, 4);
	m_rpass_sizer->Add(m_autoconnect_check, 2, wxEXPAND | wxALL | wxALIGN_RIGHT, 4);
	m_rpass_sizer->Add(m_tls_check, 2, wxEXPAND | wxALL | wxALIGN_RIGHT, 4);

	m_pass_sizer->Add(m_pass_lbl, 1, wxALL | wxALIGN_CENTER_VERTICAL, 4);
	m_pass_sizer->Add(m_pass_text, 2, wxEXPAND | wxALL, 4);

	//FIXME was lazy, absoulte positioning isn't that nice //probably causing http://springlobby.info/issues/1094
	int pos1 = (m_pass_lbl->GetSize()).GetWidth() + 40;
	m_check_sizer->Add(pos1, 0, 0);
	m_check_sizer->Add(m_rpass_sizer, 0, wxEXPAND | wxALIGN_RIGHT);

	m_nick_sizer->Add(m_nick_lbl, 1, wxALL | wxALIGN_CENTER_VERTICAL, 4);
	m_nick_sizer->Add(m_nick_text, 2, wxEXPAND | wxALL, 4);

	m_server_sizer->Add(m_server_lbl, 1, wxALL | wxALIGN_CENTER_VERTICAL, 4);
	m_server_sizer->Add(m_server_combo, 2, wxEXPAND | wxALL, 4);

	m_login_main_sizer->Add(m_server_sizer, 0, wxEXPAND | wxALL, 0);
	m_login_main_sizer->Add(m_ser_acc_line, 0, wxEXPAND | wxALL, 4);
	m_login_main_sizer->Add(m_nick_sizer, 0, wxEXPAND);
	m_login_main_sizer->Add(m_pass_sizer, 0, wxEXPAND);
	m_login_main_sizer->Add(m_check_sizer, 0, wxEXPAND);
	m_login_main_sizer->Add(m_acc_note_line, 0, wxEXPAND | wxALL, 4);
	m_login_main_sizer->Add(m_note_lbl, 0, wxEXPAND | wxALL, 4);
	m_login_main_sizer->AddStretchSpacer();

	m_login_tab->SetSizer(m_login_main_sizer);

	m_main_sizer->Add(m_tabs, 1, wxEXPAND);
	m_main_sizer->Add(m_buttons_sizer, 0, wxEXPAND);


	// Register page
	wxBoxSizer* m_register_sizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* m_regnick_sizer = new wxBoxSizer(wxHORIZONTAL);

	m_regnick_lbl = new wxStaticText(m_register_tab, wxID_ANY, _("Nickname"), wxDefaultPosition, wxDefaultSize, 0);
	m_regnick_sizer->Add(m_regnick_lbl, 1, wxALL | wxALIGN_CENTER_VERTICAL, 4);

	m_regnick_text = new wxTextCtrl(m_register_tab, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_regnick_sizer->Add(m_regnick_text, 1, wxALL, 4);

	m_register_sizer->Add(m_regnick_sizer, 0, wxEXPAND, 4);

	m_regpass_sep = new wxStaticLine(m_register_tab, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
	m_register_sizer->Add(m_regpass_sep, 0, wxALL | wxEXPAND, 4);

	wxBoxSizer* m_regpass1_sizer = new wxBoxSizer(wxHORIZONTAL);

	m_regpass1_lbl = new wxStaticText(m_register_tab, wxID_ANY, _("Password"), wxDefaultPosition, wxDefaultSize, 0);
	m_regpass1_sizer->Add(m_regpass1_lbl, 1, wxALL | wxALIGN_CENTER_VERTICAL, 4);

	m_regpass1_text = new wxTextCtrl(m_register_tab, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);
	m_regpass1_sizer->Add(m_regpass1_text, 1, wxALL, 4);

	m_register_sizer->Add(m_regpass1_sizer, 0, wxEXPAND, 4);

	wxBoxSizer* m_regpass1_sizer1 = new wxBoxSizer(wxHORIZONTAL);

	m_regpass2_lbl = new wxStaticText(m_register_tab, wxID_ANY, _("Retype password"), wxDefaultPosition, wxDefaultSize, 0);
	m_regpass1_sizer1->Add(m_regpass2_lbl, 1, wxALL | wxALIGN_CENTER_VERTICAL, 4);

	m_regpass2_text = new wxTextCtrl(m_register_tab, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);
	m_regpass1_sizer1->Add(m_regpass2_text, 1, wxALL, 4);


	m_register_sizer->Add(m_regpass1_sizer1, 0, wxEXPAND, 4);

	m_reginfo_text = new wxStaticText(m_register_tab, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);

	m_register_sizer->Add(m_reginfo_text, 1, wxALL, 4);

	m_register_tab->SetSizer(m_register_sizer);
	m_register_tab->Layout();
	m_register_sizer->Fit(m_register_tab);


	// Set sizer.
	SetSizer(m_main_sizer);

	if (!username.empty()) {
		m_ok_btn->SetFocus();
	} else { // no nick set, make register tab default
		m_tabs->SetSelection(1);
		m_regnick_text->SetFocus();
	}

	m_ok_btn->SetDefault();

	Layout();
	m_main_sizer->SetSizeHints(this);
#ifdef __WXMSW__
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
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
	m_server_combo->Append(sett().GetServers());
	m_server_combo->SetValue(sett().GetDefaultServer());
}

void ConnectWindow::OnServerChange(wxCommandEvent&)
{
	wxString HostAddress = m_server_combo->GetValue();
	m_nick_text->SetValue(sett().GetServerAccountNick(HostAddress));
	m_pass_text->SetValue(sett().GetServerAccountPass(HostAddress));
}

void ConnectWindow::CleanHide()
{
	m_reginfo_text->SetLabel(wxEmptyString);
	m_note_lbl->SetLabel(wxEmptyString);
	Hide();
}

void ConnectWindow::OnOk(wxCommandEvent&)
{
	wxString HostAddress = m_server_combo->GetValue();
	// We assume that the server is given as : "host:port" so we split based on ":"
	wxArrayString serverString = wxStringTokenize(HostAddress, _T(":"));
	if (serverString.GetCount() > 2) {
		OnLoginDenied(_T("invalid host/port."));
		return;
	}
	if (serverString.GetCount() == 2) {
		long port;
		if (!serverString[1].ToLong(&port)) {
			OnLoginDenied(_T("Invalid port."));
			return;
		}
		if (port < 1 || port > 65535) {
			OnLoginDenied(_T("Port number out of range.\n\nIt must be an integer between 1 and 65535"));
			return;
		}
		sett().SetServer(HostAddress, serverString[0], port, "");
	}
	sett().SetDefaultServer(HostAddress);
	cfg().Write(_T( "/Server/Autoconnect" ), m_autoconnect_check->IsChecked());
	sett().SetServerTLS(STD_STRING(HostAddress), m_tls_check->IsChecked());

	//if autoconnect enabled force saving of pw, actual saving is done in Ui::DoConnect

	sett().SaveSettings();
	ReloadServerList();

	if (m_tabs->GetSelection() <= 0) { //normal login
		sett().SetServerAccountNick(HostAddress, m_nick_text->GetValue());
		sett().SetServerAccountSavePass(HostAddress, m_rpass_check->GetValue());
		if (m_rpass_check->IsChecked()) {
			sett().SetServerAccountPass(HostAddress, m_pass_text->GetValue());
		}
		CleanHide();
		ServerManager::Instance()->DoConnectToServer(STD_STRING(HostAddress), STD_STRING(m_nick_text->GetValue()), STD_STRING(m_pass_text->GetValue()));
		return;
	}

	sett().SetServerAccountNick(HostAddress, m_regnick_text->GetValue());
	if (m_rpass_check->IsChecked()) {
		sett().SetServerAccountPass(HostAddress, m_regpass1_text->GetValue());
	}

	// register new nick
	if (!IsValidNickname(STD_STRING(m_regnick_text->GetValue()))) {
		OnRegistrationDenied(_("The entered nickname contains invalid characters like )? &%.\n Please try again"));
		return;
	}
	if (m_regpass2_text->GetValue() != m_regpass1_text->GetValue() || m_regpass1_text->GetValue().IsEmpty()) {
		OnRegistrationDenied(_("Registration failed, the reason was:\nPassword / confirmation mismatch (or empty passwort)"));
		return;
	}
	CleanHide();
	ServerManager::Instance()->RegisterNewUser(STD_STRING(HostAddress), STD_STRING(m_regnick_text->GetValue()), STD_STRING(m_regpass1_text->GetValue()));
}

void ConnectWindow::OnCancel(wxCommandEvent&)
{
	Hide();
}

void ConnectWindow::OnRegistrationAccepted(const wxString& user, const wxString& pass)
{
	//	Show();
	m_tabs->SetSelection(0);
	m_nick_text->SetValue(user);
	m_pass_text->SetValue(pass);
}

void ConnectWindow::OnRegistrationDenied(const wxString& reason)
{
	m_reginfo_text->SetLabel(reason);
	m_reginfo_text->SetForegroundColour(wxColour(255, 0, 0));
	m_reginfo_text->Wrap(400);
	m_tabs->SetSelection(1);
	Show();
}

void ConnectWindow::OnLoginDenied(const wxString& reason)
{
	m_note_lbl->SetLabel(reason);
	m_note_lbl->SetForegroundColour(wxColour(255, 0, 0));
	m_note_lbl->Wrap(400);
	m_tabs->SetSelection(0);
	Show();
}
