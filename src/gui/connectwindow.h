/* This file is part of the Springlobby (GPL v2 or later), see COPYING */
#ifndef SPRINGLOBBY_HEADERGUARD_CONNECTWINDOW_H
#define SPRINGLOBBY_HEADERGUARD_CONNECTWINDOW_H

#include <wx/dialog.h>

class Ui;
class wxBoxSizer;
class wxButton;
class wxCheckBox;
class wxComboBox;
class wxNotebook;
class wxPanel;
class wxStaticLine;
class wxStaticText;
class wxStdDialogButtonSizer;
class wxTextCtrl;

//! @brief wxFrame with a connection dialog used to specify username, password, and server. It can also register a new acount.
class ConnectWindow : public wxDialog
{
public:
	ConnectWindow(wxWindow* parent, Ui& ui);
	~ConnectWindow();

	void OnServerChange(wxCommandEvent& event);
	void ReloadServerList();

	void OnLoginDenied(const wxString& reason);
	void OnRegistrationAccepted(const wxString& user, const wxString& pass);
	void OnRegistrationDenied(const wxString& reason);
private:
	void OnCancel(wxCommandEvent& event);
	void OnOk(wxCommandEvent& event);
	void OnQuit(wxCommandEvent& data);

	void CleanHide();

	wxNotebook* m_tabs;      //!< Notebook containing the login and register tabs
	wxPanel* m_login_tab;    //!< The login tab
	wxPanel* m_register_tab; //!< The register tab

	wxStaticText* m_server_lbl; //!< Label saying "Server"
	wxComboBox* m_server_combo; //!< Combobox where user can select server

	wxStaticLine* m_ser_acc_line; //!< Line that separates the server setting and the acount settings

	wxStaticText* m_nick_lbl;	//!< Label saying "Nickname"
	wxTextCtrl* m_nick_text;	 //!< Textbox where user can input nickname
	wxStaticText* m_pass_lbl;	//!< Label saying "Password"
	wxTextCtrl* m_pass_text;	 //!< Textbox where user can input password
	wxCheckBox* m_rpass_check;       //!< Checkbox where user can chose if he wants his password to be saved.
	wxCheckBox* m_autoconnect_check; //!< Checkbox where user can chose if he wants to autoconnect.

	wxStaticLine* m_acc_note_line; //!< Line that separates the account settings from the note

	wxStaticText* m_note_lbl; //!< Label saying something about register

	wxButton* m_ok_btn;     //!< The ok button
	wxButton* m_cancel_btn; //!< The cancel button

	wxBoxSizer* m_main_sizer;		 //!< The main sizer connecting it all together
	wxBoxSizer* m_login_main_sizer;		 //!< Login tab main sizer
	wxBoxSizer* m_server_sizer;		 //!< Sizer connecting server label and combo
	wxBoxSizer* m_nick_sizer;		 //!< Sizer connecting nick label and input
	wxBoxSizer* m_pass_sizer;		 //!< Sizer connecting password label and input
	wxBoxSizer* m_rpass_sizer;		 //!< Sizer spacing the remember password setting
	wxStdDialogButtonSizer* m_buttons_sizer; //!< Sizer connecting the ok and cancel buttons

	wxStaticText* m_regnick_lbl;
	wxTextCtrl* m_regnick_text;
	wxStaticLine* m_regpass_sep;
	wxStaticText* m_regpass1_lbl;
	wxTextCtrl* m_regpass1_text;
	wxStaticText* m_regpass2_lbl;
	wxTextCtrl* m_regpass2_text;
	wxStaticText* m_reginfo_text;

	Ui& m_ui;

	enum {
		CON_SERV_SEL = wxID_HIGHEST
	};

	DECLARE_EVENT_TABLE()
};

#endif // SPRINGLOBBY_HEADERGUARD_CONNECTWINDOW_H
