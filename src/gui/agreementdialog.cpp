/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

//
// Class: AgreementDialog
//
#include "agreementdialog.h"
#include "utils/conversion.h"

AgreementDialog::AgreementDialog(wxWindow* parent, const wxString& agreement, std::string* code)
	: AgreementDialogBase(parent)
{
	verif_code = code;

	m_text->SetValue(agreement);
}


void AgreementDialog::OnYes(wxCommandEvent& /*unused*/)
{
	*verif_code = STD_STRING(m_verif_text->GetValue());
	EndModal(wxID_OK);
}


void AgreementDialog::OnNo(wxCommandEvent& /*unused*/)
{
	EndModal(wxID_CANCEL);
}
