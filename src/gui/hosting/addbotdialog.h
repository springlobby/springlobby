/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_ADDBOTDIALOG_H
#define SPRINGLOBBY_HEADERGUARD_ADDBOTDIALOG_H

#include <wx/dialog.h>
#include <map>
#include <vector>

#include "gui/windowattributespickle.h"

class wxTextCtrl;
class wxStaticText;
class wxChoice;
class wxStaticLine;
class wxButton;
class wxCommandEvent;
class IBattle;
class wxListCtrl;
class wxBoxSizer;
class wxListEvent;

/** \brief used in SP/MP BattletAB to present choice of AIs to add
 * \todo DOCMEMORE */
class AddBotDialog : public wxDialog, public WindowAttributesPickle
{
public:
	AddBotDialog(wxWindow* parent, IBattle& battle, bool singleplayer = false);
	~AddBotDialog();

	wxString GetNick();
	wxString GetAIShortName();
	wxString GetAiRawName();
	wxString GetAIVersion();
	int GetSelectedAIType();

	void ReloadAIList();

private:
	void ShowAIInfo();

	void OnClose(wxCommandEvent& event);
	void OnAddBot(wxCommandEvent& event);
	void OnSelectBot(wxCommandEvent& event);

	void OnOptionActivate(wxListEvent& event);
	void UpdateOption(const wxString& Tag);
	long AddMMOptionsToList(long pos, int optFlag);
	void ShowAIOptions();


	wxString Get(const std::string& section);
	AddBotDialog(const AddBotDialog&);

	wxStaticText* m_nick_lbl;
	wxTextCtrl* m_nick;
	wxStaticText* m_ai_lbl;
	wxChoice* m_ai;
	wxStaticLine* m_buttons_sep;
	wxButton* m_cancel_btn;
	wxButton* m_add_btn;
	wxListCtrl* m_ai_infos_lst;
	wxListCtrl* m_opts_list;

	typedef std::map<wxString, long> OptionListMap;
	OptionListMap m_opt_list_map;

	wxBoxSizer* m_main_sizer;
	wxBoxSizer* m_info_sizer;


	IBattle& m_battle;

	wxArrayString m_ais;

	bool m_sp;

	// Maps display (wxChoice m_ai) indices to unitsync indices (wxArrayString m_ais)
	std::vector<int> m_valid_ai_index_map;
	wxString RefineAIName(const wxString& name);

	enum {
		ADDBOT_ADD = wxID_HIGHEST,
		ADDBOT_CANCEL,
		ADDBOT_AI,
		ADDBOT_OPTIONLIST
	};

	DECLARE_EVENT_TABLE()
};


#endif // SPRINGLOBBY_HEADERGUARD_ADDBOTDIALOG_H
