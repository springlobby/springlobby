/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

//
// Class: CrashReporterDialog
//
#include "crashreporterdialog.h"

#include <wx/filename.h>
#include <wx/log.h>
#include <wx/string.h>
#include <wx/txtstrm.h>
#include <wx/wfstream.h>

#include "uiutils.h"

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
	}

	wxFileInputStream input(filePath);
	wxTextInputStream text(input);

	wxTextAttr defaultStyle = m_report_file_text->GetDefaultStyle();
	while (!input.Eof()) {
		wxString line(text.ReadLine());

		int pos = line.Find(" Warning");
		if (wxNOT_FOUND != pos) {
			m_report_file_text->SetDefaultStyle(wxTextAttr(wxColour(255, 175, 0)));
		}

		pos = line.Find(" Error");
		if (wxNOT_FOUND != pos) {
			m_report_file_text->SetDefaultStyle(wxTextAttr(*wxRED));
		}

		m_report_file_text->AppendText(line);
		m_report_file_text->AppendText("\n");
		m_report_file_text->SetDefaultStyle(defaultStyle);
	}

	m_safe_button->SetFocus();

}


void CrashReporterDialog::OnCancel(wxCommandEvent& /*unused*/)
{
	EndModal(CANCEL);
}


void CrashReporterDialog::OnClickBugReport(wxCommandEvent& event)
{
	// The URL should be in tooltip!
	wxButton* button = (wxButton*)event.GetEventObject();
	const wxString& url = button->GetToolTipText();
	wxLogMessage(_T("Open URL: %s"), url);
	OpenWebBrowser(url);
}


void CrashReporterDialog::OnNorm(wxCommandEvent& /*unused*/)
{
	EndModal(RERUN_NORMAL);
}


void CrashReporterDialog::OnSafe(wxCommandEvent& /*unused*/)
{
	EndModal(RERUN_SAFE);
}


int CrashReporterDialog::RunCrashReporterDialog(wxWindow* parent, const wxString& heading, const wxString& message, const wxString& filePath)
{
	CrashReporterDialog dialog(parent, heading, message, filePath);
	return dialog.ShowModal();
}
