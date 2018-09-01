/* This file is part of the Springlobby (GPL v2 or later), see COPYING */
#include "taskbar.h"

#include <wx/artprov.h>
#include <wx/colour.h>
#include <wx/event.h>
#include <wx/gauge.h>
#include <wx/sizer.h>
#include <wx/statbmp.h>
#include <wx/stattext.h>
#include <wx/timer.h>

#include "downloader/prdownloader.h"
#include "log.h"
#include "utils/conversion.h"
#include "utils/globalevents.h"

const unsigned int TIMER_INTERVAL = 5000;
const unsigned int TIMER_ID = wxNewId();

BEGIN_EVENT_TABLE(TaskBar, wxPanel)
EVT_TIMER(TIMER_ID, TaskBar::OnTimer)
END_EVENT_TABLE()

TaskBar::TaskBar(wxWindow* statusbar)
    : wxPanel(statusbar, wxID_ANY, wxPoint(3, 3), wxSize(460 - (2 * 3), statusbar->GetSize().GetHeight()))
    , timer(nullptr)
{

	wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
	SetSizer(sizer);

	wxStaticBitmap* refreshIcon = new wxStaticBitmap(
	    this, wxID_ANY, wxArtProvider::GetBitmap(wxART_INFORMATION));
	sizer->Add(refreshIcon);

	text = new wxStaticText(this, wxID_ANY, wxEmptyString);
	text->SetForegroundColour(wxColour(0, 0, 0));
	sizer->Add(text, 0, wxLEFT | wxALIGN_CENTER_VERTICAL, 5);

	sizer->AddStretchSpacer();

	gauge = new wxGauge(this, wxID_ANY, 100, wxPoint(-1, -1),
			    wxSize(100, 14), wxGA_SMOOTH);
	sizer->Add(gauge, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 5);

	Hide();

	SUBSCRIBE_GLOBAL_EVENT(GlobalEventManager::OnDownloadStarted, TaskBar::OnDownloadStarted);
	SUBSCRIBE_GLOBAL_EVENT(GlobalEventManager::OnDownloadFailed, TaskBar::OnDownloadFailed);
	SUBSCRIBE_GLOBAL_EVENT(GlobalEventManager::OnDownloadComplete, TaskBar::OnDownloadComplete);
	SUBSCRIBE_GLOBAL_EVENT(GlobalEventManager::OnDownloadProgress, TaskBar::OnDownloadProgress);
}

TaskBar::~TaskBar()
{
	EnsureTimerRemoved();
	GlobalEventManager::Instance()->UnSubscribeAll(this);
}

void TaskBar::OnDownloadStarted(wxCommandEvent& /*event*/)
{
	slLogDebugFunc("");

	EnsureTimerRemoved();

	SetBackgroundColour(wxColour(255, 244, 168));
	Show();
	gauge->Show();

	UpdateProgress();

	Layout();
	Refresh();
}

void TaskBar::OnDownloadFailed(wxCommandEvent& /*event*/)
{
	slLogDebugFunc("");

	EnsureTimerRemoved();

	Show();

	text->SetLabel(_("Download failed"));
	gauge->Hide();
	SetBackgroundColour(wxColour(208, 10, 10));

	timer = new wxTimer(this, TIMER_ID);
	timer->Start(TIMER_INTERVAL);

	Layout();
	Refresh();
}

void TaskBar::OnDownloadComplete(wxCommandEvent& /*event*/)
{
	slLogDebugFunc("");

	EnsureTimerRemoved();

	text->SetLabel(_("Download finished"));
	gauge->Hide();
	SetBackgroundColour(wxColour(0, 208, 10));

	timer = new wxTimer(this, TIMER_ID);
	timer->Start(TIMER_INTERVAL);

	Layout();
	Refresh();
}

void TaskBar::OnDownloadProgress(wxCommandEvent& /*event*/)
{
	UpdateProgress();
}

void TaskBar::OnTimer(wxTimerEvent&)
{
	slLogDebugFunc("");

	EnsureTimerRemoved();

	Hide();

	Layout();
	Refresh();
}

void TaskBar::UpdateProgress()
{
	PrDownloader::DownloadProgress p;
	prDownloader().GetProgress(p);

	int progress = (int)p.GetProgressPercent();
	text->SetLabel(wxString::Format(_("Downloading %s"), TowxString(p.name)));
	gauge->SetValue(progress);
}

void TaskBar::EnsureTimerRemoved()
{
	if (timer != nullptr) {
		if (timer->IsRunning()) {
			timer->Stop();
		}
		wxDELETE(timer);
	}
}
