/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef AUTOBALANCEDIALOG_H
#define AUTOBALANCEDIALOG_H

//(*Headers(AutoBalanceDialog)
#include <wx/dialog.h>
#include "ibattle.h"
//*)

class wxButton;
class wxChoice;
class wxStaticLine;
class wxStaticText;


/** \brief lets host automatically assign players to teams, optionally taking clan/rank into account
 * dialog sets balancing options in Settings handler for the calling class to use after modal display ends
 * \todo use this in a non-modal way? */
class AutoBalanceDialog : public wxDialog
{
public:
	struct BalanceOptions {
		IBattle::BalanceType type;
		bool respectclans;
		bool strongclans;
		int groupingsize;
	};

	AutoBalanceDialog(wxWindow* parent, const BalanceOptions& defaultval, unsigned int maxnum = 16);
	virtual ~AutoBalanceDialog();

	BalanceOptions GetResult();

	//(*Declarations(AutoBalanceDialog)
	wxButton* m_ok_button;
	wxChoice* m_method_choice;
	wxStaticLine* m_separator1;
	wxStaticText* StaticText1;
	wxChoice* m_clans_choice;
	wxStaticText* m_method_label;
	wxButton* m_cancel_button;
	wxStaticText* StaticText2;
	wxChoice* m_group_choice;
	//*)

private:
	//(*Identifiers(AutoBalanceDialog)
	static const long ID_STATICTEXT1;
	static const long ID_METHOD;
	static const long ID_STATICTEXT2;
	static const long ID_STATICTEXT3;
	static const long ID_CHOICE1;
	static const long ID_CHOICE2;
	static const long ID_STATICLINE1;
	static const long ID_CANCEL;
	static const long ID_OK;
	//*)

private:
	//(*Handlers(AutoBalanceDialog)
	void OnCancel(wxCommandEvent& event);
	void OnOk(wxCommandEvent& event);
	//*)

	DECLARE_EVENT_TABLE()
};

#endif
