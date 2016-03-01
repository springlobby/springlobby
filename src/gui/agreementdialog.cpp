/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

//
// Class: AgreementDialog
//
#include "agreementdialog.h"

#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/statline.h>

BEGIN_EVENT_TABLE(AgreementDialog, wxDialog)

EVT_BUTTON(AGREE_YES, AgreementDialog::OnYes)
EVT_BUTTON(AGREE_NO, AgreementDialog::OnNo)

END_EVENT_TABLE()


AgreementDialog::AgreementDialog(wxWindow* parent, const wxString& agreement)
    : wxDialog(parent, -1, _("Accept Agreement"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
	SetSizeHints(wxDefaultSize, wxDefaultSize);

	wxBoxSizer* m_main_sizer = new wxBoxSizer(wxVERTICAL);

	m_text = new wxTextCtrl(this, wxID_ANY, agreement, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_RICH | wxTE_WORDWRAP | wxTE_READONLY);
	m_main_sizer->Add(m_text, 1, wxALL | wxEXPAND, 5);

	m_accept_lbl = new wxStaticText(this, wxID_ANY, _("Do you accept the terms of this agreement?"), wxDefaultPosition, wxDefaultSize, 0);
	m_main_sizer->Add(m_accept_lbl, 0, wxALL, 5);

	m_button_sep = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
	m_main_sizer->Add(m_button_sep, 0, wxALL | wxEXPAND, 5);

	wxBoxSizer* m_button_sizer = new wxBoxSizer(wxHORIZONTAL);

	m_no_btn = new wxButton(this, AGREE_NO, _("No"), wxDefaultPosition, wxDefaultSize, 0);
	m_button_sizer->Add(m_no_btn, 0, wxALL, 5);

	m_button_sizer->Add(0, 0, 1, wxEXPAND, 0);

	m_yes_btn = new wxButton(this, AGREE_YES, _("Yes"), wxDefaultPosition, wxDefaultSize, 0);
	m_button_sizer->Add(m_yes_btn, 0, wxALL, 5);

	m_main_sizer->Add(m_button_sizer, 0, wxEXPAND, 5);

	m_no_btn->SetDefault();

	SetSizer(m_main_sizer);
	Layout();
}


void AgreementDialog::OnYes(wxCommandEvent& /*unused*/)
{
	EndModal(wxID_OK);
}


void AgreementDialog::OnNo(wxCommandEvent& /*unused*/)
{
	EndModal(wxID_CANCEL);
}
