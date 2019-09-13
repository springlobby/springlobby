/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

//
// Class: CrashReporterDialog
//
#include "crashreporterdialog.h"

CrashReporterDialog::CrashReporterDialog(wxWindow* parent, const wxString& heading, const wxString& message)
    : CrashReporterDialogBase(parent)
    , WindowHintsPickle(_T("crashreporterdialog"), this, wxSize(410, 520))
{
	SetLabel(heading);
	m_report_text->ChangeValue(message);
	m_ok_button->SetFocus();
}


void CrashReporterDialog::OnOk(wxCommandEvent& /*unused*/)
{
	EndModal(wxID_OK);
}


void CrashReporterDialog::OnCancel(wxCommandEvent& /*unused*/)
{
	EndModal(wxID_CANCEL);
}


int CrashReporterDialog::RunCrashReporterDialog(wxWindow* parent, const wxString& heading, const wxString& message)
{
	CrashReporterDialog dialog(parent, heading, message);
	return dialog.ShowModal();
}
