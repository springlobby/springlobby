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

#include "downloadoptionspanel.h"
#include "aui/auimanager.h"
#include "settings.h"
#include "downloader/prdownloader.h"
#include "utils/conversion.h"
#include "gui/controls.h"


BEGIN_EVENT_TABLE(TorrentOptionsPanel, wxScrolledWindow)
END_EVENT_TABLE()

TorrentOptionsPanel::TorrentOptionsPanel(wxWindow* parent)
    : wxScrolledWindow(parent, -1)
{
	GetAui().manager->AddPane(this, wxLEFT, _T("torrentoptionspanel"));

	m_main_sizer = new wxBoxSizer(wxVERTICAL);
	m_parallel_http = new wxSpinCtrl(this, wxID_ANY, wxString::Format(_T("%d"), sett().GetHTTPMaxParallelDownloads()));
	m_parallel_http->SetRange(1, 10);
	m_parallel_http_sizer = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Parallel HTTP Connections when downloading content"));
	m_parallel_http_sizer->Add(m_parallel_http);


	m_main_sizer->Add(m_parallel_http_sizer, 0, wxALL | wxEXPAND, 5);

	//the lazy man's solution to options not being set correctly at panel creation
	wxCommandEvent dummy;
	OnRestore(dummy);

	SetSizer(m_main_sizer);
	SetScrollRate(SCROLL_RATE, SCROLL_RATE);
	Layout();
}

TorrentOptionsPanel::~TorrentOptionsPanel()
{
	wxDELETE(m_main_sizer);
}

void TorrentOptionsPanel::OnApply(wxCommandEvent& /*unused*/)
{

	sett().SetHTTPMaxParallelDownloads(m_parallel_http->GetValue());

	prDownloader().UpdateSettings();
}

void TorrentOptionsPanel::OnRestore(wxCommandEvent& /*unused*/)
{
	m_parallel_http->SetValue(sett().GetHTTPMaxParallelDownloads());
}
