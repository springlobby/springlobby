/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef LOBBYOPTIONSTAB_H
#define LOBBYOPTIONSTAB_H

#include <wx/scrolwin.h>

class wxCheckBox;
class wxBoxSizer;
class wxStaticText;
class wxRadioButton;
class wxTextCtrl;
class wxButton;
class wxStaticBoxSizer;
class wxChoice;
class wxSpinCtrl;

class LobbyOptionsTab : public wxScrolledWindow
{
public:
	LobbyOptionsTab(wxWindow* parent);
	virtual ~LobbyOptionsTab();

	void OnApply(wxCommandEvent& event);
	void OnRestore(wxCommandEvent& event);

	void HandleWebloc(bool defloc);

private:
	void OnBrowseWeb(wxCommandEvent& event);
	void OnBrowseEditor(wxCommandEvent& event);
	void OnDefaultWeb(wxCommandEvent& event);
	void OnTestNotification(wxCommandEvent& event);

	wxCheckBox* m_autojoin;
	wxCheckBox* m_updater;
	wxCheckBox* m_show_tooltips;
	wxCheckBox* m_use_tabicons;
	wxCheckBox* m_x_on_all_tabs;
	wxCheckBox* m_use_notif_popups;

	wxStaticText* m_autoconnect_label;
	wxStaticText* m_updater_label;
	wxStaticText* m_show_tooltips_label;
	wxStaticText* m_web_loc_text;
	wxStaticText* m_editor_loc_text;
	wxTextCtrl* m_web_edit;
	wxTextCtrl* m_editor_edit;

	wxStaticText* m_complete_method_label;
	wxStaticText* m_notif_popup_pos_label;
	wxStaticText* m_notif_popup_time_label;
	wxRadioButton* m_complete_method_old;
	wxRadioButton* m_complete_method_new;
	wxButton* m_web_browse_btn;
	wxButton* m_editor_browse_btn;
	wxButton* m_test_notification;
	//        wxButton* m_select_locale;
	wxRadioButton* m_web_def_radio;
	wxRadioButton* m_web_spec_radio;


	wxStaticBoxSizer* m_web_box_sizer;
	wxStaticBoxSizer* m_editor_box_sizer;
	wxBoxSizer* m_web_loc_sizer;
	wxBoxSizer* m_editor_loc_sizer;
	wxBoxSizer* m_notif_popup_pos_sizer;
	wxBoxSizer* m_notif_popup_time_sizer;

	wxChoice* m_start_tab;
	wxChoice* m_notif_popup_pos;
	wxSpinCtrl* m_notif_popup_time;


	enum {
		SPRING_WEBBROWSE = wxID_HIGHEST,
		SPRING_DEFWEB,
		ID_BUT_EDITOR,
		TEST_NOTIFICATION
	};


	DECLARE_EVENT_TABLE()
};

#endif // LOBBYOPTIONSTAB_H
