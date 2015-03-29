/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

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
	SpringOptionsTab(wxWindow* parent);
	~SpringOptionsTab();

	void OnApply(wxCommandEvent& event);
	void OnRestore(wxCommandEvent& event);

private:
	void DoRestore();
	void EnableSpringBox(bool enabled);
	void EnableUnitsyncBox(bool enabled);

	void OnBrowseExec(wxCommandEvent& event);
	void OnBrowseSync(wxCommandEvent& event);
	void OnAutoConf(wxCommandEvent& event);
	void OnFindExec(wxCommandEvent& event);
	void OnFindSync(wxCommandEvent& event);
	void OnAddBundle(wxCommandEvent& event);
	void OnRemoveBundle(wxCommandEvent& event);

	void OnDataDir(wxCommandEvent& event);

	void OnDontSearch(wxCommandEvent& event);
	void OnForceBundle(wxCommandEvent& event);

	void SetupUserFolders();
	void OnGroupListSelectionChange(wxCommandEvent& event);
	void ReloadSpringList();
	void SwitchUnitsync(const std::string& newIndex, const std::string& oldIndex);

	wxStaticText* m_exec_loc_text;
	wxStaticText* m_sync_loc_text;

	wxButton* m_exec_browse_btn;
	wxButton* m_exec_find_btn;
	wxButton* m_sync_browse_btn;
	wxButton* m_sync_find_btn;
	wxButton* m_restore_btn;

	wxButton* m_auto_btn;

	//	wxRadioButton* m_exec_def_radio;
	//	wxRadioButton* m_exec_spec_radio;
	//	wxRadioButton* m_sync_def_radio;
	//	wxRadioButton* m_sync_spec_radio;

	wxTextCtrl* m_exec_edit;
	wxTextCtrl* m_sync_edit;

	wxStaticBox* m_exec_box;
	wxStaticBox* m_sync_box;
	wxStaticBoxSizer* m_exec_box_sizer;
	wxStaticBoxSizer* m_sync_box_sizer;

	wxListBox* m_spring_list;
	wxButton* m_remove_spring_button;
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
		SPRING_ADD,
		SPRING_RESTORE
	};

	DECLARE_EVENT_TABLE()
};

#endif // SPRINGLOBBY_HEADERGUARD_SPRINGOPTIONSTAB_H
