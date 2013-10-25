#include "taskbar.h"
#include "../downloader/downloadsobserver.h"
#include "../utils/conversion.h"

#include <wx/colour.h>
#include <wx/statbmp.h>
#include <wx/artprov.h>
#include <wx/sizer.h>

const unsigned int TIMER_ID         = 103;

BEGIN_EVENT_TABLE(TaskBar, wxPanel)
	EVT_TIMER(TIMER_ID, TaskBar::OnTimer)
END_EVENT_TABLE()

enum {
	STATE_FINISHED,
	STATE_WORKING,
	STATE_HIDDEN
};

TaskBar::TaskBar(wxWindow *statusbar)
	:wxPanel(statusbar, wxID_ANY, wxPoint(3, 3),
			 wxSize(460 - (2 * 3), statusbar->GetSize().GetHeight()))
{
	timer = new wxTimer(this, TIMER_ID);

	wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
	SetSizer(sizer);

	wxStaticBitmap *refreshIcon = new wxStaticBitmap(
		this, wxID_ANY, wxArtProvider::GetBitmap(wxART_INFORMATION));
	sizer->Add(refreshIcon);

	text = new wxStaticText(
		this, wxID_ANY, _T(""));
	text->SetForegroundColour(wxColour(0, 0, 0));
	sizer->Add(text, 0, wxLEFT | wxALIGN_CENTER_VERTICAL, 5);

	sizer->AddStretchSpacer();

	gauge = new wxGauge(this, wxID_ANY, 100, wxPoint(-1, -1),
						wxSize(100, 14), wxGA_SMOOTH);
	sizer->Add(gauge, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 5);

	state = STATE_HIDDEN;
	Hide();

	timer->Start(100);
}

void TaskBar::UpdateDisplay()
{
	bool finished = true;

	// get status of all downloads
	unfinishedTasks = 0;
	overalSize = 0;
	overalProgress = 0;
	DownloadsObserver& observ=downloadsObserver();
	std::list<ObserverDownloadInfo> list;
	observ.GetList(list);
	std::list<ObserverDownloadInfo>::iterator it;
	for(it=list.begin(); it!=list.end(); ++it) {
		ObserverDownloadInfo info = (*it);
		if (!info.finished) {
			finished = false;
			unfinishedTasks++;
			downloadName = info.name;
		}
		overalSize+=info.size;
		overalProgress+=info.progress;
	}

	// do state transition & actions
	switch(state) {
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
			text->SetLabel(_T("Download finished"));
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
}

void TaskBar::UpdateProgress()
{
	int overalPercent;
	// set percent to -1 if unknown
	if (overalSize <= 100 ) {
		overalPercent = -1;
	} else {
		overalPercent = overalProgress / (overalSize / 100);
	}

	if (unfinishedTasks == 1 ) {
		text->SetLabel(wxFormat( _T("Downloading %s") ) % downloadName );
	} else {
		text->SetLabel(wxFormat( _T("Downloading %u files") ) % unfinishedTasks );
	}

	if (overalPercent > -1) {
		// just pulse the bar, if the progress is unknown
		gauge->SetValue(overalPercent);
	} else {
		gauge->Pulse();
	}
}


void TaskBar::OnTimer(wxTimerEvent& /*event*/)
{
	UpdateDisplay();
}
