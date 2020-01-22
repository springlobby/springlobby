/* This file is part of the Springlobby (GPL v2 or later), see COPYING */
#include "connectwindow.h"

#include <wx/log.h>
#include <wx/tokenzr.h>

#include "gui/controls.h"
#include "images/connect.xpm"
#include "servermanager.h"
#include "settings.h"
#include "ui.h"
#include "utils/conversion.h"
#include "utils/slconfig.h"
#include "utils/tasutil.h"

//! @brief Constructor
//!
//! @param parent Parent window
ConnectWindow::ConnectWindow(wxWindow* parent)
    : ConnectWindowBase(parent)
{
	SetIcon(wxIcon(connect_xpm));
	wxString server = sett().GetDefaultServer();
	wxString username = sett().GetServerAccountNick(server);
	wxString password = sett().GetServerAccountPass(server);
	bool savepass = sett().GetServerAccountSavePass(server);
	bool autoconnect = cfg().ReadBool(_T( "/Server/Autoconnect" ));

	if (sett().GetUserLevel() < Settings::UserLevel::Professional) {
		m_remember_password_check->Hide();
		m_autoconnect_check->Hide();
	}

	ReloadServerList();
	m_nickname_text->SetValue(username);
	m_password1_hidden_text->SetValue(password);
	m_remember_password_check->SetValue(savepass);
	m_autoconnect_check->SetValue(autoconnect);

	if (!username.empty()) {
		EnterLoginMode();
	} else { // no nick set, make register tab default
		EnterRegistrationMode();
	}

	Fit();
#ifdef __WXMSW__
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
#endif
}

void ConnectWindow::EnterRegistrationMode()
{
	in_login_mode = false;

	Freeze();
	m_loginreg_button->SetLabel(_("Return to login..."));
	m_email_label->Show();
	m_email_text->Show();
	m_password2_label->Show();
	m_password2_hidden_text->Show();
	m_nickname_text->SetFocus();
	m_ok_button->SetLabel(_("Register"));
	m_note_text->SetValue("A verification code will immidiately be sent to your email address.");
	m_note_text->Show();

	Layout(); Fit(); Thaw();
}

void ConnectWindow::EnterLoginMode()
{
	in_login_mode = true;

	Freeze();
	m_loginreg_button->SetLabel(_("Create a new account..."));
	m_email_label->Hide();
	m_email_text->Hide();
	m_password2_label->Hide();
	m_password2_hidden_text->Hide();
	m_ok_button->SetFocus();
	m_ok_button->SetLabel(_("Login"));
	m_note_text->Hide();

	Fit(); Layout(); Thaw();
}

void ConnectWindow::OnChangeMode(wxCommandEvent&)
{
	if (in_login_mode)
		EnterRegistrationMode();
	else
		EnterLoginMode();
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
	m_nickname_text->SetValue(sett().GetServerAccountNick(HostAddress));
	m_password1_hidden_text->SetValue(sett().GetServerAccountPass(HostAddress));
	m_password1_plain_text->SetValue(sett().GetServerAccountPass(HostAddress));
	m_password2_hidden_text->SetValue(sett().GetServerAccountPass(HostAddress));
	m_password2_plain_text->SetValue(sett().GetServerAccountPass(HostAddress));
}

void ConnectWindow::CleanHide()
{
	m_note_text->SetValue(wxEmptyString);
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

	//if autoconnect enabled force saving of pw, actual saving is done in Ui::DoConnect

	sett().SaveSettings();
	ReloadServerList();

	if (in_login_mode) {
		sett().SetServerAccountNick(HostAddress, m_nickname_text->GetValue());
		sett().SetServerAccountSavePass(HostAddress, m_remember_password_check->GetValue());
		if (m_remember_password_check->IsChecked()) {
			sett().SetServerAccountPass(HostAddress, m_password1_hidden_text->GetValue());
		}
		CleanHide();
		ServerManager::Instance()->DoConnectToServer(STD_STRING(HostAddress),
		                                             STD_STRING(m_nickname_text->GetValue()),
		                                             STD_STRING(m_password1_hidden_text->GetValue()));
		return;
	}

	sett().SetServerAccountNick(HostAddress, m_nickname_text->GetValue());
	// Save password if the user choses so
	if (m_remember_password_check->IsChecked()) {
		sett().SetServerAccountPass(HostAddress, m_password1_hidden_text->GetValue());
	}

	// register new nick
	if (!IsValidNickname(STD_STRING(m_nickname_text->GetValue()))) {
		OnRegistrationDenied(_("The entered nickname contains invalid characters like )? &%.\n Please try again"));
		return;
	}
	if (m_password2_hidden_text->GetValue() != m_password1_hidden_text->GetValue() || m_password1_hidden_text->GetValue().IsEmpty()) {
		OnRegistrationDenied(_("Registration failed, the reason was:\nPassword / confirmation mismatch (or empty passwort)"));
		return;
	}
	CleanHide();
	ServerManager::Instance()->RegisterNewUser(STD_STRING(HostAddress),
	                                           STD_STRING(m_email_text->GetValue()),
	                                           STD_STRING(m_nickname_text->GetValue()),
	                                           STD_STRING(m_password1_hidden_text->GetValue()));
}

void ConnectWindow::OnCancel(wxCommandEvent&)
{
	Hide();
}

void ConnectWindow::OnRegistrationAccepted(const wxString& user, const wxString& pass)
{
	//	Show();
	EnterLoginMode();
	m_nickname_text->SetValue(user);
	m_password1_hidden_text->SetValue(pass);
}

void ConnectWindow::OnRegistrationDenied(const wxString& reason)
{
	EnterRegistrationMode();
	m_note_text->SetValue(reason);
	m_note_text->Show();
	Show();
}

void ConnectWindow::OnLoginDenied(const wxString& reason)
{
	EnterLoginMode();
	m_note_text->SetValue(reason);
	m_note_text->Show();
	Show();
}
