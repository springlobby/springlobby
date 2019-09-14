/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

//
// Class: CrashReporterDialog
//
#include "crashreporterdialog.h"

#include <wx/filename.h>
#include <wx/string.h>


CrashReporterDialog::CrashReporterDialog(wxWindow* parent, const wxString& heading, const wxString& message, const wxString& filePath)
    : CrashReporterDialogBase(parent)
    , WindowHintsPickle(_T("crashreporterdialog"), this, wxSize(410, 520))
{
	SetLabel(heading);
	m_report_text->ChangeValue(message);
	m_report_text->InvalidateBestSize();
	m_report_text->SetClientSize(m_report_text->GetBestSize());

	m_report_file_name_label->SetLabel(wxString::Format(_("Contents of %s:"), filePath));

	if (filePath.empty()) {
		m_report_file_text->AppendText(_T("Error loading file: none given"));
	} else if (wxNOT_FOUND == filePath.Find(wxFileName::GetPathSeparator())) {
		m_report_file_text->AppendText(_T("Error loading file: directory separator missing, is engine write path known?"));
	} else if (!wxFileExists(filePath)) {
		m_report_file_text->AppendText(_T("Error loading file: it does not exist."));
	} else if (!wxFileName::IsFileReadable(filePath)) {
		m_report_file_text->AppendText(_T("Error loading file: it is not readable."));
	} else if (!m_report_file_text->LoadFile(filePath)) {
		m_report_file_text->AppendText(_T("Error loading file: unspecified error."));
	}

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


int CrashReporterDialog::RunCrashReporterDialog(wxWindow* parent, const wxString& heading, const wxString& message, const wxString& filePath)
{
	CrashReporterDialog dialog(parent, heading, message, filePath);
	return dialog.ShowModal();
}
