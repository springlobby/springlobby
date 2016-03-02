/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "taskbar.h"
#include "utils/conversion.h"
#include "utils/globalevents.h"
#include "downloader/prdownloader.h"

#include <wx/colour.h>
#include <wx/statbmp.h>
#include <wx/artprov.h>
#include <wx/sizer.h>

const unsigned int TIMER_ID = wxNewId();

BEGIN_EVENT_TABLE(TaskBar, wxPanel)
EVT_TIMER(TIMER_ID, TaskBar::OnTimer)
END_EVENT_TABLE()

enum {
	STATE_FINISHED,
	STATE_WORKING,
	STATE_HIDDEN
};

TaskBar::TaskBar(wxWindow* statusbar)
    : wxPanel(statusbar, wxID_ANY, wxPoint(3, 3),
	      wxSize(460 - (2 * 3), statusbar->GetSize().GetHeight()))
    , overalSize(0)
    , overalProgress(0)
    , unfinishedTasks(0)
    , finishedCounter(0)
{
	timer = new wxTimer(this, TIMER_ID);

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

	state = STATE_HIDDEN;
	Hide();

	timer->Start(100);


	GlobalEventManager::Instance()->Subscribe(this, GlobalEventManager::OnDownloadStarted, wxObjectEventFunction());
	GlobalEventManager::Instance()->Subscribe(this, GlobalEventManager::OnDownloadComplete, wxObjectEventFunction());
}

TaskBar::~TaskBar()
{
	timer->Stop();
	wxDELETE(timer);
}

void TaskBar::OnDownloadStarted()
{
	//FIXME: implement this
	//prDownloader()::GetProgress(...)
}

void TaskBar::OnDownloadComplete()
{
}


void TaskBar::UpdateDisplay()
{
	bool finished = true;

	// get status of all downloads
	unfinishedTasks = 0;
	overalSize = 0;
	overalProgress = 0;
/*
	for (ObserverDownloadInfo info: activedownloads) {
		if (!info.finished) {
			finished = false;
			unfinishedTasks++;
			downloadName = info.name;
		}
		overalSize += info.size;
		overalProgress += info.progress;
	}
*/
	// do state transition & actions
	switch (state) {
		case STATE_FINISHED:
			if (finished) {
				// wait 5sec and hide widget
				if (finishedCounter < 50) {
					finishedCounter++;
				} else {
					state = STATE_HIDDEN;
					Hide();
					finishedCounter = 0;
				}
			} else {
				// change to STATE_WORKING
				state = STATE_WORKING;
				SetBackgroundColour(wxColour(255, 244, 168));
				Show();
				gauge->Show();
				UpdateProgress();
			}
			break;
		case STATE_WORKING:
			if (finished) {
				state = STATE_FINISHED;
				text->SetLabel(_("Download finished"));
				gauge->Hide();
				SetBackgroundColour(wxColour(0, 208, 10));
			} else {
				UpdateProgress();
			}
			break;
		case STATE_HIDDEN:
			if (finished) {
				// nop
			} else {
				// change to STATE_WORKING
				state = STATE_WORKING;
				SetBackgroundColour(wxColour(255, 244, 168));
				Show();
				gauge->Show();
				UpdateProgress();
			}
			break;
	}
	Layout();
	Refresh();
}

void TaskBar::UpdateProgress()
{
	float overalPercent = -1; // -1 means unknown
	if (overalSize > 0) {
		overalPercent = ((float)overalProgress / overalSize) * 100;
	}

	if (unfinishedTasks == 1) {
		text->SetLabel(wxString::Format(_("Downloading %s"), downloadName.c_str()));
	} else {
		text->SetLabel(wxString::Format(_("Downloading %d files"), unfinishedTasks));
	}

	if (overalPercent < 0) {
		// just pulse the bar, if the progress is unknown
		gauge->Pulse();
	} else {
		gauge->SetValue(overalPercent);
	}
}


void TaskBar::OnTimer(wxTimerEvent& /*event*/)
{
	UpdateDisplay();
}
