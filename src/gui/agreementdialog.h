/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_AGREEMENTDIALOG_H
#define SPRINGLOBBY_HEADERGUARD_AGREEMENTDIALOG_H

#include "generated/AgreementDialogBase.h"

/** \brief dialog showing agreement on first start/ register that user needs to accept
 * \todo DOCMEMORE */
class AgreementDialog : public AgreementDialogBase
{
public:
	AgreementDialog(wxWindow* parent, const wxString& agreement, std::string* verif_code);

	void OnYes(wxCommandEvent& event);
	void OnNo(wxCommandEvent& event);

private:
	std::string* verif_code;
};

#endif // SPRINGLOBBY_HEADERGUARD_AGREEMENTDIALOG_H
