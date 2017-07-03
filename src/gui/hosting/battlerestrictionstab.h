/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_BATTLEOPTIONSTAB_H
#define SPRINGLOBBY_HEADERGUARD_BATTLEOPTIONSTAB_H

#include <wx/scrolwin.h>

class Ui;
class IBattle;
class wxCommandEvent;
class wxBoxSizer;
class wxStaticText;
class wxSplitterWindow;
class wxButton;
class wxCheckBox;
class wxListBox;
class wxRadioBox;
class wxCheckListBox;

/** \brief manipulate "standard" engine options, unit restriction list
 * \todo DOCMEMORE */
class BattleRestrictionsTab : public wxScrolledWindow
{
public:
	BattleRestrictionsTab(wxWindow* parent, IBattle* battle);
	~BattleRestrictionsTab();

	void ReloadRestrictions();
	void SetBattle(IBattle* battle);

private:
	int GetAllowedUnitIndex(const wxString& name);
	int GetRestrictedUnitIndex(const wxString& name);

	bool IsRestricted(const wxString& name);
	void Restrict(const wxString& name, int count);
	void Allow(const wxString& name);
	void Restrict(int index, int count);
	void Allow(int index);

	IBattle* GetBattle();
	void OnRestrict(wxCommandEvent& event);
	void OnAllow(wxCommandEvent& event);
	void OnClearRestrictions(wxCommandEvent& event);

	IBattle* m_battle;

	wxStaticText* m_aloowed_lbl;
	wxListBox* m_allowed_list;
	wxButton* m_restrict_btn;
	wxButton* m_allow_btn;
	wxStaticText* m_restricted_lbl;
	wxListBox* m_restrict_list;
	wxButton* m_clear_btn;

	enum {
		BOPTS_END = wxID_HIGHEST,
		BOPTS_OPTS,
		BOPTS_SLIDE,

		BOPTS_RESTRICT,
		BOPTS_ALLOW,
		BOPTS_CLEARRES,

		BOPTS_LOADPRES,
		BOPTS_SAVEPRES,
		BOPTS_DELETEPRES,
		BOPTS_SETDEFAULTPRES,
		BOPTS_CHOSEPRES
	};

	DECLARE_EVENT_TABLE()
};


enum {
	BOPTS_RESTRICT = wxID_HIGHEST,
	BOPTS_ALLOW,
	BOPTS_CLEARRES
};


#endif // SPRINGLOBBY_HEADERGUARD_BATTLEOPTIONSTAB_H
