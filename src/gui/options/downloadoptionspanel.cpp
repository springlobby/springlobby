/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifdef _MSC_VER
#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX
#include <winsock2.h>
#endif // _MSC_VER

#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/intl.h>
#include <wx/radiobut.h>
#include <wx/button.h>
#include <wx/spinctrl.h>
#include <wx/dirdlg.h>

#include "downloadoptionspanel.h"
#include "aui/auimanager.h"
#include "settings.h"
#include "downloader/prdownloader.h"
#include "utils/conversion.h"
#include "gui/controls.h"
#include "utils/slpaths.h"

BEGIN_EVENT_TABLE(DownloadOptionsPanel, wxScrolledWindow)
	EVT_BUTTON(ID_SELECT_NEW_DIRECTORY, DownloadOptionsPanel::OnNewDirectory)
END_EVENT_TABLE()

DownloadOptionsPanel::DownloadOptionsPanel(wxWindow* parent)
    : wxScrolledWindow(parent, -1)
{
	GetAui().manager->AddPane(this, wxLEFT, _T("torrentoptionspanel"));

	wxBoxSizer* m_main_sizer = new wxBoxSizer(wxVERTICAL);
	m_parallel_http = new wxSpinCtrl(this, wxID_ANY, wxString::Format(_T("%d"), sett().GetHTTPMaxParallelDownloads()));
	m_parallel_http->SetRange(1, 10);
	m_parallel_http_sizer = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Parallel HTTP Connections when downloading content"));
	m_parallel_http_sizer->Add(m_parallel_http);

	m_main_sizer->Add(m_parallel_http_sizer, 0, wxALL | wxEXPAND, 5);

	/*Download directory editing*/
	{
		wxStaticBoxSizer* outerSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Current download directory"));

		wxBoxSizer* innerSizer = new wxBoxSizer(wxVERTICAL);
			wxBoxSizer* secondInnerSizer = new wxBoxSizer(wxHORIZONTAL);
			m_DownloadDirectoryTextCtrl = new wxTextCtrl(this, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
			secondInnerSizer->Add(m_DownloadDirectoryTextCtrl, 1, wxALL | wxEXPAND, 5);
			m_NewDownloadDirButton = new wxButton(this, ID_SELECT_NEW_DIRECTORY, _("Browse"));
			secondInnerSizer->Add(m_NewDownloadDirButton, 0, wxALL, 5);
		innerSizer->Add(secondInnerSizer, 1, wxALL | wxEXPAND);

		wxStaticText* statText = new wxStaticText(this, wxID_ANY, _("May need restart if changed!"));
		statText->SetFont(wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
		innerSizer->Add(statText, 0, wxALL, 3);
		outerSizer->Add(innerSizer, 1, wxALL | wxEXPAND);

		m_main_sizer->Add(outerSizer, 0, wxALL | wxEXPAND, 5);
	}


	//the lazy man's solution to options not being set correctly at panel creation
	wxCommandEvent dummy;
	OnRestore(dummy);

	SetSizer(m_main_sizer);
	SetScrollRate(SCROLL_RATE, SCROLL_RATE);
	Layout();
}

DownloadOptionsPanel::~DownloadOptionsPanel()
{
}

void DownloadOptionsPanel::OnApply(wxCommandEvent& /*unused*/)
{

	sett().SetHTTPMaxParallelDownloads(m_parallel_http->GetValue());

	prDownloader().UpdateSettings();
}

void DownloadOptionsPanel::OnRestore(wxCommandEvent& /*unused*/)
{
	m_parallel_http->SetValue(sett().GetHTTPMaxParallelDownloads());
}

void DownloadOptionsPanel::OnNewDirectory(wxCommandEvent&) {
	wxDirDialog pick(this, _("Choose a directory for downloading"),
			  wxPathOnly(TowxString(SlPaths::GetDownloadDir())),  wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);

	if (pick.ShowModal() == wxID_OK) {
		m_DownloadDirectoryTextCtrl->SetValue(pick.GetPath());
		SlPaths::SetDownloadDir(pick.GetPath().ToStdString());
	}
}
