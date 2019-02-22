/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_AGREEMENTDIALOG_H
#define SPRINGLOBBY_HEADERGUARD_AGREEMENTDIALOG_H

#include <wx/dialog.h>
class wxTextCtrl;
class wxStaticText;
class wxStaticLine;
class wxButton;

/** \brief dialog showing agreement on first start/ register that user needs to accept
 * \todo DOCMEMORE */
class AgreementDialog : public wxDialog
{
public:
	AgreementDialog(wxWindow* parent, const wxString& agreement, std::string* verif_code);

	void OnYes(wxCommandEvent& event);
	void OnNo(wxCommandEvent& event);

private:
	wxTextCtrl* m_text;

	wxStaticLine* m_verif_sep;
	wxStaticText* m_verif_lbl;
	wxTextCtrl* m_verif_text;
	wxStaticLine* m_verif_sep_2;

	wxStaticLine* m_button_sep;
	wxStaticText* m_accept_lbl;
	wxButton* m_yes_btn;
	wxButton* m_no_btn;

	std::string* verif_code;

	enum {
		AGREE_YES = wxID_HIGHEST,
		AGREE_NO
	};

	DECLARE_EVENT_TABLE()
};

#endif // SPRINGLOBBY_HEADERGUARD_AGREEMENTDIALOG_H
