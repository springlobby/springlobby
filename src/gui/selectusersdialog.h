/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_SELECTUSERSDIALOG_H
#define SPRINGLOBBY_HEADERGUARD_SELECTUSERSDIALOG_H


#include <wx/dialog.h>
class wxPanel;
class wxStaticText;
class wxTextCtrl;
class wxListCtrl;
class wxStaticLine;
class wxStdDialogButtonSizer;
class wxButton;
class wxListEvent;

class SelectUsersDialog : public wxDialog
{
private:
	DECLARE_EVENT_TABLE()

	enum {
		FILTER_TEXT = 1000,
		NICK_LIST,
		NAME_TEXT
	};

	wxBoxSizer* m_main_sizer;
	wxPanel* m_users_panel;
	wxStaticText* m_filter_names_staticText;
	wxTextCtrl* m_name_filter_text;
	wxListCtrl* m_user_list;
	wxStaticText* m_name_staticText;
	wxTextCtrl* m_selection_text;
	wxStaticLine* m_buttons_hr;
	wxStdDialogButtonSizer* m_dialog_buttons;
	wxButton* m_dialog_buttonsOK;
	wxButton* m_dialog_buttonsCancel;

	void OnNameFilterChange(wxCommandEvent& event);
	void OnNameActivated(wxListEvent& event);
	void OnNameDeselected(wxListEvent& event);
	void OnNameSelected(wxListEvent& event);
	void OnCancel(wxCommandEvent& event);
	void OnOk(wxCommandEvent& event);

	wxSortedArrayString m_selection;
	wxSortedArrayString m_filtered_users;

	void Initialize();
	void PopulateUsersList();
	void ClearList();
	void UpdateUsersList();
	void UpdateSelection();
	wxArrayString GetSelectionFromText();
	wxString BuildSelectionText(const wxSortedArrayString& sel);

	long AddUserToList(const wxString& nick, const wxString& flag);
	long AddUserToList(const wxString& nick, const int& flag);
	void RemoveUserFromList(long item);

	static int wxCALLBACK CompareName(long item1, long item2, long sortData);

	void Sort();

public:
	SelectUsersDialog(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Select Users"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(358, 518), long style = wxDEFAULT_DIALOG_STYLE);
	~SelectUsersDialog();

	static wxArrayString GetUsers(wxWindow* parent = 0);

	int ShowModal();
	wxArrayString GetSelection();
};

#endif // SPRINGLOBBY_HEADERGUARD_SELECTUSERSDIALOG_H
