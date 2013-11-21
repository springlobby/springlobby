#ifndef SPRINGLOBBY_HEADERGUARD_SPRINGOPTIONSTAB_H
#define SPRINGLOBBY_HEADERGUARD_SPRINGOPTIONSTAB_H

#include <wx/scrolwin.h>

class wxStaticBoxSizer;
class wxStaticBox;
class wxStaticText;
class wxRadioButton;
class wxButton;
class wxTextCtrl;
class wxBoxSizer;
class Ui;
class wxCheckBox;
class wxListBox;

class SpringOptionsTab : public wxScrolledWindow
{
public:
	SpringOptionsTab( wxWindow* parent );
	~SpringOptionsTab();

	void OnApply( wxCommandEvent& event );
	void OnRestore( wxCommandEvent& event );

private:
	void DoRestore();
	void EnableSpringBox(bool enabled);
	void EnableUnitsyncBox(bool enabled);
	void EnableBundleBox(bool enabled);

	void OnBrowseExec( wxCommandEvent& event );
	void OnBrowseSync( wxCommandEvent& event );
	void OnBrowseBundle( wxCommandEvent& event );
	void OnAutoConf( wxCommandEvent& event );
	void OnFindExec( wxCommandEvent& event );
	void OnFindSync( wxCommandEvent& event );
	void OnFindBundle( wxCommandEvent& event );
	void OnAddBundle(wxCommandEvent& event);
	void OnRemoveBundle(wxCommandEvent& event);

	void OnDataDir( wxCommandEvent& event );

	void OnDontSearch( wxCommandEvent& event );
	void OnForceBundle( wxCommandEvent& event );

	void SetupUserFolders();
	void OnGroupListSelectionChange( wxCommandEvent& event );
	void ReloadSpringList();

	wxStaticText* m_exec_loc_text;
	wxStaticText* m_sync_loc_text;
	wxStaticText* m_bundle_loc_text;

	wxButton* m_exec_browse_btn;
	wxButton* m_exec_find_btn;
	wxButton* m_sync_browse_btn;
	wxButton* m_sync_find_btn;
	wxButton* m_bundle_browse_btn;
	wxButton* m_bundle_find_btn;
	wxButton* m_datadir_btn;

	wxButton* m_auto_btn;

	wxRadioButton* m_exec_def_radio;
	wxRadioButton* m_exec_spec_radio;
	wxRadioButton* m_sync_def_radio;
	wxRadioButton* m_sync_spec_radio;
	wxRadioButton* m_bundle_def_radio;
	wxRadioButton* m_bundle_spec_radio;

	wxTextCtrl* m_exec_edit;
	wxTextCtrl* m_sync_edit;
	wxTextCtrl* m_bundle_edit;

	wxStaticBox* m_exec_box;
	wxStaticBox* m_sync_box;
	wxStaticBox* m_bundle_box;
	wxStaticBox* m_web_box;
	wxStaticBoxSizer* m_exec_box_sizer;
	wxStaticBoxSizer* m_sync_box_sizer;
	wxStaticBoxSizer* m_bundle_box_sizer;

	wxCheckBox* m_dontsearch_chkbox;
	wxCheckBox* m_forcebundle_chkbox;
	wxCheckBox* m_oldlaunch_chkbox;

	wxListBox* m_spring_list;
	wxButton* m_remove_spring_button;
	wxButton* m_rename_spring_button;
	wxButton* m_add_spring_button;

	enum {
		SPRING_EXECBROWSE = wxID_HIGHEST,
		SPRING_SYNCBROWSE,
		SPRING_BUNDLEBROWSE,
		SPRING_WEBBROWSE,
		SPRING_DEFEXE,
		SPRING_DEFUSYNC,
		SPRING_DEFBUNDLE,
		SPRING_DEFWEB,
		SPRING_AUTOCONF,
		SPRING_EXECFIND,
		SPRING_SYNCFIND,
		SPRING_BUNDLEFIND,
		SPRING_DATADIR,
		SPRING_DONTSEARCH,
		SPRING_FORCEBUNDLE,
		SPRING_LIST,
		SPRING_REMOVE,
		SPRING_RENAME,
		SPRING_ADD
	};

	DECLARE_EVENT_TABLE()
};

#endif // SPRINGLOBBY_HEADERGUARD_SPRINGOPTIONSTAB_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2011

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    SpringLobby is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

